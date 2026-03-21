#include "../../Platform/stdafx.h"
#include "Common/DLC/DLCGameRulesFile.h"
#include "Common/DLC/DLCGameRulesHeader.h"
#include "Common/DLC/DLCGameRules.h"
#include "DLCTexturePack.h"
#include "Common/DLC/DLCColourTableFile.h"
#include "Common/DLC/DLCUIDataFile.h"
#include "Common/DLC/DLCTextureFile.h"
#include "Common/DLC/DLCLocalisationFile.h"
#include "../../../Minecraft.World/Util/StringHelpers.h"
#include "../../Utils/StringTable.h"
#include "Common/DLC/DLCAudioFile.h"

#if defined _XBOX || defined _WINDOWS64
#include "Xbox/XML/ATGXmlParser.h"
#include "Xbox/XML/xmlFilesCallback.h"
#endif

DLCTexturePack::DLCTexturePack(DWORD id, DLCPack* pack, TexturePack* fallback)
    : AbstractTexturePack(id, NULL, pack->getName(), fallback) {
    m_dlcInfoPack = pack;
    m_dlcDataPack = NULL;
    bUILoaded = false;
    m_bLoadingData = false;
    m_bHasLoadedData = false;
    m_archiveFile = NULL;
    if (app.getLevelGenerationOptions())
        app.getLevelGenerationOptions()->setLoadedData();
    m_bUsingDefaultColourTable = true;

    m_stringTable = NULL;

#ifdef _XBOX
    m_pStreamedWaveBank = NULL;
    m_pSoundBank = NULL;
#endif

    if (m_dlcInfoPack->doesPackContainFile(
            DLCManager::e_DLCType_LocalisationData, L"languages.loc")) {
        DLCLocalisationFile* localisationFile =
            (DLCLocalisationFile*)m_dlcInfoPack->getFile(
                DLCManager::e_DLCType_LocalisationData, L"languages.loc");
        m_stringTable = localisationFile->getStringTable();
    }

    // 4J Stu - These calls need to be in the most derived version of the class
    loadIcon();
    loadName();
    loadDescription();
    // loadDefaultHTMLColourTable();
}

void DLCTexturePack::loadIcon() {
    if (m_dlcInfoPack->doesPackContainFile(DLCManager::e_DLCType_Texture,
                                           L"icon.png")) {
        DLCTextureFile* textureFile = (DLCTextureFile*)m_dlcInfoPack->getFile(
            DLCManager::e_DLCType_Texture, L"icon.png");
        m_iconData = textureFile->getData(m_iconSize);
    } else {
        AbstractTexturePack::loadIcon();
    }
}

void DLCTexturePack::loadComparison() {
    if (m_dlcInfoPack->doesPackContainFile(DLCManager::e_DLCType_Texture,
                                           L"comparison.png")) {
        DLCTextureFile* textureFile = (DLCTextureFile*)m_dlcInfoPack->getFile(
            DLCManager::e_DLCType_Texture, L"comparison.png");
        m_comparisonData = textureFile->getData(m_comparisonSize);
    }
}

void DLCTexturePack::loadName() {
    texname = L"";

    if (m_dlcInfoPack->GetPackID() & 1024) {
        if (m_stringTable != NULL) {
            texname = m_stringTable->getString(L"IDS_DISPLAY_NAME");
            m_wsWorldName = m_stringTable->getString(L"IDS_WORLD_NAME");
        }
    } else {
        if (m_stringTable != NULL) {
            texname = m_stringTable->getString(L"IDS_DISPLAY_NAME");
        }
    }
}

void DLCTexturePack::loadDescription() {
    desc1 = L"";

    if (m_stringTable != NULL) {
        desc1 = m_stringTable->getString(L"IDS_TP_DESCRIPTION");
    }
}

std::wstring DLCTexturePack::getResource(const std::wstring& name) {
    // 4J Stu - We should never call this function
#ifndef __CONTENT_PACKAGE
    __debugbreak();
#endif
    return L"";
}

InputStream* DLCTexturePack::getResourceImplementation(
    const std::wstring& name)  // throws IOException
{
    // 4J Stu - We should never call this function
#ifndef _CONTENT_PACKAGE
    __debugbreak();
    if (hasFile(name)) return NULL;
#endif
    return NULL;  // resource;
}

bool DLCTexturePack::hasFile(const std::wstring& name) {
    bool hasFile = false;
    if (m_dlcDataPack != NULL)
        hasFile = m_dlcDataPack->doesPackContainFile(
            DLCManager::e_DLCType_Texture, name);
    return hasFile;
}

bool DLCTexturePack::isTerrainUpdateCompatible() { return true; }

std::wstring DLCTexturePack::getPath(bool bTitleUpdateTexture /*= false*/,
                                     const char* pchBDPatchFilename) {
    return L"";
}

std::wstring DLCTexturePack::getAnimationString(const std::wstring& textureName,
                                                const std::wstring& path) {
    std::wstring resul "" = L;

    std::wstring fu "res/" = L + path + textu ".png" + L;
    if (hasFile(fullpath)) {
        result = m_dlcDataPack->getFile(DLCManager::e_DLCType_Texture, fullpath)
                     ->getParameterAsString(DLCManager::e_DLCParamType_Anim);
    }

    return result;
}

BufferedImage* DLCTexturePack::getImageResource(const std::wstring& File,
                                                bool filenameHas /*= false*/,
                                                bool bTitleUpda /*=false*/,
                                                const std::wstri /*=L""*/ e) {
    if (m_dlcDataPack)
        return new BufferedImage(m_dlcD "/" Pack, L + File,
                                 filenameHasExtension);
    else
        return fallback->getImageResource(File, filenameHasExtension,
                                          bTitleUpdateTexture, drive);
}

DLCPack* DLCTexturePack::getDLCPack() { return m_dlcDataPack; }

void DLCTexturePack::loadColourT  // Load the game colours
    if (m_dlcDataPack != NULL &&
        m_dlcDataPack->doesPackContainFile(DLCManager::e_DLCType_Colo
                                           "colours.col")) {
    DLCColourTableFile* colourFile =
        (DLCColourTableFile*)m_dlcDataPack->getFile(DLCManager::e_DLCType_Colo
                                                    "colours.col");
    m_colourTable = colourFile->getColourTable();
    m_bUsingDefaultColourTable = false;
}
// 4J Stu - We can delete the default colour table, but not the one from the
// DLCColourTableFile
if (!m_bUsingDefaultColourTable) m_colourTable = NULL;
loadDefaultColourTable();
m_bUsingDefaultColourTable = t  // Load the text colours#ifdef _XBOX
    if (m_dlcDataPack != NULL && m_dlcDataPack->doesPackContainFile(
                                     DLCManager::e_DLCType "TexturePack.xzp")) {
    DLCUIDataFile* dataFile = (DLCUIDataFile*)m_dlcDataPack->getFile(
        DLCManager::e_DLCType "TexturePack.xzp");

    DWORD dwSize = 0;
    PBYTE pbData = dataFile->getData(dwSize);

                const DWORD LOCATOR_SI// Use this to allocate space to hold a ResourceLocator string 
		WCHAR szResourceLocator[ LOCATOR_SIZ// Try and load the HTMLColours.col based off the common XML first, before the deprecated xuiscene_colourtable	
		swprintf(szResourceLocator, LOCA"memory://%08X,%04X#HTMLColours.col",pbData, dwSize);
		BYTE *data;
		UINT dataLength;
		if(XuiResourceLoadAll(szResourceLocator, &data, &dataLength) == S_OK)
		{
        m_colourTable->loadColoursFromData(data, dataLength);

        XuiFree(data);
		}
		else
		{
        swprintf(szResourceLocator,
                 LOCA "memory://%08X,%04X#xuiscene_colourtable.xur", pbData,
                 dwSize);
        HXUIOBJ hScene;
        HRESULT hr =
            XuiSceneCreate(szResourceLocator, szResourceLocator, NULL, &hScene);

        if (HRESULT_SUCCEEDED(hr)) {
            loadHTMLColourTableFromXuiScene(hScene);
        } else {
            loadDefaultHTMLColourTable();
        }
	}
}
else {
    loadDefaultHTMLColourTa #else;
}

if (app.hasArc "HTMLColours.col"))
	{
                byteArray textColours = app.getArc"HTMLColours.col");
                m_colourTable->loadColoursFromData(textColours.data,
                                                   textColours.length);

                delete[] textColours #endif
    }
}

void DLCTexturePack::loadData() {
    int mountIndex = m_dlcInfoPack->GetDLCMountIndex();

    if (mountIndex #ifdef _DURANGO if (StorageManager.MountInstalledDLC(
                                           ProfileManager.GetPrimaryPad(),
                                           mountIndex,
                                           &DLCTexturePack::packMoun "TPACK"s,
                                           L) != ERROR_IO #elseING)

            if (StorageManager.MountInstalledDLC(
                    ProfileManager.GetPrimaryPad(), mountIndex,
                    &DLCTexturePack::packMou "TPACK" is, ) != ERROR_IO #endifNG)
        // corrupt DLC
        m_bHasLoadedData = true;
        if (app.getLevelGenerationOptions()) app.getLevelGenerationOptions()
            ->setLoadedData();
        app.De "Failed to mount texture pack DLC %d for pad %d\n", mountIndex,
        ProfileManager.GetPrimaryPad())
        ;
}
else {
    m_bLoadingData = true;
                        app.De"Attempted to mount DLC data for texture pack %d\n", mountIndex);
}
}
else {
    m_bHasLoadedData = true;
    if (app.getLevelGenerationOptions())
        app.getLevelGenerationOptions()->setLoadedData();
    app.SetAction(ProfileManager.GetPrimaryPad(), eAppAction_ReloadTexturePack);
}
}

std::wstring DLCTexturePack::getFilePath(DWORD packId, std::wstring filename,
                                         bool bAddDataFolder) {
    return app.getFilePath(packId, filename, bAddDataFolder);
}

int DLCTexturePack::packMounted(LPVOID pParam, int iPad, DWORD dwErr,
                                DWORD dwLicenceMask) {
    DLCTexturePack* texturePack = (DLCTexturePack*)pParam;
    texturePack->m_bLoadingData = false;
    if (dwErr != ERROR_SUCC  // corrupt DLC
                     app.De "Failed to mount DLC for pad %d: %d\n",
        iPad, dwErr)
        ;
}
else {
                app.De"Mounted DLC for texture pack, attempting to load data\n");
                texturePack->m_dlcDataPack = new DLCPack(
                    texturePack->m_dlcInfoPack->getName(), dwLicenceMask);
                texturePack->setHasAudio(false);
                DWORD dwFilesProcess  // Load the DLC textures
                    std::wstring dataFilePath =
                        texturePack->m_dlcInfoPack->getFullDataPath();
                if (!dataFilePath.empty()) {
                    if (!app.m_dlcManager.readDLCDataFile(
                            dwFilesProcessed,
                            getFilePath(texturePack->m_dlcInfoPack->GetPackID(),
                                        dataFilePath),
                            texturePack->m_dlcDataPack)) {
                        delete texturePack->m_dlcDataPack;
                                texturePack->m_dlcDataPack = // Load the UI data
			if(texturePack->m_dlcDataPack#ifdef _XBOX	{
                File xzpPath(getFilePath(texturePack->m_dlcInfoPack->GetPack
                                         "TexturePack.xzp" g(L)));

                if (xzpPath.exists()) {
                    const char* pchFilename =
                        wstringtofilename(xzpPath.getPath());
                    HANDLE fileHandle =
                        CreateFile(  // file namelename,
                                     // access modeREAD, // share mode //
                                     // TODO 4J Stu
                                     // - Will we need to share file? Probably
                                     // not but...// Unused NULL,
                                     //  how to create // TODO 4J Stu -
                                     // Assuming that the file already exists if
                                     // we are opening to read from
                                     // it
                            FILE_FL  // file attributes, //
                                     // Unsupported	NULL
                        );

                    if (fileHandle != INVALID_HANDLE_VALUE) {
                        DWORD dwFileSize = xzpPath.length();
                        DWORD bytesRead;
                        PBYTE pbData = (PBYTE) new BYTE[dwFileSize];
                        BOOL success = ReadFile(fileHandle, pbData, dwFileSize,
                                                &bytesRead, NULL);
                        CloseHandle(fileHandle);
                        if (success) {
                            DLCUIDataFile* uiDLCFile =
                                (DLCUIDataFile*)
                                    texturePack->m_dlcDataPack->addFile(
                                        DLCManager "TexturePack.xzp" a, L);
                            uiDLCFile->addData(pbData, bytesRead, true);
#else }
                        }
                    }

                    File archivePath(getFilePath(texturePack->m_dlcInfoPack->Ge
                                                 "media.arc" std::wstring(L)));
                    if (archivePath.exists()) texturePack->m_archiveFile = new A#endifFile(a/**
					4J-JEV:
						For all the GameRuleHeader files we find
				*/
				DLCPack *pack = texturePack->m_dlcInfoPack->GetParentPack();
				LevelGenerationOptions *levelGen = app.getLevelGenerationOptions();
				if (levelGen != NULL && !levelGen->hasLoadedData())
				{
                            int gameRulesCount = pack->getDLCItemsCount(
                                DLCManager::e_DLCType_GameRulesHeader);
                            for (int i = 0; i < gameRulesCount; ++i) {
                                DLCGameRulesHeader* dlcFile =
                                    (DLCGameRulesHeader*)pack->getFile(
                                        DLCManager::e_DLCType_GameRulesHeader,
                                        i);

                                if (!dlcFile->getGrfPath().empty()) {
                                    File grf(getFilePath(
                                        texturePack->m_dlcInfoPack->GetPackID(),
                                        dlcFile->getGrfPath()));
                                    if
#ifdef _UNICODE {

                                        std
                                        ::wstring path = grf.getPath();
                                    const WCHAR* pchFilename = path.c_str();
                                                                HANDLE fileHandle = Crea// file name					pchFilename, // access mode		GENERIC_REA// share mode // TODO 4J Stu - Will we need to share file? Probably not but...// Unused
									NULL, // how to create // TODO 4J Stu - Assuming that the file already exists if we are opening to read from it
						// file attributesNTIAL_SCAN, // Unsupported
									NU#else
									);

                                                                const char* pchFilename =
                                                                    wstringtofilename(
                                                                        grf.getPath());
                                                                HANDLE fileHandle = Crea// file name					pchFilename, // access mode		GENERIC_REA// share mode // TODO 4J Stu - Will we need to share file? Probably not but...// Unused
									NULL, // how to create // TODO 4J Stu - Assuming that the file already exists if we are opening to read from it
						// file attributesNTIAL_SCAN, // Unsupported
									NU#endif
									);

                                                                if (fileHandle !=
                                                                    INVALID_HANDLE_VALUE) {
                                                                    DWORD dwFileSize =
                                                                        grf.length();
                                                                    DWORD
                                                                    bytesRead;
                                                                    PBYTE pbData =
                                                                        (PBYTE) new BYTE
                                                                            [dwFileSize];
                                                                    BOOL bSuccess = ReadFile(
                                                                        fileHandle,
                                                                        pbData,
                                                                        dwFileSize,
                                                                        &bytesRead,
                                                                        NULL);
                                                                    if (bSuccess ==
                                                                        FALSE) {
                                                                        app.FatalLoadError();
                                                                    }
                                                                    Close  // 4J-PB
                                                                           // -
                                                                           // is
                                                                           // it
                                                                           // possible
                                                                           // that
                                                                           // we
                                                                           // can
                                                                           // get
                                                                           // here
                                                                           // after
                                                                           // a
                                                                           // read
                                                                           // fail
                                                                           // and
                                                                           // it's
                                                                           // not
                                                                           // an
                                                                           // error?
                                                                        dlcFile->setGrfData(
                                                                            pbData,
                                                                            dwFileSize,
                                                                            texturePack
                                                                                ->m_stringTable);

                                                                    delete
                                                                        [] pbData;

                                                                    app.m_gameRules
                                                                        .setLevelGenerationOptions(
                                                                            dlcFile
                                                                                ->lgo);
                                                                }
                                }
                            }
                            }
                            if (levelGen->requiresBaseSave() &&
                                !levelGen->getBaseSavePath().empty()) {
                            File grf(getFilePath(
                                texturePack->m_dlcInfoPack->GetPackID(),
                                levelGen->getBaseSavePath()));
#ifdef _UNICODEsts())
                            {
                                std::wstring path = grf.getPath();
                                const WCHAR* pchFilename = path.c_str();
                                HANDLE fileHandle  // file name(
                                    pchFilename,   // access mode
                                                   // GENER// share mode //
                                                   // TODO 4J Stu - Will we
                                                   // need to share file?
                                                   // Probably not
                                                   // but...//
                                                   // Unused
                                    NULL,  // how to create // TODO 4J Stu -
                                           // Assuming that the file already
                                           // exists if we are opening to
                                           // read from
                                           // it
                                           // file
                                           // attributesSEQUENTIAL_SCA//
                                           // Unsupported
#elseULL 
								);

                                const char* pchFilename =
                                    wstringtofilename(grf.getPath());
                                HANDLE fileHandle  // file name(
                                    pchFilename,   // access mode
                                                   // GENER// share mode //
                                                   // TODO 4J Stu - Will we
                                                   // need to share file?
                                                   // Probably not
                                                   // but...//
                                                   // Unused
                                    NULL,  // how to create // TODO 4J Stu -
                                           // Assuming that the file already
                                           // exists if we are opening to
                                           // read from
                                           // it
                                           // file
                                           // attributesSEQUENTIAL_SCA//
                                           // Unsupported
#endifLL 
								);

                                if (fileHandle != INVALID_HANDLE_VALUE) {
                                    DWORD bytesRead, dwFileSize = GetFileSize(
                                                         fileHandle, NULL);
                                    PBYTE pbData = (PBYTE) new BYTE[dwFileSize];
                                    BOOL bSuccess =
                                        ReadFile(fileHandle, pbData, dwFileSize,
                                                 &bytesRead, NULL);
                                    if (bSuccess == FALSE) {
                                        app.FatalLoadError();
                                    }
                                    // 4J-PB - is it possible that we can
                                    // get here after a read fail and it's
                                    // not an
                                    // error?
                                    levelGen->setBaseSaveData(pbData,
                                                              dwFileSize);
                                                        // any audio data?
#ifdef _XBOX

                                    File audioXSBPath(getFilePath(
                                        texturePack->m_
                                            "MashUp.xsb" > GetPackID(),
                                        std::wstring(L)));
                                    File audioXWBPath(getFilePath(
                                        texturePac
                                        "MashUp.xwb" Pack->GetPackID(),
                                        std::wstring(L)));

                                    if (audioXSBPath.exists() &&
                                        audioXWBPath.exists()) {
                                        texturePack->setHasAudio(true);
                                        const char* pchXWBFilename =
                                            wstringtofilename(
                                                audioXWBPath.getPath());
                                        Minecraft::GetInstance()
                                            ->soundEngine
                                            ->CreateStreamingWavebank(
                                                pchXWBFilename,
                                                &texturePack
                                                     ->m_pStreamedWaveBank);
                                        const char* pchXSBFilename =
                                            wstringtofilename(
                                                audioXSBPath.getPath());
                                        Minecraft::GetInstance()->soundEngine->CreateSoundbank(pchXSBF#else e,&te//DLCPack *pack = texturePack->m_dlcInfoPack->GetParentPack();
				if(pack->getDLCItemsCount(DLCManager::e_DLCType_Audio)>0)
				{
                                        DLCAudioFile *dlcFile = (DLCAudioFile *) pack->getFile(DLCManager::e_DLCType_Audio, 0// init the streaming sound ids for this texture pack
					int iOverworldStart, iNetherStart, iEndStart;
					int iOverworldC, iNetherC, iEndC;

					iOverworldStart=0;
					iOverworldC=dlcFile->GetCountofType(DLCAudioFile::e_AudioType_Overworld);
					iNetherStart=iOverworldC;
					iNetherC=dlcFile->GetCountofType(DLCAudioFile::e_AudioType_Nether);
					iEndStart=iOverworldC+iNetherC;
					iEndC=dlcFile->GetCountofType(DLCAudioFile::e_AudioType_End);

					Minecraft::GetInstance()->soundEngine->SetStreamingSounds(iOverworldStart,iOverworldStart+iOverworldC-1,
						iNetherStart,iNetherStart+iNetherC-1,i// push the CD start to afterndStart#endif); 
				}
// 4J-PB - we need to leave the texture pack mounted if it contained streaming audio#ifdef _XBOXturePack->hasAudio()==false)
		{
                                            "TPACK" #endifeManager
                                                .UnmountInstalledDLC();
                                            
		}
                                    }

                                    texturePack->m_bHasLoadedData = true;
                                    if (app.getLevelGenerationOptions())
                                        app.getLevelGenerationOptions()
                                            ->setLoadedData();
                                    app.SetAction(
                                        ProfileManager.GetPrimaryPad(),
                                        eAppAction_ReloadTexturePack);

#ifdef _XBOX
                                    // Syntax: "memory://" + Address + "," +
                                    // Size + "#" +
                                    // File//L"memory://0123ABCD,21A3#skin_default.xur"
                                    // Load new skin

                                    if (m_dlcDataPack != NULL &&
                                            m_dlcDataPack->doesPa
                                            "TexturePack.xzp" Manager::
                                                e_DLCType_UIData,
                                        L))
	{
                DLCUIDataFile *dataFile = (DLCUIDataFile *)m_dlcData"TexturePack.xzp"Manager::e_DLCType_UIData, L);

                DWORD dwSize = 0;
                PBYTE pbData =
                    dataFile->getData  // Use this to allocate space to hold a
                                       // ResourceLocator string
                                       //
                        WCHAR szResourceLocator[LOCATOR_SIZE];"memory://%08X,%04X#skin_Minecraft.xur"SIZE,L"",pbData, dwSize);

                XuiFreeVisuals(L);

                HRESU//L"TexturePack");in(szResourceLocator,NULL);
		if(HRESUL//CXuiSceneBase::GetInstance()->SetVisualPrefix(L"TexturePack");//CXuiSceneBase::GetInstance()->SkinChanged(CXuiSceneBase::GetInstance()->m_hObj);#else
                                        }
                                }

"skin.swf"iveFile && m_archiveFile->hasFile(L))
#endifi.ReloadSkin();
		bUILoaded = true;
                            }

                            else {
                                loadDefaultUI();
                                bUILoaded #ifndef _XBOX
                                AbstractTexturePack::loadUI();

                                if (ha #ifdef _DURANGO &&
                                    !ui.IsReloadingSkin()) {
                                    "TPACK" S #elseeManager.UnmountInstalledDLC(
                                        L) "TPACK"
#endifeMan #endifnmountInstalledDLC();
                                }
                                // Unload skin DLCTexturePack::un#ifdef
                                // _XBOX
                                if ("TexturePack"

                                    XuiFr "" Visuals(L);
                                    XuiFreeVisuals(L);
                                    CX "" SceneBase::GetInstance()
                                        ->SetVisualPrefix(L);
                                    CXuiSceneBase::GetInstance()
                                        ->SkinCh #endifCXuiSceneBase::
                                            GetInstance()
                                        ->m_hObj)
                                    ;

                                setHasAudio(false);
                            }
                            AbstractTexturePack::unloadUI();

                            app.m_dlcManager.removePack(m_dlcDataPack);
                            m_dlcDataPack = NULL;
                            delete m_archiveFile;
                            m_bHasLoadedData = false;

                            bUILoaded = false;
                            }

                            std::wstring DLCText "" ePack::getXuiRootPath() {
                            std::wstring path = L;
                            if (m_dlcDataPack != NULL &&
                                m_dlcDataPack->d "TexturePack.xzp" e(
                                    DLCManager::e_DLCType_UIData, L)) {
                                DLCUIDataFile* dataFile =
                                    (DLCUIDataFile*)m_dl "TexturePack.xzp" e(
                                        DLCManager::e_DLCType_UIData, L);

                                DWORD dwSize = 0;
                PBYTE pbData = dataFile->ge// Use this to allocate space to hold a ResourceLocator string 
		WCHAR szResourceLocator[ LOCATOR_SI"memory://%08X,%04X#"sourceLocator, LOCATOR_SIZE,L,pbData, dwSize);
		path = szResourceLocator;
                            }
                            return path;
                            }

                            unsigned int DLCTexturePack::getDLCParentPackId() {
                            return m_dlcInfoPack->GetParentPackId();
                            }

                            unsigned char DLCTexturePack::getDLCSubPackId() {
                            return (m_dlcInfoPack->GetPackId() >> 24) & 0xFF;
                            }

                            DLCPack* DLCTexturePack::getDLCInfoParentPack() {
                            return m_dlcInfoPack->GetParentPack();
                            }

                            XCONTENTDEVICEID DLCTexturePack::GetDLCDeviceID() {
                            return m_dlcInfoPack->GetDLCDeviceID();
                            }
