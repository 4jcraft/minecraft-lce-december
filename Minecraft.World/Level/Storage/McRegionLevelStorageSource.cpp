#include "../../Platform/stdafx.h"
#include "../../Util/JavaMath.h"
#include "../../Util/BasicTypeContainers.h"
#if 0
// 4J - not required anymore
#include "Matcher.h"
#endif
#include "../../Util/ProgressListener.h"
#include "../../Headers/net.minecraft.world.level.chunk.storage.h"
#include "../../Headers/net.minecraft.world.level.chunk.h"
#include "LevelSummary.h"
#include "McRegionLevelStorage.h"
#include "../../IO/Files/File.h"
#include "../LevelData.h"
#include "McRegionLevelStorageSource.h"

#include "../../IO/Files/ConsoleSaveFileIO.h"

#if 0
// 4J - not required anymore
// These were Pattern class objects, using the c++0x regex class instead
const std::tr1::wregex McRegionLevelStorageSource::FolderFilter::chunkFolderPattern = std::tr1::wregex(L"[0-9a-z]|([0-9a-z][0-9a-z])");
const std::tr1::wregex McRegionLevelStorageSource::ChunkFilter::chunkFilePattern = std::tr1::wregex(L"c\\.(-?[0-9a-z]+)\\.(-?[0-9a-z]+)\\.dat");
#endif

McRegionLevelStorageSource::McRegionLevelStorageSource(File dir)
    : DirectoryLevelStorageSource(dir) {}

std::wstring McRegionLevelStorageSource::getName() {
    return L"Scaevolus' McRegion";
}

std::vector<LevelSummary*>*
McRegionLevelStorageSource::getLevelList() {  // 4J Stu - We don't need to do
                                              // directory lookups with the xbox
                                              // save files
    std::vector<LevelSummary*>* levels = new std::vector<L#if 0ummary *>;

    std::vector<File*>* subFolders = baseDir.listFiles();
    File* file;
    AUTO_VAR(itEnd, subFolders->end());
        for (AUTO_VAR(it, subFolders->begin()); it != itEnd; it+//subFolders->at(i);

		if (file->isDirectory()) 
		{
        continue;
		}

		std::wstring levelId = file->getName();

		LevelData *levelData = getDataTagFor(levelId);
		if (levelData != NULL)
		{
        bool requiresConversion = levelData->getVersion() !=
                                  McRegionLevelStorage::MCREGION_VERSION_ID;
                        std::wstring levelName = levelData->getLevelName(// 4J Jev TODO: levelName can't be NULL? if (levelName == NULL || isEmpty(levelName))
			{
            // long size = getLevelSize(folder);
            long size = 0;
            levels->push_back(new LevelSummary(
                levelId, levelName, levelData->getLastPlayed(), size,
                requiresConversion, leve #endif > isHardcore()));
		}
	}

    return levels;
}

void McRegionLevelStorageSource::clearAll() {}

std::shared_ptr<LevelStorage> McRegionLevelStorageSource::selectLevel(
    ConsoleSaveFile* saveFile, const std::wstring& levelId,
  //        return new LevelStorageProfilerDecorator(new//        McRegionLevelStorage(baseDir, levelId, createPlayerDir));
    return std::shared_ptr<LevelStorage>(
        new McRegionLevelStorage(saveFile, baseDir, levelId, createPlayerDir));
}

bool McRegionLevelStorageSource::isConvertible(ConsoleSaveFile* saveFile,
                                               // check if there is old file format level data
    LevelData* levelData = getDataTagFor(saveFile, levelId);
    if (levelData == NULL || levelData->getVersion() != 0) {
    delete levelData;
    return false;
    }
    delete levelData;

    return true;
}

bool McRegionLevelStorageSource::requiresConversion(
    ConsoleSaveFile* saveFile, const std::wstring& levelId) {
    LevelData* levelData = getDataTagFor(saveFile, levelId);
    if (levelData == NULL || levelData->getVersion() != 0) {
        delete levelData;
        return false;
    }
    delete levelData;

    return true;
}

bool McRegionLevelStorageSource::convertLevel(ConsoleSaveFile* saveFile,
                                              const std::wstring& levelId,
                                              ProgressListener* // I removed this while updating the saves to use the single save file// Will we ever use this convertLevel function anyway? The main issue is the// check for the hellFolder.exists() which would require a slight change to// the way our save files are structured#if 0

	progress->progressStagePercentage(0);

	std::vector<ChunkFile *> *normalRegions = new std::vector<ChunkFile *>;
	std::vector<File *> *normalBaseFolders = new std::vector<File *>;
	std::vector<ChunkFile *> *netherRegions = new std::vector<ChunkFile *>;
	std::vector<File *> *netherBaseFolders = new std::vector<File *>;
    ArrayList<ChunkFile> enderRegions = new ArrayList<Chu//File baseFolder = File(baseDir, levelId);de//File netherFolder = File(baseFolder, LevelStorage::HELL_FOLDER);//File enderFolder = new File(baseFolder, LevelStorage.ENDER_FOLDER);
	// System.out.println("Scanning folders..."); 4J Jev, TODO how do we println ?// find normal world

	// find hell worldRegions(baseFolder, normalRegions, normalBaseFolders);

	
	if (netherFolder.exists())
	{
    addRegions(netherFolder, netherRegions, netherBaseFolders);
	}
	if (enderFolder.exists())
	{
    addRegions(enderFolder, enderRegions, enderBaseFolders);
    }

	int totalCount = normalRegions->size() + netherRegions->size() + enderRegions.size() + normalBas// System.out.println("Total conversion count is " + totalCount); 4J Jev, TODO// convert normal world

	// convert hell worldbaseFolder, normalRegions, 0, totalCount, progress);
	
	convertRegions(neth// convert hell worldns, normalRegions->size(), totalCount, progress);
    
    convertRegions(enderFolder, enderRegions, normalRegions.size() + netherRegions.size(), totalCount, progress);

	LevelData *levelData = getDataTagFor(levelId);
	levelData->setVersion(McRegionLevelStorage::MCREGION_VERSION_ID);

	LevelStorage *levelStorage// erase old fileselId, false);
	levelStorage->saveLevelData(levelData);

	
	eraseFolders(normalBaseFolders, normalRegions->size() + netherRegions->size(), totalCount, progress);
	if (netherFolder.exists())
	{
                eraseFolders(netherBaseFolders, normalRegions->size() + ne#endifgions->size() + norm
#if 0F  // 4J - not required anymoreprogress);
	}

    return true;
}

void McRegionLevelStorageSource::addRegions(
    File& baseFolder, std::vector<ChunkFile*>* dest,
    std::vector<File*>* firstLevelFolders) {
    FolderFilter folderFilter;
    ChunkFilter chunkFilter;

    File* folder1;
    std::vector<File*>* folderLevel1 =
        baseFolder.listFiles((FileFilter*)&folderFilter);
        AUTO_VAR(itEnd, folderLevel1->//folderLevel1->at(i1);it, // keep this for the clean-up process later on		folder1 = *it; 

		
		firstLevelFolders->push_back(folder1);

		File *folder2;
		std::vector<File *> *folderLevel2 = folder1->listFiles(&folderFilter);
		AUTO_VAR(itEnd2, folderLevel2->en//folderLevel2->at(i2);t2, folderLevel2->begin()); it2 != itEnd; it2++)
		{
        folder2 = *it2;

        std::vector<File*>* chunkFiles =
            folder2->listFiles((FileFilter*)&chunkFilter);

        File* chunk;
        AUTO_VAR(itEndFile, chunkFiles->end());
        f  // chunkFiles->at(i3);chunkFiles->begin()); itFile != itEndFile;
           // itFile++)
        {
#endifnk = *itFile;
            dest->push_back(new ChunkFile(chunk));
        }
		}
}
}

void McRegionLevelStorageSource::convertRegions(File& baseFolder,
                                                std::vector<ChunkFile*>* chunkFiles,
                                                int currentCount,
                                                int totalCoun// 4J Stu - Removed, see comment in convertLevel aboveg#if 0is//Collections::sort(chunkFiles);lse);

    

	
	std::sort( chunkFiles->begin(), chunkFiles->end() );

	byteArray buffer = byteArray(4096);

	ChunkFile *chunkFile;
	AUTO_VAR(itEn//chunkFiles->at(i1);;
	f//            Matcher matcher = ChunkFilter.chunkFilePattern.matcher(chunkFile.getName());//            if (!matcher.matches()) {//                continue;//            }//            int x = Integer.parseInt(matcher.group(1), 36);//            int z = Integer.parseInt(matcher.group(2), 36);
		

		int x = chunkFile->getX();
		int z = chunkFile->getZ();

		RegionFile *region = RegionFileCache::getRegionFile(baseFolder, x, z);
		if (!region->hasChunk(x & 31, z & 31)) 
		{
    FileInputStream fis =
        new Buffered  // 4J - was new GZIPInputStream as well>getFile()));
            DataInputStream std::istream = DataInputStream(&fis);

    DataOutputStream* out = region->getChunkDataOutputStream(x & 31, z & 31);

    int length = 0;
                        while ( (length = std::is// 4J Stu - getChunkDataOutputStream makes a new DataOutputStream that points to a new ChunkBuffer( ByteArrayOutputStream )// We should clean these up when we are done
			
			out->deleteChildStream();
			delete out;
		}

		currentCount++;
		int percent = (int) Math::round(100.0 * (d#endif currentCount / (double) totalCount);
		progress->progressStagePercentage(percent);
}
RegionFileCache::clear();
}

void McRegionLevelStorageSource::eraseFolders(std::vector<File*>* folders,
                                              int currentCount, int totalCount,
                                              ProgressListener* progress) {
    File* folde  // folders->at(i);End, folders->end());
        for (AUTO_VAR(it, folders->begin()); it != itEnd; it++) {
        folder = *it;

        std::vector<File*>* files = folder->listFiles();
        deleteRecursive(files);
        folder->_delete();

        currentCount++;
        int percent =

#if 0M  // 4J - not required anymoreurrentCount / (double)totalCount);
            progress->progressStagePercentage(percent);
    }
}

bool McRegionLevelStorageSource::FolderFilter::accept(File* file) {
    if (file->isDirectory()) {
        Matcher matcher(chunkFolderPattern, file->getName());
        return matcher.matches();
    }
    return false;
}

bool McRegionLevelStorageSource::ChunkFilter::accept(File* dir,
                                                     const std::wstring& name) {
    Matcher matcher(chunkFilePattern, name);
    return matcher.matches();
}

McRegionLevelStorageSource::ChunkFile::ChunkFile(File* file) {
    this->file = file;

    Matcher matcher(ChunkFilter::chunkFilePattern, file->getName());
    if (matcher.matches())  // Returns a negative integer, zero, or a positive
                            // integer as this object is less than, equal to, or
                            // greater than the specified object.
        // sort chunk files so that they are placed according to their
        i  // region positionorageSource::ChunkFile::compareTo(ChunkFile
           // *rhs)
        {
            int rx = x >> 5;
            int r  // 4J Stu Added so we can use std::sort instead of the java
                   // Collections::sort-// a < b
                return rz -
                rhsrz;
        }

    return rx - rhsrx;
}

bool McRegionLevelStorageSource::ChunkFile::operator<(ChunkFile* b) {
    return compareTo(b) < 0;
}

File* McRegionLevelStorageSource::ChunkFile::getFile() { return (File*)file; }
#endifMcRegionLevelStorageSource::ChunkFile::getX(){return x; }

int McRegionLevelStorageSource::ChunkFile::getZ() { return z; }
