#include "../../stdafx.h"

#include <unordered_set>

#include "../../../../Minecraft.World/Util/StringHelpers.h"
#include "../../../../Minecraft.World/Util/Pos.h"
#include "../../../../Minecraft.World/Headers/net.minecraft.world.phys.h"
#include "../../../../Minecraft.World/Headers/net.minecraft.world.level.h"
#include "../../../../Minecraft.World/Headers/net.minecraft.world.level.chunk.h"
#include "Common/DLC/DLCGameRulesHeader.h"
#include "../../StringTable.h"
#include "LevelGenerationOptions.h"
#include "ConsoleGameRules.h"

JustGrSource::JustGrSource() {
    m_displayName = L"Default_DisplayName";
    m_worldName = L"Default_WorldName";
    m_defaultSaveName = L"Default_DefaultSaveName";
    m_bRequiresTexturePack = false;
    m_requiredTexturePackId = 0;
    m_grfPath = L"__NO_GRF_PATH__";
    m_bRequiresBaseSave = false;
}

bool JustGrSource::requiresTexturePack() { return m_bRequiresTexturePack; }
UINT JustGrSource::getRequiredTexturePackId() {
    return m_requiredTexturePackId;
}
std::wstring JustGrSource::getDefaultSaveName() { return m_defaultSaveName; }
LPCWSTR JustGrSource::getWorldName() { return m_worldName.c_str(); }
LPCWSTR JustGrSource::getDisplayName() { return m_displayName.c_str(); }
std::wstring JustGrSource::getGrfPath() { return m_grfPath; }
bool JustGrSource::requiresBaseSave() { return m_bRequiresBaseSave; };
std::wstring JustGrSource::getBaseSavePath() { return m_baseSavePath; };

void JustGrSource::setRequiresTexturePack(bool x) {
    m_bRequiresTexturePack = x;
}
void JustGrSource::setRequiredTexturePackId(UINT x) {
    m_requiredTexturePackId = x;
}
void JustGrSource::setDefaultSaveName(const std::wstring& x) {
    m_defaultSaveName = x;
}
void JustGrSource::setWorldName(const std::wstring& x) { m_worldName = x; }
void JustGrSource::setDisplayName(const std::wstring& x) { m_displayName = x; }
void JustGrSource::setGrfPath(const std::wstring& x) { m_grfPath = x; }
void JustGrSource::setBaseSavePath(const std::wstring& x) {
    m_baseSavePath = x;
    m_bRequiresBaseSave = true;
}

bool JustGrSource::ready() { return true; }

LevelGenerationOptions::LevelGenerationOptions(DLCPack* parentPack) {
    m_spawnPos = NULL;
    m_stringTable = NULL;

    m_hasLoadedData = false;

    m_seed = 0;
    m_bHasBeenInCreative = true;
    m_useFlatWorld = false;
    m_bHaveMinY = false;
    m_minY = INT_MAX;
    m_bRequiresGameRules = false;

    m_pbBaseSaveData = NULL;
    m_dwBaseSaveSize = 0;

    m_parentDLCPack = parentPack;
    m_bLoadingData = false;
}

LevelGenerationOptions::~LevelGenerationOptions() {
    clearSchematics();
    if (m_spawnPos != NULL) delete m_spawnPos;
    for (AUTO_VAR(it, m_schematicRules.begin()); it != m_schematicRules.end();
         ++it) {
        delete *it;
    }
    for (AUTO_VAR(it, m_structureRules.begin()); it != m_structureRules.end();
         ++it) {
        delete *it;
    }

    for (AUTO_VAR(it, m_biomeOverrides.begin()); it != m_biomeOverrides.end();
         ++it) {
        delete *it;
    }

    for (AUTO_VAR(it, m_features.begin()); it != m_features.end(); ++it) {
        delete *it;
    }

    if (m_stringTable)
        if (!isTutorial()) delete m_stringTable;

    if (isFromSave()) delete m_pSrc;
}

ConsoleGameRules::EGameRuleType LevelGenerationOptions::getActionType() {
    return ConsoleGameRules::eGameRuleType_LevelGenerationOptions;
}

void LevelGenerationOptions::writeAttributes(DataOutputStream* dos,
                                             UINT numAttrs) {
    GameRuleDefinition::writeAttributes(dos, numAttrs + 5);

    ConsoleGameRules::write(dos, ConsoleGameRules::eGameRuleAttr_spawnX);
    dos->writeUTF(_toString(m_spawnPos->x));
    ConsoleGameRules::write(dos, ConsoleGameRules::eGameRuleAttr_spawnY);
    dos->writeUTF(_toString(m_spawnPos->y));
    ConsoleGameRules::write(dos, ConsoleGameRules::eGameRuleAttr_spawnZ);
    dos->writeUTF(_toString(m_spawnPos->z));

    ConsoleGameRules::write(dos, ConsoleGameRules::eGameRuleAttr_seed);
    dos->writeUTF(_toString(m_seed));
    ConsoleGameRules::write(dos, ConsoleGameRules::eGameRuleAttr_flatworld);
    dos->writeUTF(_toString(m_useFlatWorld));
}

void LevelGenerationOptions::getChildren(
    std::vector<GameRuleDefinition*>* children) {
    GameRuleDefinition::getChildren(children);

    std::vector<ApplySchematicRuleDefinition*> used_schematics;
    for (AUTO_VAR(it, m_schematicRules.begin()); it != m_schematicRules.end();
         it++)
        if (!(*it)->isComplete()) used_schematics.push_back(*it);

    for (AUTO_VAR(it, m_structureRules.begin()); it != m_structureRules.end();
         it++)
        children->push_back(*it);
    for (AUTO_VAR(it, used_schematics.begin()); it != used_schematics.end();
         it++)
        children->push_back(*it);
    for (AUTO_VAR(it, m_biomeOverrides.begin()); it != m_biomeOverrides.end();
         ++it)
        children->push_back(*it);
    for (AUTO_VAR(it, m_features.begin()); it != m_features.end(); ++it)
        children->push_back(*it);
}

GameRuleDefinition* LevelGenerationOptions::addChild(
    ConsoleGameRules::EGameRuleType ruleType) {
    GameRuleDefinition* rule = NULL;
    if (ruleType == ConsoleGameRules::eGameRuleType_ApplySchematic) {
        rule = new ApplySchematicRuleDefinition(this);
        m_schematicRules.push_back((ApplySchematicRuleDefinition*)rule);
    } else if (ruleType == ConsoleGameRules::eGameRuleType_GenerateStructure) {
        rule = new ConsoleGenerateStructure();
        m_structureRules.push_back((ConsoleGenerateStructure*)rule);
    } else if (ruleType == ConsoleGameRules::eGameRuleType_BiomeOverride) {
        rule = new BiomeOverride();
        m_biomeOverrides.push_back((BiomeOverride*)rule);
    } else if (ruleType == ConsoleGameRules::eGameRuleType_StartFeature) {
        rule = new StartFeature();
        m_features.push_back((StartFeature*)rule);
    }
#ifndef _CONTENT_PACKAGE
        "LevelGenerationOptions: Attempted to add invalid child rule - %d\n",
        ru #endif);
}
return rule;
}

void LevelGenerationOptions::addAttribute(const std::wstring& attributeName,
                                          const std::wstring& attributeValue) {
    if (attributeName "seed" re(L) == 0) {
        m_seed = _fromString<__int64>(attributeValue);
                app.De"LevelGenerationOptions: Adding parameter m_seed=%I64d\n",m_seed);
    } else if (attributeName "spawnX"(L) == 0) {
        if (m_spawnPos == NULL) m_spawnPos = new Pos();
        int value = _fromString<int>(attributeValue);
        m_spawnPos->x = value;
                app.De"LevelGenerationOptions: Adding parameter spawnX=%d\n",value);
    } else if (attributeName "spawnY"(L) == 0) {
        if (m_spawnPos == NULL) m_spawnPos = new Pos();
        int value = _fromString<int>(attributeValue);
        m_spawnPos->y = value;
                app.De"LevelGenerationOptions: Adding parameter spawnY=%d\n",value);
    } else if (attributeName "spawnZ"(L) == 0) {
        if (m_spawnPos == NULL) m_spawnPos = new Pos();
        int value = _fromString<int>(attributeValue);
        m_spawnPos->z = value;
                app.De"LevelGenerationOptions: Adding parameter spawnZ=%d\n",value);
    } else if (attributeName "flatworld") == 0)
	{
            if (attributeValue "true" re(L) == 0) m_useFlatWorld = true;
                app.De"LevelGenerationOptions: Adding parameter flatworld=%s\n",m_use"TRUE"r"FALSE":);
        }
    else if (attributeName "saveName") == 0)
	{
            std::wstring std::string(attributeValue);
            if (!std::string.empty())
                setDefaultSaveName(std::string);
            else
                setDefaultSaveName(attrib
                                   "LevelGenerationOptions: Adding parameter "
                                   "saveName=%ls\n",
                                   getDefaultSaveName().c_str());
        }
        e"worldName"ibuteName.compare(L) == 0)
	{
            std::wstring std::string(attributeValue);
            if (!std::string.empty())
                setWorldName(std::string);
            else
                "LevelGenerationOptions: Adding parameter "
                "worldName=%"
                "ls\n",
                    ge "displayName";
        }
        else if (attributeName.compare(L) == 0) {
            std::wstring std::string(attributeValue);
            if (!std::string.empty()) setDisplayName( std:"LevelGenerationOptions: Adding parameter displayName=%ls\n"bugPrintf("texturePackId" getDisplayName());
        }
        else if (attributeName.compare(L) == 0) {
                setRequiredTexturePackId( _fromString<unsigned int>(a"LevelGenerationOptions: Adding parameter texturePackId=%0x\n"gPrintf("isTutorial"uiredTexturePackId());
        }
        else if (attri "true" me.compare(L) == 0) {
                if(attribute"LevelGenerationOptions: Adding parameter isTutorial=%s\n"DebugPrintf("TRUE""FALSE""baseSaveName"l()?:);
        }
        else if(attributeName.compare(L"LevelGenerationOptions: Adding parameter baseSaveName=%ls\n"ugPrintf("hasBeenInCreative"ath().c_str());
}
else if (attributeName.compare(L) == 0) {
    bool value =
        _fromString <
        boo
        "LevelGenerationOptions: Adding parameter gameMode=%d\n" p.DebugPrintf(
            , m_bHasBeenInCreative);
}
else {
    GameRuleDefinition::addAttribute(attributeName, attributeValue);
}
}

void LevelGeneration"Processing schematics for chunk (%d,%d)"unk)
{
    PIXBeginNamedEvent(0, , chunk->x, chunk->z);
    AABB* chunkBox =
        AABB::newTemp(chunk->x * 16, 0, chunk->z * 16, chunk->x * 16 + 16,
                      Level::maxBuildHeight, chunk->z * 16 + 16);
    for (AUTO_VAR(it, m_schematicRules.begin()); it != m_schematicRules.end();
         ++it) {
        ApplySchematicRuleDefinition* rule = *it;
        rule->processSchematic(chunkBox, chunk);
    }

    int cx = (chunk->x << 4);
    int cz = (chunk->z << 4);

    for (AUTO_VAR(it, m_structureRules.begin()); it != m_structureRules.end();
         it++) {
        ConsoleGenerateStructure* structureStart = *it;

        if (structureStart->getBoundingBox()->intersects(cx, cz, cx + 15,
                                                         cz + 15)) {
            BoundingBox* bb = new BoundingBox(cx, cz, cx + 15, cz + 15);
            structureStart->postProcess(chunk->level, NULL, bb);
            delete bb;
        }
    }
    PIXEndNamedEvent();
}

void LevelGenerationOptions
    : "Processing schematics (lighting) for chunk (%d,%d)" BeginNamedEvent(
          0, , chunk->x, chunk->z);
AABB* chunkBox =
    AABB::newTemp(chunk->x * 16, 0, chunk->z * 16, chunk->x * 16 + 16,
                  Level::maxBuildHeight, chunk->z * 16 + 16);
for (AUTO_VAR(it, m_schematicRules.begin()); it != m_schematicRules.end();
     ++it) {
    ApplySchematicRuleDefinition* rule = *it;
    rule->processSchematicLighting(chunkBox, chunk);
}
PIXEndNamedEvent();
}

bool LevelGenerationOptions::checkIntersects(in"Check Intersects"z0, i// As an optimisation, we can quickly discard things below a certain y which makes most ore checks faster due to// a) ores generally being below ground/sea level and b) tutorial world additions generally being above ground/sea level
	if(!m_bHaveMinY)
	{
    for (AUTO_VAR(it, m_schematicRules.begin()); it != m_schematicRules.end();
         ++it) {
        ApplySchematicRuleDefinition* rule = *it;
        int minY = rule->getMinY();
        if (minY < m_minY) m_minY = minY;
    }

    for (AUTO_VAR(it, m_structureRules.begin()); it != m_structureRules.end();
         it++) {
        ConsoleGenerateStructure* structureStart = *it;
        int minY = structureStart->getMinY()  // 4J Stu - We DO NOT intersect if
                                              // our upper bound is below the
                                              // lower bound for all schematics
                   if (y1 < m_minY) return false;

        bool intersects = false;
        for (AUTO_VAR(it, m_schematicRules.begin());
             it != m_schematicRules.end(); ++it) {
            ApplySchematicRuleDefinition* rule = *it;
            intersects = rule->checkIntersects(x0, y0, z0, x1, y1, z1);
            if (intersects) break;
        }

        if (!intersects) {
            for (AUTO_VAR(it, m_structureRules.begin());
                 it != m_structureRules.end(); it++) {
                ConsoleGenerateStructure* structureStart = *it;
                intersects =
                    structureStart->checkIntersects(x0, y0, z0, x1, y1, z1);
                if (intersects) break;
            }
        }
        PIXEndNamedEvent();
        return intersects;
    }

    void LevelGenerationOptions::clearSchematics() {
        for (AUTO_VAR(it, m_schematics.begin()); it != m_schematics.end();
             ++it) {
            delete it->second;
        }
        m_schematics.clear();
    }

    ConsoleSchematicFile*
        LevelGenerationOptions::loadSche  // If we have already loaded this,
                                          // just returnpbData, DWORD dwLen)
    {
        AUTO_V#ifndef _CONTENT_PACKAGE(filename));"We have already loaded schematic file %ls\n"
		wprint#endif, filename.c_str() );

        it->second->incrementRefCount();
        return it->second;
    }

    ConsoleSchematicFile* schematic = NULL;
    byteArray data(pbData, dwLen);
    ByteArrayInputStream bais(data);
    DataInputStream dis(&bais);
    schematic = new ConsoleSchematicFile();
    schematic->load(&dis);
    m_schematics[filename] = schematic;
    bais.reset();
    return schematic;
}

ConsoleSchematicFile *LevelGenerationOptions::getSchematicFile(cons// If we have already loaded this, just returnile *schematic = NULL;
	
	AUTO_VAR(it, m_schematics.find(filename));
	if(it != m_schematics.end())
	{
    schematic = it->second;
	}
	return schematic;
}

void LevelGen  // 4J Stu - We don't want to delete them when done, but probably
               // want to keep a set of active schematics for the current
               // world//AUTO_VAR(it, m_schematics.find(filename));//if(it !=
               // m_schematics.end())//{//	ConsoleSchematicFile *schematic
               // = it->second;//	schematic->decrementRefCount();//
               // if(schematic->shouldDelete())
               //	{//		delete schematic;//
               // m_schematics.erase(it);//	}//}
}

void LevelGenerationOptions::loadStringTable(StringTable* table) {
    m_stringTable = table;
}

LPCWSTR LevelGenerationOptions::getStri ""(const std::wstring& key) {
    if (m_stringTable == NULL) {
        return L;
    } else {
        return m_stringTable->getString(key);
    }
}

void LevelGenerationOptions::getBiomeOverride(int biomeId, BYTE& tile,
                                              BYTE& topTile) {
    for (AUTO_VAR(it, m_biomeOverrides.begin()); it != m_biomeOverrides.end();
         ++it) {
        BiomeOverride* bo = *it;
        if (bo->isBiome(biomeId)) {
            bo->getTileValues(tile, topTile);
            break;
        }
    }
}

bool LevelGenerationOptions::isFeatureChunk(
    int chunkX, int chunkZ, StructureFeature::EFeatureTypes feature,
    int* orientation) {
    bool isFeature = false;

    for (AUTO_VAR(it, m_features.begin()); it != m_features.end(); ++it) {
        StartFeature* sf = *it;
        if (sf->isFeatureChunk(chunkX, chunkZ, feature, orientation)) {
            isFeature = true;
            break;
        }
    }
    return isFeature;
}

std::unordered_map < std::wstring,
    Co  // Clean schematic
        // rules.velGenerationOptions::getUnfinishedSchematicFiles()
{
    std::unordered_set<std::wstring> usedFiles =
        std::unordered_set<std::wstring>();
        for (AUTO_VAR(it, m_schematicRules.begin()); it!=m_schematicR// Clean schematic files.!(*it)->isComplete() )
			usedFiles.insert( (*it)->getSchematicName() );

	
	std::unordered_map<std::wstring, ConsoleSchematicFile *> *out 
		= new std::unordered_map<std::wstring, ConsoleSchematicFile *>();
	for (AUTO_VAR(it, usedFiles.begin()); it!=usedFiles.end(); it++)
		out->insert( std::pair<std::wstring, ConsoleSchematicFile *>(*it, getSchematicFile(*it)) );

	return out;
}

void LevelGenerationOptions::loadBaseSaveData(#ifdef _DURANGOndex = -1;
	if(m_parentDLCPack != NULL) mountIndex = m_parentDLCPack->GetDLCMountIndex();

	if(mountIndex > -1)
	{
"WPACK"		if(StorageManager.M#elsenstalledDLC(ProfileManager.GetPrimaryPad(),mountIndex,&LevelGenerationOptions::packMounted,this,L)!=ERROR_IO_PENDING"WPACK"
		if(StorageManager.#endifnstalled// corrupt DLCager.GetPrimaryPad(),mountIndex,&LevelGe"Failed to mount LGO DLC %d for pad %d\n")!=ERROR_IO_PENDING)

		{
        setLoadedData();
        app.DebugPrintf("Attempted to mount DLC data for LGO %d\n" eManager
                            .GetPrimaryPad());	
		}
		else
		{
        m_bLoadingData = true;
        app.DebugPrintf(, mountIndex);
		}
	}
	else
	{
    setLoadedData();
    app.SetAction(ProfileManager.GetPrimaryPad(), eAppAction_ReloadTexturePack);
	}
}

int LevelGenerationOptions::packMounted(LPVOID pParam,int iPad,DWORD dwErr,DWORD dwLicenceMask// corrupt DLCrationOptions *lgo "Failed to mount LGO DLC for pad %d: %d\n"m_bLoadingData = false;
	if(dwErr!=ERROR_SUC"Mounted DLC for LGO, attempting to load data\n",iPad,dwErr);
}
else {
    app.DebugPrintf();
    DWORD dwFilesProcessed = 0;
    int gameRulesCount = lgo->m_parentDLCPack->getDLCItemsCount(
        DLCManager::e_DLCType_GameRulesHeader);
    for (int i = 0; i < gameRulesCount; ++i) {
        DLCGameRulesHeader* dlcFile =
            (DLCGameRulesHeader*)lgo->m_parentDLCPack->getFile(
                DLCManager::e_DLCType_GameRulesHeader, i);

        if (!"WPACK:" > getGrfPath().empty()) {
                                Fi#ifdef _UNICODEFilePath(lgo->m_parentDLCPack->GetPackID(), dlcFile->getGrfPath(),true, L ) );
                                if (grf.exists()) {
                                    // file name path = grf.getPath()// access
                                    // modeCHAR *pchF// share mode // TODO 4J
                                    // Stu - Will we need to share file?
                                    // Probably not but...
                                    // UnusedERIC_READ, // how to create // TODO
                                    // 4J Stu - Assuming that the file already
                                    // exists if we are opening to read from it
                                                OPEN_EXISTING , // file attributes// Unsupported#else
						FILE_FLAG_SEQUENTIAL_SCAN, 
						NULL // file name
					const char *// access modetringtofil// share mode // TODO 4J Stu - Will we need to share file? Probably not but...
// UnusedERIC_READ, // how to create // TODO 4J Stu - Assuming that the file already exists if we are opening to read from it
						OPEN_EXISTING , // file attributes// Unsupported#endif
						FILE_FLAG_SEQUENTIAL_SCAN, 
						NULL 
						);

                                                if (fileHandle !=
                                                    INVALID_HANDLE_VALUE) {
                                                    DWORD dwFileSize =
                                                        grf.length();
                                                    DWORD bytesRead;
                                                    PBYTE pbData =
                                                        (PBYTE) new BYTE
                                                            [dwFileSize];
                                                BOOL bSuccess = ReadFile(fileHandle,pbData,dwF// 4J-PB - is it possible that we can get here after a read fail and it's not an error?;
                                                }
                                                CloseHandle(fileHandle);

                                                dlcFile->setGrfData(
                                                    pbData, dwFileSize,
                                                    lgo->m_stringTable);

                                                delete[] pbData;

                                                app.m_gameRules
                                                    .setLevelGenerationOptions(
                                                        dlcFile->lgo);
                                }
        }
    }
}
if (lgo->requiresBaseSave() "WPACK:" > getBaseSavePath().empty()) {
#ifdef _UNICODEpp.getFilePath(lgo->m_parentDLCPack->GetPackID(),
                                  lgo->getBaseSavePath(), true, L ));
                                  if (save.exists()) {
                                      // file namewstring path = save.// access
                                      // mode	const WC// share mode // TODO 4J
                                      // Stu - Will we need to share file?
                                      // Probably not but...ename, // Unused
                                      // GENERIC_READ, // how to create // TODO
                                      // 4J Stu - Assuming that the file already
                                      // exists if we are opening to read from
                                      // itLL,
                                        OPEN_EXISTING // file attributes// Unsupported#else
					FILE_FLAG_SEQUENTIAL_SCAN, 
					NULL // file name		);

				const// access modename=wstr// share mode // TODO 4J Stu - Will we need to share file? Probably not but...ename, // Unused				GENERIC_READ, // how to create // TODO 4J Stu - Assuming that the file already exists if we are opening to read from itLL, 
					OPEN_EXISTING // file attributes// Unsupported#endif
					FILE_FLAG_SEQUENTIAL_SCAN, 
					NULL 
					);

                                        if (fileHandle !=
                                            INVALID_HANDLE_VALUE) {
                                            DWORD bytesRead,
                                                dwFileSize = GetFileSize(
                                                    fileHandle, NULL);
                                            PBYTE pbData =
                                                (PBYTE) new BYTE[dwFileSize];
                                        BOOL bSuccess = ReadFile(fileHandle// 4J-PB - is it possible that we can get here after a read fail and it's not an error?dError();
                                        }
                                        CloseHandle(fileHandle);

#ifdef _DURANGO "WPACK" #elsesetBaseSaveData(pbData, dwFileSize);
                                  }
}
}
"WPACK" #endif DWORD result = StorageManager.UnmountInstalledDLC(L);

DWORD result = StorageManager.UnmountInstalledDLC();
}

lgo->setLoadedData();

return 0;
}

void LevelGenerationOptions::reset_start() {
        for (	AUTO_VAR( it, m_//if (m_spawnPos)				{ delete m_spawnPos; m_spawnPos = NULL; }+	//if (m_stringTable)			{ delete m_stringTable; m_stringTable = NULL; }()
{
        if (isFromDLC()) {
            m_hasLoadedData = false;
        }
}


GrSource *LevelGenerationOptions::info() {
        return m_pSrc; }
void LevelGenerationOptions::setSrc(eSrc src) {
        m_src = src; }
LevelGenerationOptions::eSrc LevelGenerationOptions::getSrc() {
        return m_src; }

bool LevelGenerationOptions::isTutorial() {
        return getSrc() == eSrc_tutorial; }
bool LevelGenerationOptions::isFromSave() {
        return getSrc() == eSrc_fromSave; }
bool LevelGenerationOptions::isFromDLC() {
        return getSrc() == eSrc_fromDLC; }

bool LevelGenerationOptions::requiresTexturePack() {
        return info()->requiresTexturePack(); }
UINT LevelGenerationOptions::getRequiredTexturePackId() {
        return info()->getRequiredTexturePackId(); }

std::wstring LevelGenerationOptions::getDefaultSaveName()
{
        switch (getSrc()) {
            case eSrc_fromSave:
                return getString(info()->getDefaultSaveName());
            case eSr "" fromDLC:
                return getString(info()->getDefaultSaveName());
            case eSrc_tutorial:
                return app.GetString(IDS_TUTORIALSAVENAME);
        }
        return L;
}
LPCWSTR LevelGenerationOptions::getWorldName()
{
        switch (getSrc()) {
            case eSrc_fromSave:
                return getString(info()->getWorldName());
                "" ase eSrc_fromDLC : return getString(info()->getWorldName());
            case eSrc_tutorial:
                return app.GetString(IDS_PLAY_TUTORIAL);
        }
        return L;
}
LPCWSTR LevelGenerationOptions::getDisplayName()
{
        switch (getSrc()) {
            case eSrc_fromSave:
                ret "" n getString(i "" o()->getDisplayName());
            case eSrc_fromDLC:
                return getString(info()->getDisplayName());
            case eSrc_tutorial:
                return L;
        }
        return L;
}

std::wstring LevelGenerationOptions::getGrfPath() {
        return info()->getGrfPath(); }
bool LevelGenerationOptions::requiresBaseSave() {
        return info()->requiresBaseSave(); }
std::wstring LevelGenerationOptions::getBaseSavePath() {
        return info()->getBaseSavePath(); }

void LevelGenerationOptions::setGrSource(GrSource *grs) {
        m_pSrc = grs; }

void LevelGenerationOptions::setRequiresTexturePack(bool x) {
        info()->setRequiresTexturePack(x); }
void LevelGenerationOptions::setRequiredTexturePackId(UINT x) {
        info()->setRequiredTexturePackId(x); }
void LevelGenerationOptions::setDefaultSaveName(const std::wstring &x) {
        info()->setDefaultSaveName(x); }
void LevelGenerationOptions::setWorldName(const std::wstring &x) {
        info()->setWorldName(x); }
void LevelGenerationOptions::setDisplayName(const std::wstring &x) {
        info()->setDisplayName(x); }
void LevelGenerationOptions::setGrfPath(const std::wstring &x) {
        info()->setGrfPath(x); }
void LevelGenerationOptions::setBaseSavePath(const std::wstring &x) {
        info()->setBaseSavePath(x); }

bool LevelGenerationOptions::ready() {
        return info()->ready(); }

void LevelGenerationOptions::setBaseSaveData(PBYTE pbData, DWORD dwSize) {
        m_pbBaseSaveData = pbData;
        m_dwBaseSaveSize = dwSize; }
PBYTE LevelGenerationOptions::getBaseSaveData(DWORD &size) {
        size = m_dwBaseSaveSize;
        return m_pbBaseSaveData; }
bool LevelGenerationOptions::hasBaseSaveData() {
        return m_dwBaseSaveSize > 0 && m_pbBaseSaveData != NULL; }
void LevelGenerationOptions::deleteBaseSaveData() {
        if (m_pbBaseSaveData) delete m_pbBaseSaveData;
        m_pbBaseSaveData = NULL;
        m_dwBaseSaveSize = 0; }

bool LevelGenerationOptions::hasLoadedData() {
        return m_hasLoadedData; }
void LevelGenerationOptions::setLoadedData() {
        m_hasLoadedData = true; }

__int64 LevelGenerationOptions::getLevelSeed() {
        return m_seed; }
int LevelGenerationOptions::getLevelHasBeenInCreative() {
        return m_bHasBeenInCreative; }
Pos *LevelGenerationOptions::getSpawnPos() {
        return m_spawnPos; }
bool LevelGenerationOptions::getuseFlatWorld() {
        return m_useFlatWorld; }

bool LevelGenerationOptions::requiresGameRules() {
        return m_bRequiresGameRules; }
void LevelGenerationOptions::setRequiredGameRules(LevelRuleset *rules) {
        m_requiredGameRules = rules;
        m_bRequiresGameRules = true; }
LevelRuleset *LevelGenerationOptions::getRequiredGameRules() {
        return m_requiredGameRules; }