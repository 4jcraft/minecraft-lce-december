#include "../../Platform/stdafx.h"
#include "../../Platform/System.h"
#include "../../Headers/net.minecraft.world.entity.player.h"
#include "../../Headers/net.minecraft.world.level.h"
#include "../../Headers/net.minecraft.world.level.chunk.storage.h"
#include "../../Headers/net.minecraft.world.level.dimension.h"
#include "../../Headers/com.mojang.nbt.h"
#include "../../IO/Files/File.h"
#include "../../IO/Streams/DataInputStream.h"
#include "../../IO/Files/FileInputStream.h"
#include "../LevelData.h"
#include "DirectoryLevelStorage.h"
#include "../../IO/Files/ConsoleSaveFileIO.h"

const std::wstring DirectoryLevelStorage::sc_szPlayerDir(L"players/");

_MapDataMappings::_MapDataMappings() {
#ifndef _DURANGO
    ZeroMemory(xuids, sizeof(PlayerUID) * MAXIMUM_MAP_SAVE_DATA);
#endif
    ZeroMemory(dimensions, sizeof(uint8_t) * (MAXIMUM_MAP_SAVE_DATA / 4));
}

int _MapDataMappings::getDimension(int id) {
    int offset = (2 * (id % 4));
    int val = (dimensions[id >> 2] & (3 << offset)) >> offset;

    int returnVal = 0;

    switch (val) {
        case 0:
            returnVal = 0;  // Overworld
            break;
        case 1:
            returnVal = -1;  // Nether
            break;
        case 2:
            returnVal = 1;  // End
            break;
        default:
#ifndef _CONTENT_PACKAGE
            printf("Read invalid dimension from MapDataMapping\n");
            __debugbreak();
#endif
            break;
    }
    return returnVal;
}

void _MapDataMappings::setMapping(int id, PlayerUID xuid, int dimension) {
    xuids[id] = xuid;

    int offset = (2 * (id % 4));

    // Reset it first
    dimensions[id >> 2] &= ~(2 << offset);
    switch (dimension) {
        case 0:  // Overworld
            // dimensions[id>>2] &= ~( 2 << offset );
            break;
        case -1:  // Nether
            dimensions[id >> 2] |= (1 << offset);
            break;
        case 1:  // End
            dimensions[id >> 2] |= (2 << offset);
            break;
        default:
#ifndef _CONTENT_PACKAGE
            printf(
                "Trinyg to set a MapDataMapping for an invalid dimension.\n");
            __debugbreak();
#endif
            break;
    }
}

// Old version the only used 1 bit for dimension indexing
_MapDataMappings_old::_MapDataMappings_old() {
#ifndef _DURANGO
    ZeroMemory(xuids, sizeof(PlayerUID) * MAXIMUM_MAP_SAVE_DATA);
#endif
    ZeroMemory(dimensions, sizeof(uint8_t) * (MAXIMUM_MAP_SAVE_DATA / 8));
}

int _MapDataMappings_old::getDimension(int id) {
    return dimensions[id >> 3] & (128 >> (id % 8)) ? -1 : 0;
}

void _MapDataMappings_old::setMapping(int id, PlayerUID xuid, int dimension) {
    xuids[id] = xuid;
    if (dimension == 0) {
        dimensions[id >> 3] &= ~(128 >> (id % 8));
    } else {
        dimensions[id >> 3] |= (128 >> (id % 8));
    }
}

#ifdef _LARGE_WORLDS
void DirectoryLevelStorage::PlayerMappings::addMapping(int id, int centreX,
                                                       int centreZ,
                                                       int dimension,
                                                       int scale) {
    __int64 index = (((__int64)(centreZ & 0x1FFFFFFF)) << 34) |
                    (((__int64)(centreX & 0x1FFFFFFF)) << 5) |
                    ((scale & 0x7) << 2) | (dimension & 0x3);
    m_mappings[index] = id;
    // app.DebugPrintf("Adding mapping: %d - (%d,%d)/%d/%d [%I64d -
    // 0x%016llx]\n", id, centreX, centreZ, dimension, scale, index, index);
}

bool DirectoryLevelStorage::PlayerMappings::getMapping(int& id, int centreX,
                                                       int centreZ,
                                                       int dimension,
                                                       int scale) {
    //__int64 zMasked = centreZ & 0x1FFFFFFF;
    //__int64 xMasked = centreX & 0x1FFFFFFF;
    //__int64 zShifted = zMasked << 34;
    //__int64 xShifted = xMasked << 5;
    // app.DebugPrintf("xShifted = %d (0x%016x), zShifted = %I64d
    // (0x%016llx)\n", xShifted, xShifted, zShifted, zShifted);
    __int64 index = (((__int64)(centreZ & 0x1FFFFFFF)) << 34) |
                    (((__int64)(centreX & 0x1FFFFFFF)) << 5) |
                    ((scale & 0x7) << 2) | (dimension & 0x3);
    AUTO_VAR(it, m_mappings.find(index));
    if (it != m_mappings.end()) {
        id = it->second;
        // app.DebugPrintf("Found mapping: %d - (%d,%d)/%d/%d [%I64d -
        // 0x%016llx]\n", id, centreX, centreZ, dimension, scale, index, index);
        return true;
    } else {
        // app.DebugPrintf("Failed to find mapping: (%d,%d)/%d/%d [%I64d -
        // 0x%016llx]\n", centreX, centreZ, dimension, scale, index, index);
        return false;
    }
}

void DirectoryLevelStorage::PlayerMappings::writeMappings(
    DataOutputStream* dos) {
    dos->writeInt(m_mappings.size());
    for (AUTO_VAR(it, m_mappings.begin()); it != m_mappings.end(); ++it) {
        app.DebugPrintf("    -- %lld (0x%016llx) = %d\n", it->first, it->first,
                        it->second);
        dos->writeLong(it->first);
        dos->writeInt(it->second);
    }
}

void DirectoryLevelStorage::PlayerMappings::readMappings(DataInputStream* dis) {
    int count = dis->readInt();
    for (unsigned int i = 0; i < count; ++i) {
        __int64 index = dis->readLong();
        int id = dis->readInt();
        m_mappings[index] = id;
        app.DebugPrintf("    -- %lld (0x%016llx) = %d\n", index, index, id);
    }
}
#endif

DirectoryLevelStorage::DirectoryLevelStorage(ConsoleSaveFile* saveFile,
                                             const File dir,
                                             const std::wstring& levelId,
                                             bool createPlayerDir)
    : sessionId(System::currentTimeMillis()),
      dir(L""),
      playerDir(sc_szPlayerDir),
      dataDir(std::wstr"data/")),
      levelId(levelId) {
    m_saveFile = saveFile;
    m_bHasLoadedMapDataMappings = fa
#ifdef _LARGE_WORLDS
    m_usedMappings = byteArray(MAXIMUM_MAP_SAVE_DATA / #endif
}

DirectoryLevelStorage::~DirectoryLevelStorage() {
    delete m_saveFile;

    for (AUTO_VAR(it, m_cachedSaveData.begin()); it != m_cachedSaveData.end();
         ++it) {
        delete it->second;

#ifdef _LARGE_WORLDS
        delete m_usedMappings.d #endif
    }

    void DirectoryLevelStorage::initiateSession() {  // 4J Jev, removed
                                                     // try/catch.

    File dataFile = File(dir, std:"session.lock"));
    FileOutputStream fos = FileOutputStream(dataFile);
    DataOutputStream dos = DataOutputStream(&fos);
    dos.writeLong(sessionId);
    dos.close();
    }

    File DirectoryLevelStorage::getFolder() { return dir; }

    void DirectoryLevelStorage::checkSessio  // 4J-PB - Not in the Xbox game/*
        File dataFile = File(dir, wstring(L"session.lock"));
    FileInputStream fis = FileInputStream(dataFile);
    DataInputStream dis = DataInputStream(&fis);
    dis.close();
    */
}

ChunkStorage* DirectoryLevelStorage::createChunkStorage(Dimension* dimensi// 4J Jev, removed try/catch.

    if (dynamic_cast<HellDimension*>(dimension) != NULL) {
    File dir2 = File(dir, LevelStorage::NETHER_FOLDER)  // dir2.mkdirs(); // 4J
                                                        // Removed
        return new OldChunkStorage(dir2, true);
    }
    if (dynamic_cast<TheEndDimension*>(dimension) != NULL) {
    File dir2 = File(dir, LevelStorage::ENDER_FOLDER)  // dir2.mkdirs(); // 4J
                                                       // Removed
        return new OldChunkStorage(dir2, true);
    }

    return new OldChunkStorage(dir, true);
}

LevelData* DirectoryLevelStorage::prepareLeve// 4J Stu Added#ifdef _LARGE_WORLDS
    ConsoleSavePath mapFile = get"largeMapDataMappings"#else);

    ConsoleSavePath mapFile = get"mapDataMappings"#endif);

    if (!m_bHasLoadedMapDataMappings && !mapFile.getName().empty() &&
        getSaveFile()->doesFileExist(mapFile)) {
        DWORD NumberOfBytesRead;
        FileEntry* fileEntry = getSaveFile()->createFile(
#ifdef __PS3__  // 4J Stu - This version changed happened before initial
                        // release
        if (getSaveFile()->getSaveVersion() <
            SAVE_FILE_VERSION_CHANGE_MAP_DATA_MAPPING_SIZE) {
        // Delete the old file
        if (fileEntry) getSaveFile()->deleteFile(fileEntry);

        // Save a new, blank version
        saveMapIdLookup();
#elif defined(_DURANGO)  // 4J Stu - This version changed happened
                                 // before initial release
        if (getSaveFile()->getSaveVersion() <
            SAVE_FILE_VERSION_DURANGO_CHANGE_MAP_DATA_MAPPING_SIZE) {
  // Delete the old file
            if (fileEntry) getSaveFile()->deleteFile(fileEntry);

  // Save a new, blank version
            saveMapIdLookup();
#endiflse

        {
            getSaveFile()->setFilePointer(fileEntry, 0, NULL, FIL
#ifdef _LARGE_WORLDS
            byteArray data(fileEntry->getFileSize());
            getSaveFile()->readFile(fileEntry, data.data,
                                    fileEntry->getFileSize(),
                                    &NumberOfBytesRead);
            assert(NumberOfBytesRead == fileEntry->getFileSize());

            ByteArrayInputStream bais(data);
            DataInputStream dis(&bais);
            int count = dis.readInt();
            app.De"Loading %d mappings\n", count);
            for (unsigned int i = 0; i < count; ++i) {
                PlayerUID playerUid = dis.readPla#ifdef _WINDOWS64
                app.De"  -- %d\n", pl#elseid);

                app.De"  -- %ls\n", playerUid.toString().#endif));

                m_playerMappings[playerUid].readMappings(&dis);
            }
            dis.readFully(m_usedM#elsegs);


            if (getSaveFile()->getSaveVersion() <
                END_DIMENSION_MAP_MAPPINGS_SAVE_VERSION) {
                MapDataMappings_old oldMapDataMappings;
                getSaveFile()->readFile(
                    fileEntry,
                    &oldMapDataMappings,// data buffer
                    sizeof(MapDataMappin// number of bytes to read
                    &NumberOfBytesRead  // number of bytes read
                );
                assert(NumberOfBytesRead == sizeof(MapDataMappings_old));

                for (unsigned int i = 0; i < MAXIMUM_MAP_SAVE_DATA; ++i) {
                    m_saveableMapDataMappings.setMapping(
                        i, oldMapDataMappings.xuids[i],
                        oldMapDataMappings.getDimension(i));
                }
            } else {
                getSaveFile()->readFile(
                    fileEntry,
                    &m_saveableMapDataM// data buffer
                    sizeof(MapDataMappi// number of bytes to read
                    &NumberOfBytesRead // number of bytes read
                );
                assert(NumberOfBytesRead == sizeof(MapDataMappings));
            }

            memcpy(&m_mapDataMappings, &m_saveableMapDataMappings,
                   sizeof(MapDataMa#endif));


  // Write out our changes now
            if (getSaveFile()->getSaveVersion() <
                END_DIMENSION_MAP_MAPPINGS_SAVE_VERSION)
                saveMapIdLookup();
        }

        m_bHasLoadedMapDataMappings = true;
 // 4J Jev, removed try/catch

    ConsoleSavePath dataFile = ConsoleSavePath"level.dat"ng(L));

    if (m_saveFile->doesFileExist(dataFile)) {
        ConsoleSaveFileInputStream fis =
            ConsoleSaveFileInputStream(m_saveFile, dataFile);
        CompoundTag* root = NbtIo::readCompressed(&fis);
        CompoundTag* tag = root"Data"ompound(L);
        LevelData* ret = new LevelData(tag);
        delete root;
        return ret;
    }

    return NULL;
}

void DirectoryLevelStorage::saveLevelData(
    LevelData* levelData, std::vector<std::shared_ptr<P// 4J Jev, removed try/catch

    CompoundTag* dataTag = levelData->createTag(players);

    CompoundTag* root = new Compo"Data"();
    root->put(L, dataTag);

    ConsoleSavePath currentFile = "level.dat"Path(std::wstring(L));

    ConsoleSaveFileOutputStream fos =
        ConsoleSaveFileOutputStream(m_saveFile, currentFile);
    NbtIo::writeCompressed(root, &fos);

    delete root;
}

void DirectoryLevelStorage::saveLevelDat// 4J Jev, removed try/catch  

    CompoundTag* dataTag = levelData->createTag();

    CompoundTag* root = new "Data"ndTag();
    root->put(L, dataTag);

    ConsoleSavePath currentFi"level.dat"eSavePath(std::wstring(L));

    ConsoleSaveFileOutputStream fos =
        ConsoleSaveFileOutputStream(m_saveFile, currentFile);
    NbtIo::writeCompressed(root, &fos);

    delete root;
}

void DirectoryLevelStorage::save(std// 4J Jev, removed try/catch.{
    
    PlayerU#if defined(__PS3__) || defined(__ORBIS__)
#else(playerXuid != INVALID_XUID)

    if (playerXuid != I#endif_XUID && !player->isGuest())

    {
        CompoundTag* tag = new CompoundTag();
#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__)
        ConsoleSavePath realFile = ConsoleSavePath(
            m_saveFile->getPlayerDataFil#elif defined(_DURANGO)).c_str());

        ConsoleSavePath realFile = ConsoleSavePath(
            playerDir.getName()".dat"yer#elseXuid().toString() + L);

        ConsoleSavePath realFile = ConsoleSavePath(
            playerDir.getName()".dat"Str#endifayer->get// If saves are disabled (e.g. because we are writing the save buffer to// disk) then cache this player data
        if (StorageManager.GetSaveDisabled()) {
            ByteArrayOutputStream* bos = new ByteArrayOutputStream();
            NbtIo::writeCompressed(tag, bos);

            AUTO_VAR(it, m_cachedSaveData.find(realFile.getName()));
            if (it != m_cachedSaveData.end()) {
                delete it->second;
            }
            m_cachedSaveData[realFile.getName()] = bos;
          "Cached saving of file %ls due to saves being disabled\n",
                realFile.getName().c_str());
        } else {
            ConsoleSaveFileOutputStream fos =
                ConsoleSaveFileOutputStream(m_saveFile, realFile);
            NbtIo::writeCompressed(tag, &fos);
        }
        delete tag;
    } else if (playerXuid != INVAL"Not saving player as their XUID is a guest\n");
        dontSaveMapMapp// 4J Changed return val to bool to check if new player or loaded player
CompoundTag* DirectoryLevelStorage::load(std::shared_ptr<Player> player) {
    CompoundTag* tag = loadPlayerDataTag(player->getXuid());
    if (tag != NULL) {
        player->load(tag);
    }
    return tag;
}

CompoundTag* DirectoryLevelStorage::loadP// 4J Jev, removed try/catch.{#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__)
    ConsoleSavePath realFile =
        ConsoleSavePath(m_saveFile->getPlayerD#elif defined(_DURANGO)).c_str());

    ConsoleSavePath realFile =
        ConsoleSavePath(player".dat"tNa#else+ xuid.toString() + L);

    ConsoleSavePath realFile =
        ConsoleSavePath(player".dat"tNa#endif _toString(xuid) + L);

    AUTO_VAR(it, m_cachedSaveData.find(realFile.getName()));
    if (it != m_cachedSaveData.end()) {
        ByteArrayOutputStream* bos = it->second;
        ByteArrayInputStream bis(bos->buf, 0, bos->size());
        CompoundTag* tag = NbtIo::readCompressed(&bis);
        bi"Loaded player data from cached file %ls\n",
                        realFile.getName().c_str());
        return tag;
    } else if (m_saveFile->doesFileExist(realFile)) {
        ConsoleSaveFileInputStream fis =
            ConsoleSaveFileInputStream(m_saveFile, realFile);
        return NbtIo::readCompresse// 4J Added functioneturn NULL;
}


void DirectoryLevelStorage::clearOldPlayerFiles() {
    if (Storage
#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__)
    std::vector<FileEntry*> *                 \
        playerFiles #elseaveFile->getValidPlayerDatFiles();

    std::vector<FileEntry*>* playerFiles =
        m_saveFi#endiftFilesWithPrefix(playerDir.getNam#ifndef _FINAL_BUILD (playerFiles != NULL) {

        if (app.DebugSettingsOn() &&
            app.GetGameSettingsDebugMask(ProfileManager.GetPrimaryPad()) &
                (1L << eDebugSetting_DistributableSave)) {
            for (unsigned int i = 0; i < playerFiles->size(); ++i) {
                FileEntry* file = playerFiles->at(i);
                std::wstring xuidStr = replaceAll(
                    repl""eAll(file->data.filename".dat"erD"".ge#if defined(__PS3__) || defined(__ORBIS__) || defined(_DURANGO)#else
                PlayerUID xuid(xuidStr);

#endifUID xuid = _fromString < PlayerUID>(xuidStr);

                deleteMapFilesForPlayer(xuid);
                m_saveFile->deleteFile(#endifFiles->at(i));
            }
        } else

            if (playerFiles->size() > MAX_PLAYER_DATA_SAVES) {
            std::sort(playerFiles->begin(), playerFiles->end(),
                 FileEntry::newestFirst);

            for (unsigned int i = MAX_PLAYER_DATA_SAVES;
                 i < playerFiles->size(); ++i) {
                FileEntry* file = playerFiles->at(i);
                std::wstring xuidStr = replaceAll(
              ""    replaceAll(file->dat".dat"nam"" pl#if defined(__PS3__) || defined(__ORBIS__) || defined(_DURANGO)#else
                PlayerUID xuid(xuidStr);

#endif PlayerUID xuid = _fromString < PlayerUID>(xuidStr);

                deleteMapFilesForPlayer(xuid);
                m_saveFile->deleteFile(playerFiles->at(i));
            }
        }

        delete playerFiles;
    }
}

PlayerIO* DirectoryLevelStorage::getPlayerIO() { return this; }

void DirectoryLevelStorage::closeAll() {}

ConsoleSavePath DirectoryLevelStorage::getDataFile(const std::wstring&".dat"
    return ConsoleSavePath(dataDir.getName() + id + L);
}

std::wstring DirectoryLevelStorage::getLevelId() { return levelId; }
#ifndef _CONTENT_PACKAGEage::flushSaveFile(bool autosave) {

    if (app.DebugSettingsOn() &&
        app.GetGameSettingsDebugMask(ProfileManager.GetPrimaryPad()) &
   // Delete gamerules files if it existstableSave)) {
        
        ConsoleSavePath gameRulesFiles(GAME_RULE_SAVENAME);
        if (m_saveFile->doesFileExist(gameRulesFiles)) {
            FileEntry* fe = m_saveFile->createFile(gameRulesFile#endif          m_saveFile->deleteFile(fe);// 4J Added    }

    m_saveFile->Flush(autosave);
}


void DirectoryLevelStorage::res#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__)
        std::vector<#elsentry*>* playerFiles = m_saveFile->getValidPlayerDatFiles();

        std::vector<FileEntry*>* playerFil#endif           m_saveFile->getFilesWithPrefix(playerDir.getName());


        if (playerFiles != NULL) {
            for (AUTO_VAR(it, playerFiles->begin()); it != playerFiles->end();
                 ++it) {
                FileEntry* realFile = *it;
                ConsoleSaveFileInputStream fis =
                    ConsoleSaveFileInputStream(m_saveFile, realFile);
                CompoundTag* tag = NbtIo::readCom// If the player is in the nether, set their y position               // above the top of the nether This will force the player to          // be spawned in a valid position in the overworld when they          // are loaded
   "Dimension"      
                    if"Dimension"ains(L) &&
                        tag->getInt(L) ==
                            "Pos"Data::DIMENSION_NETHER &&
                        tag->contains(L)) {
                        ListTag<DoubleTag>*"Pos"=
                            (ListTag<DoubleTag>*)tag->getList(L);
                        pos->get(1)->data = DBL_MAX;

                        ConsoleSaveFileOutputStream fos =
                            ConsoleSaveFileOutputStream(m_saveFile, realFile);
                        NbtIo::writeCompressed(tag, &fos);
                    }
                    delete tag;
                }
            }
            delete playerFiles;
        }
    }
}

int DirectoryLevelStorage::getAuxValueForMap(PlayerUID xuid, int dimension,
                                             int centreXC, int centreZC,
#ifdef _LARGE_WORLDS int mapId = -1;
    bool foundMapping = false;

    AUTO_VAR(it, m_playerMappings.find(xuid));
    if (it != m_playerMappings.end()) {
        foundMapping =
            it->second.getMapping(mapId, centreXC, centreZC, dimension, scale);
    }

    if (!foundMapping) {
        for (unsigned int i = 0; i < m_usedMappings.length; ++i) {
            if (m_usedMappings[i] < 0xFF) {
                unsigned int offset = 0;
                for (; offset < 8; ++offset) {
                    if (!(m_usedMappings[i] & (1 << offset))) {
                        break;
                    }
                }
                mapId = (i * 8) + offset;
                m_playerMappings[xuid].addMapping(mapId, centreXC, centreZC,
                                                  dimension, scale);
                m_usedMappings[i] |#else<< offset);
                break;
            }
        }
    }

    for (unsigned int i = 0; i < MAXIMUM_MAP_SAVE_DATA; ++i) {
        if (m_mapDataMappings.xuids[i] == xuid &&
            m_mapDataMappings.getDimension(i) == dimension) {
            foundMapping = true;
            mapId = i;
            break;
        }
        if (mapId < 0 && m_mapDataMappings.xuids[i] == INVALID_XUID) {
            mapId = i;
        }
    }
    if (!foundMapping && mapId >= 0 && mapId < MAXIMUM_MAP_SAVE_DATA) {
        m_mapDataMappings.setMapping(mapId, xuid, dimension);
        m// If we had an old map file for a mapping that is no longer valid,   // delete it"map_"
        
        std::wstring id = std::wstring(L) + _toString(mapId);
        ConsoleSavePath file = getDataFile(id);

        if (m_saveFile->doesFileExist(file)) {
            AUTO_VAR(it, std::find(m_mapFilesToDelete.begin(),
                              m_mapFilesToDelete.end(), mapId));
            if (it != m_mapFilesToDelete.end()) m_mapFilesToDelete.erase(it);

#endif m_saveFile->deleteFile(m_saveFile->createFile(file));
        }
    }

    return mapId;
}

void DirectoryLeve
#ifdef _LARGE_WORLDSokup() {
    if (StorageManager.GetSaveDi"largeMapDataMappings"#else
    ConsoleSavePath file = getD"mapDataMappings"#endif);

    ConsoleSavePath file = getDataFile(L);


    if (!file.getName().empty()) {
        DWORD NumberOfBytesWritten;
        FileEntry* fileEntry = m_saveFile->create
#ifdef _LARGE_WORLDS_saveFile->setFilePointer(fileEntry, 0, NULL, \
                                                      FILE_BEGIN);

        ByteArrayOutputStream baos;
        DataOutputStream dos"Saving %d mappings\n"riteInt(m_playerMappings.size());
        app.DebugPrintf(, m_playerMappings.size());
        for (AUTO_VAR(it#ifdef _WINDOWS64s.begin());
             it !"  -- %d\n"appings.end();#else) {

       "  -- %ls\n"ugPrintf(, it->first);#endif
            app.DebugPrintf(, it->first.toString().c_str());

            dos.writePlayerUID(it->first);
            it->second.writeMappings(&dos);
        }
        dos.write(m_usedMappings);
        m_save// data buffere(fileEntry,
                              baos.buf.da// number of bytes to write                              baos.size(),           // number of bytes written
#else& NumberOfBytesWritten  
        );

        m_saveF// data buffer(
            fileEntry,
            &m_sa// number of bytes to write
            sizeof(MapDataMapp// number of bytes written
            &NumberOfBytesWritten        #endif      );
        assert(NumberOfBytesWritten == sizeof(MapDataMappings));

#ifdef _LARGE_WORLDSLevelStorage::dontSaveMapMappingForPlayer( \
            PlayerUID xuid) {

    AUTO_VAR(it, m_playerMappings.find(xuid));
    if (it != m_playerMappings.end()) {
        for (AUTO_VAR(itMap, it->second.m_mappings.begin());
             itMap != it->second.m_mappings.end(); ++itMap) {
            int index = itMap->second / 8;
            int offset = itMap->second % 8;
            m_usedMapping#elseex] &= ~(1 << offset);
        }
        m_playerMappings.erase(it);
    }

    for (unsigned int i = 0; i < MAXIMUM_MAP_SAVE_DATA; ++i) {
        if (m_saveableMapDataMappings.xuids[i] == xuid) {
#endifm_saveableMapDataMappings.setMapping(i, INVALID_XUID, 0);
        }
    }

}

void DirectoryLevelStorage::deleteMapFilesForPlayer(
    std::shared_ptr<Player> player) {
    PlayerUID playerXuid = player->getXuid();
    if (playerXuid != INVALID_XUID) deleteMapFilesForPlayer(player#ifdef _LARGE_WORLDStoryLevelStorage::deleteMapFilesForPlayer(PlayerUID xuid) {

    AUTO_VAR(it, m_playerMappings.find(xuid));
    if (it != m_playerMappings.end()) {
        for (AUTO_VAR(itMap, it->second.m_mappings.begin());
             itMap !="map_"econd.m_mappings.end(); ++itMap) {
            std::wstring id = std::wstring(L) + _toString(itMap->second);
            ConsoleSavePath file = // If we can't actually delete this file, store the name so wele)) {
          // can delete it later
                
                if (StorageManager.GetSaveDisabled())
                    m_mapFilesToDelete.push_back(itMap->second);
                else
                    m_saveFile->deleteFile(m_saveFile->createFile(file));
            }

            int index = itMap->second / 8;
            int offset = itMap->second % 8;
            m_usedMa#elses[index] &= ~(1 << offset);
        }
        m_playerMappings.erase(it);
    }

    bool changed = false;
    for (unsigned int i = 0; i < MAXIMUM_MAP_SAVE_DATA; ++i) {
        if (m_mapDataMappings.xui"map_"== xuid) {
            changed = true;

            std::wstring id = std::wstring(L) + _toString(i);
            ConsoleSavePath fi// If we can't actually delete this file, store the name so west(file)) {
     // can delete it later
                
                if (StorageManager.GetSaveDisabled())
                    m_mapFilesToDelete.push_back(i);
                else
                    m_saveFile->deleteFile(m_saveFile->createFile(file));
            }
            m_mapDataMappings.setMapping(i, INVALID_XUID, 0);
            m_s#endifeMapDataMappings.setMapping(i, INVALID_XUID, 0);
            break;
        }
    }

}

void DirectoryL// Save any files that were saved while saving was disabledetSaveDisabled()) return;

    
    for (AUTO_VAR(it, m_cachedSaveData.begin()); it != m_cachedSaveData.end();
         ++it) {
        ByteArrayOutputStream* bos = it->second;

        ConsoleSavePath realFile = ConsoleSavePath(it->first);
        ConsoleSaveFileOutputStream fos "Actually writing cached file %ls\n"tream(m_saveFile, realFile);

        app.DebugPrintf(,
                        it->first.c_str());
        fos.write(bos->buf, 0, bos->size());
        delete bos;
    }
    m_cachedSaveData.clear();

    for (AUTO_VAR(it, m_mapFilesToDelete.begi"map_"         it != m_mapFilesToDelete.end(); ++it) {
        std::wstring id = std::wstring(L) + _toString(*it);
        ConsoleSavePath file = getDataFile(id);
        if (m_saveFile->doesFileExist(file)) {
            m_saveFile->deleteFile(m_saveFile->createFile(file));
        }
    }
    m_mapFilesToDelete.clear();
}
