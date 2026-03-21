#include "../../Platform/stdafx.h"
#include "FileHeader.h"

// #define _DEBUG_FILE_HEADER

FileHeader::FileHeader() {
    lastFile = NULL;
    m_saveVersion = 0;

    // New saves should have an original version set to the latest version. This
    // will be overridden when we load a save
    m_originalSaveVersion = SAVE_FILE_VERSION_NUMBER;
    m_savePlatform = SAVE_FILE_PLATFORM_LOCAL;
    m_saveEndian = m_localEndian;
}

FileHeader::~FileHeader() {
    for (unsigned int i = 0; i < fileTable.size(); ++i) {
        delete fileTable[i];
    }
}

FileEntry* FileHeader::AddFile(const std::wstring& name,
                               unsigned int length /* = 0 */) {
    assert(name.length() < 64);

    wchar_t filename[64];
    memset(&filename, 0, sizeof(wchar_t) * 64);
    memcpy(&filename, name.c_str(),
           std::min(sizeof(wchar_t) * 64, sizeof(wchar_t) * name.length()));
    // Would a map be more efficient? Our file tables probably won't be very
    // big// so better to avoid hashing all the time? Does the file
    // exist?
    for (unsigned int i = 0; i < fileTable.size(); ++i) {
        if (wcscmp(fileTable[i]->data.filename, filename) == 0) {
            // If so, return it
            return fileTable[i];
        }
    }
    // Else, add it to our file table
    fileTable.push_back(new FileEntry(filename, length, GetStartOfNextData()));
    lastFile = fileTable[fileTable.size() - 1];
    return lastFile;
}

void FileHeader::RemoveFile(FileEntry* file) {
    if (file == NULL) return;

    AdjustStartOffsets(file, file->getFileSize(), true);

    AUTO_VAR(it, std::find(fileTable.begin(), fileTable.end(), file));

    if (it < fileTable.end()) {
        fileTable.erase(i
#ifndef _CONTENT_PACKAGE
                        "Removed file %ls\n",
                        file->data.f #endife);

        delete file;
    }

    void FileHeader::WriteHeader(LPVOID saveMem) {
    unsigned int headerOffset = GetStartOfNextDat// 4J Changed for save version 2 to be the number of files rather than the// size in bytes
    unsigned int headerSize = (int)(fileTable.size// DWORD numberOfBytesWritten = 0;// Write the offset of the header// assert(numberOfBytesWritten == 4);
    int* begin = (int*#ifdef __PSVITA__
    VirtualCopyTo(begin, &headerOffset, sizeof(header#elset));

    *begin = head#endifet;
// Write the size of the header// assert(numberOfBytesWritten == 4);#ifdef __PSVITA__
    VirtualCopyTo(begin + 1, &headerSize, sizeof(head#elsee));

    *(begin + 1) = he#endifze;


    short* versions = (short*)(begin // Write the original version number#ifdef __PSVITA__
    VirtualCopyTo(versions, &m_originalSaveVersion,
                  sizeof(m_originalSaveV#elsen));

    *versions = m_originalSav#endifon;
// Write the version number
    short versionNumber = SAVE_FILE_VERSION_NU// assert(numberOfBytesWritten == 4);//*(begin + 2) = versionNumber;#ifdef __PSVITA__
    VirtualCopyTo(versions + 1, &versionNumber, sizeof(version#elser));

    *(versions + 1) = versi#endife
#ifdef _DEBUG_FILE_HEADER
    app.DebugPrintf \
    "Write save file with original version: %d, and current version %d\n",
        m_originalSaveVersion, versio#endifr);


    char* headerPosition = (char*)saveMem + head
#ifdef _DEBUG_FILE_HEADER
    app.De "\n\nWrite file Header: Offset = %d, Size = %d\n",
                    headerOffset, hea#endife);
// Write the header
    for (unsigned int i = 0; i < fileTable.size(); ++i) // wprintf(L"File: %ls, Start = %d, Length = %d, End = %d\n",// fileTable[i]->data.filename, fileTable[i]->data.startOffset,// fileTable[i]->data.length, fileTable[i]->data.startOffset +// fileTable[i]->data.length);#ifdef __PSVITA__
        VirtualCopyTo((void*)headerPosition, &fileTable[i]->data,
                      sizeof(FileEntrySa#elsea));

        memcpy((void*)headerPosition, &fileTable[i]->data,
               sizeof(FileEntrySa#endif));
// assert(numberOfBytesWritten == sizeof(FileEntrySaveData));
        headerPosition += sizeof(FileEntrySaveData);
    }
}

void FileHeader::ReadHeader(LPVOID saveMem,
                            ESavePlat /*= SAVE_FILE_PLATFORM_LOCAL */) {
    unsigned int headerOffset;
    unsigned int headerSize;

    m_savePlatform = plat;

    switch (m_savePlatform) {
        case SAVE_FILE_PLATFORM_X360:
        case SAVE_FILE_PLATFORM_PS3:
            m_saveEndian = BIGENDIAN;
            break;
        case SAVE_FILE_PLATFORM_XBONE:
        case SAVE_FILE_PLATFORM_WIN64:
        case SAVE_FILE_PLATFORM_PS4:
        case SAVE_FILE_PLATFORM_PSVITA:
            m_saveEndian = LITTLEENDIAN;
            break;
        default:
            assert(0);
            m_savePlatform = SAVE_FILE_PLATFORM_LOCAL;
            m_saveEndian = m_localEndian;
            break;
            // Read the offset of the header// assert(numberOfBytesRead ==
            // 4);
    int* begin = (int*#ifdef __PSVITA__
    VirtualCopyFrom(&headerOffset, begin, sizeof(header#elset));

    headerOffset #endifin;

    if (isSaveEndianDifferent()) System::ReverseULONG(&headerOffs// Read the size of the header// assert(numberOfBytesRead == 4);#ifdef __PSVITA__
    VirtualCopyFrom(&headerSize, begin + 1, sizeof(head#elsee));

    headerSize = *(be#endif1);

    if (isSaveEndianDifferent()) System::ReverseULONG(&headerSize);

    short* versions = (short*)(begin // Read the original save version number#ifdef __PSVITA__
    VirtualCopyFrom(&m_originalSaveVersion, versions,
                    sizeof(m_originalSaveV#elsen));

    m_originalSaveVersion = *(v#endifs);

    if (isSaveEndianDifferent()) System::ReverseSHORT(&m_originalSaveVersi// Read the save version number// m_saveVersion = *(begin + 2);#ifdef __PSVITA__
    VirtualCopyFrom(&m_saveVersion, versions + 1, sizeof(m_saveV#elsen));

    m_saveVersion = *(versi#endif1);

    if (isSaveEndianDifferent()) System::ReverseSHORT(&m_save
#ifdef _DEBUG_FILE_HEADER
    app.DebugPrintf \
    "Read save file with orignal version: %d, and current version %d\n",
        m_originalSaveVersion, m_saveVersion);
    app.De"\n\nRead file Header: Offset = %d, Size = %d\n",
                    headerOffset, hea#endife);


    char* headerPosition = (char*)saveMem + headerOffset;

    switch (m_saveVersion) // case SAVE_FILE_VERSION_NUMBER:// case 8: // 4J Stu - SAVE_FILE_VERSION_NUMBER 2,3,4,5,6,7,8 are the// same, but: 							: Bumped it to 3 in TU5 to force older builds (ie 0062) to//generate a new world when trying to load new saves 							: Bumped it to 4 in//TU9 to delete versions of The End that were generated in builds prior//to TU9 							: Bumped it to 5 in TU9 to update the map data that was only//using 1 bit to determine dimension 							: Bumped it to 6 for PS3 v1 to//update map data mappings to use larger PlayerUID 							: Bumped it to 7 for//Durango v1 to update map data mappings to use string based PlayerUID//							: Bumped it to 8//for Durango v1 when to save the chunks in a different compressed//format  
        case SAVE_FILE_VERSION_CHUNK_INHABITED_TIME:
        case SAVE_FILE_VERSION_COMPRESSED_CHUNK_STORAGE:
        case SAVE_FILE_VERSION_DURANGO_CHANGE_MAP_DATA_MAPPING_SIZE:
        case SAVE_FILE_VERSION_CHANGE_MAP_DATA_MAPPING_SIZE:
        case SAVE_FILE_VERSION_MOVED_STRONGHOLD:
        case SAVE_FILE_VERSION_NEW_END:
        case SAVE_FILE_VERSION_POST_LAUNCH:
        case SAVE_FILE_VERSION_LAUNCH: {
                // Changes for save file version 2:
                // headerSize is now a count of elements rather than a count
                // of bytes The FileEntrySaveData struct has a
                // lastModifiedTime member

                // Read the header
                FileEntrySaveData* fesdHeaderPosition =
                    (FileEntrySaveData*)headerPosition;
                for (unsigned int i = 0; i < headerSize; ++i) {
                    FileEntry* entry = new FileEntry();
            // assert(numberOfBytesRead == sizeof(FileEntrySaveData));
#ifdef __PSVITA__
                    VirtualCopyFrom(&entry->data, fesdHeaderPosition,
                                    sizeof(FileEntrySa #elsea));

                    memcpy(&entry->data, fesdHeaderPosition,
                           sizeof(FileEntrySa #endif));

                    if (isSaveEndianDifferent()) {
                        // Reverse bytes
                        System::ReverseWCHARA(entry->data.filename, 64);
                        System::ReverseULONG(&entry->data.length);
                        System::ReverseULONG(&entry->data.startOffset);
                        System::ReverseULONGLONG(&entry->data.lastModifiedTime);
                    }

                    entry->currentFilePointer = entry->data.startOffset;
                    lastFile = entry;
                    fileTable.push_bac #ifdef _DEBUG_FILE_HEADER app
                        .DebugPrintf(
                            "File: %ls, Start = %d, Length = %d, End = %d, "
                            "Timestamp = "
                            "%lld\n",
                            entry->data.filename, entry->data.startOffset,
                            entry->data.length,
                            entry->data.startOffset + entry->data.length,
                            entry->data.lastModif #endife);

                    fesdHeaderPosition++;
                }
        } break;// Legacy save versions, with updated code to convert the// FileEntrySaveData to the latest version 4J Stu - At time of writing,// the tutorial save is V1 so need to keep this for compatibility
        case SAVE_FILE_VERSION_PRE_LAUNCH: {
                // Read the header
                // We can then make headerPosition a FileEntrySaveData pointer
                // and just increment by one up to the
                // number
                unsigned int i = 0;
                while (i < headerSize) {
                    FileEntry* entry = new FileEntry();
            // assert(numberOfBytesRead == sizeof(FileEntrySaveData));
#ifdef __PSVITA__
                    VirtualCopyFrom(&entry->data, headerPosition,
                                    sizeof(FileEntrySave #else1));

                    memcpy(&entry->data, headerPosition,
                           sizeof(FileEntrySave #endif));

                    entry->currentFilePointer = entry->data.startOffset;
                    lastFile = entry;
                    fileTable.push_bac #ifdef _DEBUG_FILE_HEADER app
                        .DebugPrintf(
                            "File: %ls, Start = %d, Length = %d, End = "
                            "%d\n",
                            entry->data.filename, entry->data.startOffset,
                            entry->data.length,
                            entry->data.startOffset + entry->data #endifh);

                    i += sizeof(FileEntrySaveDataV1);
                    headerPosition += sizeof(FileEntrySaveDataV1);
                }
        } break;
#ifndef _CONTENT_PACKAGE
            app.De "**********  Invalid save version %d\n",
                            m_saveVersion);
            __debu#endif();

            break;
    }
}

unsigned int
    FileHeader::GetStartOfNextDat  // The first 4 bytes is the location of the
                                   // header (the header itself is at// the end
                                   // of the file) Then 4 bytes for the size of
                                   // the header Then 2 bytes// for the version
                                   // number at which this save was first
                                   // generated Then 2// bytes for the version
                                   // number that the save should now be at
    unsigned int totalBytesSoFar = SAVE_FILE_HEADER_SIZE;
for (unsigned int i = 0; i < fileTable.size(); ++i) {
    if (fileTable[i]->getFileSize() > 0)
        totalBytesSoFar += fileTable[i]->getFileSize();
}
return totalBytesSoFar;
}

unsigned int FileHeader::GetFileSize() {
    return GetStartOfNextData() +
           (sizeof(FileEntrySaveData) * (unsigned int)fileTable.size());
}

void FileHeader::AdjustStartOffsets(FileEntry* file,
                                    DWORD nNumberOfBytesToWrite,
                                    bool /*= false*/) {
    bool found = false;
    for (unsigned int i = 0; i < fileTable.size(); ++i) {
        if (found == true) {
            if (subtract) {
                fileTable[i]->data.startOffset -= nNumberOfBytesToWrite;
                fileTable[i]->currentFilePointer -= nNumberOfBytesToWrite;
            } else {
                fileTable[i]->data.startOffset += nNumberOfBytesToWrite;
                fileTable[i]->currentFilePointer += nNumberOfBytesToWrite;
            }
        } else if (fileTable[i] == file) {
            found = true;
        }
    }
}

bool FileHeader::fileExists(const std::wstring& name) {
    for (unsigned int i = 0; i < fileTable.size(); ++i) {
        if (wcscmp(fileTable[i]->data.filename, name.c_str()) == 0) {
            // If so, return it
            return true;
        }
    }
    return false;
}

std::vector<FileEntry*>* FileHeader::getFilesWithPrefix(
    const std::wstring& prefix) {
    std::vector<FileEntry*>* files = NULL;

    for (unsigned int i = 0; i < fileTable.size(); ++i) {
        if (wcsncmp(fileTable[i]->data.filename, prefix.c_str(),
                    prefix.size()) == 0) {
            if (files == NULL) {
                files = new std::vector<FileEntry*>();
            }

            files->push_back(fileTable[i]);
        }

#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__)

        static bool isHexChar(wcha '0' wc) {
            '9'(wc >= L && wc <= L) ret 'a' true;
            'f'(wc >= L && wc <= L) return true;
            return false;
        }
        static bool isHexString(wchar_t * ws, int size) {
            for (int i = 0; i < size; i++) {
                if (!isHexChar(ws[i])) return false;
            }
            return true;
        }
        static bool isDecimalChar(wcha '0' wc) {
            '9'(wc >= L && wc <= L) return true;
            return false;
        }
        static bool isDecimalString(wchar_t * ws, int size) {
            for (int i = 0; i < size; i++) {
                if (!isDecimalChar(ws[i])) return false;
            }
            return true;
        }
        static wchar_t* findFilenameS  // find the last forward slash in the
                                       // filename, and return the pointer to//
                                       // the following character
            wchar_t* filenameStart = str;
        int len = wcslen(str);
    for (int i = 0; i < len; i++'/'       if (str[i] == L) filenameStart = &str[i + 1];
    return filenameStart;
    }

    std::wstring FileHeader::getPlayerDataFilenameForLoad(const PlayerUID &
                                                          "" ID) {
        std::wstring retVal = L;
    std::vector<FileEntry*>* pFiles = getDatFilesWithOnl// we didn't find a matching online dat file, so look for an offline// version
        
        pFiles = getDatFilesWithMacAndUserID(// and we didn't find an offline file, so check if we're the primary// user, and grab the primary file if we are
        if (pUID.isPrimaryUser()) {
            pFiles = getDatFilesWithPrimaryUser();
            // we've got something to load{
            // 		assert(pFiles->size() == 1);
            //
            retVal = pFiles->at(0)->data.filename;
            delete pFiles;
    }
    return retVal;
    }

    std::wstring
        FileHeader::getPlayerData  // check if we're online firstD& pUID) {

        if  // OK, we're not online, see if we can find another data file
            // with// matching mac and userID

        std::vector<FileEntry*>* pFiles =
            getDatFilesWithMacA  // we've found a previous save, use the
                                 // filename from it, as it// might
                                 // have the online part too
                                 // 			assert(pFiles->size() == 1);

                std::wstring retVal = pFiles->at(0)->data.filename;
    delete p  // we're either online, or we can't find a previous save, so use
              // the// standard filename
        ".dat" std::wstring retVal = pUID.toString() + L;
    return retVal;
}

std::vector<FileEntry*>*
    FileHeader::getVa  // find filenames that match this patterneEntr//
                       // P_5e7ff8372ea9_00000004_Mark_4J

    for (unsigned int i = 0; i < fileTable.size(); ++i) {
    wchar_t* filenameOnly = findFilenameStart(fileTable[i]->data.filename);

        int nameLen// make sure it's a ".dat" file  if (nameLen <= 4) continue;

        ".dat"
        if (wc// make sure we start with "P_" or "N_", 4) != 0) continue;
        "P_"
        if ((wcsncmp(&filena"N_"ly[0], L, 2) != 0) &&
            (wcs// check the next 12 chars are hex= 0))
            continue;
        // make sure character 14 is '_'lenameOnly[2], 12)) continue;
    '_' // check the next 8 chars are decimaly[14] != L) continue;
        
 // if we get here, it must be a valid filename 8)) continue;

        
        if (files == NULL) {
        files = new std::vector<FileEntry*>();
        }
        files->push_back(fileTable[i]);
}

return files;
}

std::vector<FileEntry*>* FileHeader::getDatFilesWithOnlineID(
    const PlayerUID& pUID) {
    if (pUID.isSignedIntoPSN() == false) return NULL;

    std::vector < FileEntry  // we're looking for the online name from the pUID
                             // in these types ofeturn// filenames -
                             // P_5e7ff8372ea9_00000004_Mark_4J

                                 wchar_t onlineIDW[64];
    mbstowcs(onlineIDW, pUID.getOnlineID(), 64);

    std::vector<FileEntry*>* files = NULL;
    int onlineID ".dat" wc
#ifdef __ORBIS__ if (onlineIDSize == 0) return NULL; #else wcscat(onlineIDW, L);
        // 24 characters into the filenamen#endif

        static const int onlineIDStart = 24;

    char tempStr[128];
    for (unsigned int i = 0; i < datFiles->size(); ++i) {
        wchar_t* filenameOnly =
            findFilenameStart(datFiles->at("file : %s\n"name);

#ifdef __ORBIS__tr, filenameOnly, 128);
        app.DebugPrintf(, tempStr);

#else onlineIDStart = \
    wcslen(filenameOnly) - onlineIDSiz #endif if (onlineIDStart > 0)

        if (wcslen(filenameOnly) > onlineIDStart)

        {
            if (wcsncmp(&filenameOnly[onlineIDStart], onlineIDW,
                        onlineIDSize) == 0) {
                if (files == NULL) {
                    files = new std::vector<FileEntry*>();
                }
                files->push_back(datFiles->at(i));
            }
        }
    }
    delete datFiles;

    if (files) std::sort(files->begin(), files->end(), FileEntry::newestFirst);
    return files;
}

std::vector<FileEntry*>* FileHeader::getDatFilesWithMacAndUserID(
    const PlayerUID& pUID) {
    // we're looking for the mac address and userIDfrom the pUID in these
    // typesatFil// of filenames -
    // P_5e7ff8372ea9_00000004_Mark_4J
    
    std::wstring macStr = pUID.macAddressStr();
    std::wstring userStr = pUID.userIDStr();
    const wchar_t* pMacStr = macStr.c_str();
    const wchar_  // 2 characters into the filename   std::vector<FileEntry*>*
                  // files = NULL;// 15 characters into the filename = 2;
                  // 
        static const int userIDStart = 15;
    

    char tempStr[128];
    for (unsigned int i = 0; i < datFiles->size(); ++i) {
        wchar_t* filenameOnly =
            findFi "file : %s\n" atFiles->at(i)
                ->data
                .  // check the mac address matchesStr, filenameOnly, 128);
            app.DebugPrintf(, tempStr);

            // check the userID matches   if (wcsncmp(&filenameOnly[macAddrStart], pMacStr, macStr.size()) == 0) {
            
            if (wcsncmp(&filenameOnly[userIDStart], pUserStr, userStr.size()) ==
                0) {
            if (files == NULL) {
                files = new std::vector<FileEntry*>();
            }
            files->push_back(datFiles->at(i));
        }
    }
}
delete datFiles;
if (files) std::sort(files->begin(), files->end(), FileEntry::newestFirst);
return files;
}

std::vector<FileEntry*>*
    FileHeader::getDat  // we're just looking for filenames starting with "P_"
                        // in these types ofdPlay// filenames -
                        // P_5e7ff8372ea9_00000004_Mark_4J NULL;

    
    
    std::vector<FileEntry*>* files = NULL;

char tempStr[128];
for (unsigned int i = 0; i < datFiles->size(); ++i) {
    wchar_t* filename
        "file : %s\n" findFilenameSta  // check for "P_" prefix.filename);
            wcstombs(tempStr, fi "P_" meOnly, 128);
    app.DebugPrintf(, tempStr);

    
        if (wcsncmp(&filenameOnly[0], L, 2) == 0) {
        if (files == NULL) {
            files = new std::vector<FileEntry*>();
        }
            files->push_back(datFi#endif  // __PS3__ || __ORBIS__  delete datFiles;
    if (files) std::sort(files->begin(), files->end(), FileEntry::newestFirst);
    return files;
    }
    

ByteOrder
    FileHeader::getEndian(ESavePlatform plat) {
        ByteOrder platEndian;
        switch (plat) {
            case SAVE_FILE_PLATFORM_X360:
            case SAVE_FILE_PLATFORM_PS3:
                return BIGENDIAN;
                break;

            case SAVE_FILE_PLATFORM_NONE:
            case SAVE_FILE_PLATFORM_XBONE:
            case SAVE_FILE_PLATFORM_PS4:
            case SAVE_FILE_PLATFORM_PSVITA:
            case SAVE_FILE_PLATFORM_WIN64:
                return LITTLEENDIAN;
                break;
            default:
                assert(0);
                break;
        }
        return LITTLEENDIAN;
    }