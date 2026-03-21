#include "../../Platform/stdafx.h"
#include "TexturePackRepository.h"
#include "DefaultTexturePack.h"
#include "FileTexturePack.h"
#include "FolderTexturePack.h"
#include "DLCTexturePack.h"
#include "../../GameState/Options.h"
#include "../../../Minecraft.World/IO/Files/File.h"
#include "../../../Minecraft.World/Util/StringHelpers.h"
#include "../../Rendering/Minimap.h"

TexturePack* TexturePackRepository::DEFAULT_TEXTURE_PACK = NULL;

TexturePackRepository::TexturePackRepository(File workingDirectory,
                                             Minecraft* minecraft) {
    if (!DEFAULT_TEXTURE_PACK) DEFAULT_TEXTURE_PACK = new DefaultTexturePack();

    // 4J - added
    usingWeb = false;
    selected = NULL;
    texturePacks = new std::vector<TexturePack*>;

    this->minecraft = minecraft;

    texturePacks->push_back(DEFAULT_TEXTURE_PACK);
    cacheById[DEFAULT_TEXTURE_PACK->getId()] = DEFAULT_TEXTURE_PACK;
    selected = DEFAULT_TEXTURE_PACK;

    DEFAULT_TEXTURE_PACK->loadColourTable();

    m_dummyTexturePack = NULL;
    m_dummyDLCTexturePack = NULL;
    lastSelected = NULL;

    updateList();
}

void TexturePackRepository::
    addDebugPacks #ifndef _CONTENT_PACKAGE    //File *file = new File(L"DummyTexturePack"); // Path to the test texture pack//m_dummyTexturePack = new FolderTexturePack(FOLDER_TEST_TEXTURE_PACK_ID, L"FolderTestPack", file, DEFAULT_TEXTURE_PACK);//texturePacks->push_back(m_dummyTexturePack);	//cacheById[m_dummyTexturePack->getId()] = m_dummyTexturePack;
#ifdef _XBOX
	File packedTestF "GAME:\\DummyTexturePack\\TexturePack.pck");
if (packedTestFile.exists()) {
                DLCPack *pack = app.m_dlcManager.getP"DLCTestPack");

                if (pack != NULL && pack->IsCorrupt()) {
                    app.m_dlcManager.removePack(pack);
                    pack = NULL;
                }

                if (pack == NULL) {
                        wpri"Pack \"%ls\" is not installed, so adding it\n""DLCTestPack");
                        pack = new DLCP"DLCTestPack",0xffffffff);
                        DWORD dwFilesProcessed = 0;
                        if (app.m_dlcManager.readDLCDataFile(
                                dwFilesProces
                                "GAME:\\DummyTexturePack\\TexturePack."
                                "pck",
                                pack)) {  // 4J Stu - Don't need to do this, as
                                          // the readDLCDataFile now adds
                                          // texture
                                          // packs//m_dummyDLCTexturePack =
                                          // addTexturePackFromDLC(pack,
                                          // DLC_TEST_TEXTURE_PACK_ID); //new
                                          // DLCTexturePack(0xFFFFFFFE,
                                          // L"DLCTestPack", pack,
                                          // DEFAULT_TEXTURE_PACK);
                            app.m_dlcManager.addPack(pack);
                        } else {
                            delete pack;
                        }

#endif  // _XBOX#endif // _CONTENT_PACKAGE
                }

void TexturePackRepository::createWorkingDirecoryUnlessExists(// 4J Unused#if 0
	if (!workDir.isDirectory()) {
        workDir.delete();
        workDir.mkdirs();
	}

	if (!multiplayerDir.isDirectory()) {
        multiplayerDir.delete();
        multiplayerDir.mkdirs() #endif 
}

bool TexturePackRepository::selectSkin(TexturePack *skin)
{
        if (skin == selected) return false;

        lastSelected = selected;
        usingWeb = false;
        selected = skin;  // minecraft->options->skin =
                          // skin->getName();//minecraft->options->save();
        return true;
}

void TexturePackRepository::selectWebSkin(const std::wstring &url)
{
        app.DebugPr
            "TexturePackRepository::selectWebSkin is not "
            "implemented\n" #if 0 String filename =
            url.substring(url.lastInd "/" f() + 1);
        if (filename.cont "?"s())
            filename = filename.substring(0, filename.ind "?" f());
        if (!filename.ends ".zip")) return;
        File file = new File(multiplayerDir, filename);
        downloadWebSkin(url, fi#endif
}

void TexturePackRepository::downloadWebSkin(const std::wstring &url, File file)
{
        app.DebugPr
            "TexturePackRepository::selectWebSkin is not "
            "implemented\n" #if 0 Map<String, String>
                headers = new HashMap<String, String>();
        final ProgressScreen listener = new ProgressScreen();
        headers"X-Minecraft-Username", minecraft.user.name);
        headers"X-Minecraft-Version", SharedConstants.VERSION_STRING);
        headers"X-Minecraft-Supported-Resolutions""16" );
        usingWeb = true;

        minecraft.setScreen(listener);

        HttpUtil.downloadTo(file, url, new HttpUtil.DownloadSuccessRunnable() {
		public void onDownloadSuccess(File file) {
			if (!usingWeb) return;

			selected = new FileTexturePack(getIdOrNull(file), file, DEFAULT_TEXTURE_PACK);
			minecraft.delayTextureReload();
		}
}
, headers, MAX_WEB_FILESIZE, listen #endif
}

bool TexturePackRepository::isUsingWebSkin() { return usingWeb; }

void TexturePackRepository::resetWebSkin() {
    usingWeb = false;
    updateList();
    minecraft->delayTextureReload();
}

void TexturePackRepository::updateList(// 4J Stu - We don't ever want to completely refresh the lists, we keep them up-to-date as we go#if 0
    std::vector<TexturePack *> *currentPacks = new std::vector<TexturePack *>;
	currentPacks->push_back(DEFAULT_TEXTURE_PACK);
	cacheById[DEFAULT_TEXTURE_PACK->getId()] = DEFAULT#ifndef _CONTENT_PACKAGE
	currentPacks->push_back(m_dummyTexturePack);	
	cacheById[m_dummyTexturePack->getId()] = m_dummyTexturePack;

	if(m_dummyDLCTexturePack != NULL)
	{
    currentPacks->push_back(m_dummyDLCTexturePack);
    cacheById[m_dummyDLCTexturePack->getId()] =
        m_dummyDLCTextur  // selected = m_dummyTexturePack;#endif

            selected = DEFAULT_TE  // 4J Unused

        for (File file : getWorkDirContents()) {
        final String id = getIdOrNull(file);
        if (id == null) continue;

        TexturePack pack = cacheById.get(id);
        if (pack == null) {
            pack = file.isDirectory()
                       ? new FolderTexturePack(id, file, DEFAULT_TEXTURE_PACK)
                       : new FileTexturePack(id, file, DEFAULT_TEXTURE_PACK);
            cacheById.put(id, pack);
        }

        if (pack.getName().equals(minecraft.options.skin)) {
            selected = pack;
        }
        currentPacks.a  // 4J - was texturePacks.removeAll(currentPacks);
            AUTO_VAR(itEnd, currentPacks->end());
        for (std::vector<TexturePack*>::iterator it1 = currentPacks->begin();
             it1 != itEnd; it1++) {
            for (std::vector<TexturePack*>::iterator it2 =
                     texturePacks->begin();
                 it2 != texturePacks->end(); it2++) {
                if (*it1 == *it2) {
                    it2 = texturePacks->erase(it2);
                }
            }
        }

        itEnd = texturePacks->end();
        for (std::vector<TexturePack*>::iterator it = texturePacks->begin();
             it != itEnd; it++) {
            TexturePack* pack = *it;
            pack->unload(minecraft->textures);
            cacheById.erase(pack->getId());
        }

        delete texturePacks;
#endifrePacks = currentPacks;
    }

    std::wstring TexturePackRepository::getIdOrNull(
        "TexturePackRepository::getIdOrNull is not "
        "implemented\n" #if 0);

        if (file.isFile() && file.get".zip".toLowerCase().endsWith()":"
		return file.getN":"() +  + file.length() +  + file.lastModified();
	} else if (file.is"pack.txt") && new File(file, ).exists()":folder:"rn file.getName() +  + file.last#endifed();
}

"" eturn NULL;

return L;
}

std::vector<File> TexturePackRepository::getWo"TexturePackRepository::getWorkDirContents is not implemented\n"#if 0);

if (workDir.exists() && workDir.isDirectory()) {
    return Arrays.asList(workDir.listFiles());
}
#endifurn Collections.emptyList();

return std::vector<File>();
}

std::vector < Textur  // 4J - note that original constucted a copy of
                      // texturePacks here
    return texturePacks;
}

TexturePack* TexturePackRepository::getSelected() {
    if (selected->hasData())
        return selected;
    else
        return DEFAULT_TEXTURE_PACK;
}

bool TexturePackRepository:"TexturePackRepository::shouldPromptForWebSkin is not implemented\n"#if 0);

if (!minecraft.options.serverTextures) return false;
ServerData data = minecraft.getCurrentServer();

if (data == null) {
    return true;
}
#endif{return data.promptOnTextures(); }

return false;
}

bool TexturePackRe"TexturePackRepository::canUseWebSkin is not implemented\n"#if 0);

if (!minecraft.options.serverTextures) return false;
ServerData data = minecraft.getCurrentServer();

if (data == null) {
    return false;
#endifse{return data.allowTextures(); }

    return false;
}

std::vector<std::pair<DWORD, std::wstring> >*
TexturePackRepository::getTexturePackIdNames() {
    std::vector<std::pair<DWORD, std::wstring> >* packList =
        new std::vector<std::pair<DWORD, std::wstring> >();

    for (AUTO_VAR(it, texturePacks->begin()); it != texturePacks->end(); ++it) {
        TexturePack* pack = *it;
        packList->push_back(
            std::pair<DWORD, std::wstring>(pack->getId(), pack->getName()));
    }
    return pac  // 4J-PB - add in a store of the texture pack required, so that
                // join from invite gamesec// (where they don't have the texture
                // pack) can check this when the texture pack is installed

        app.SetRequiredTexturePackID(id);

    AUTO_VAR(it, cacheById.find(id));
    if (it != cacheById.end()) {
        TexturePack* newPack = it->second;
        if (newPack != selected) {
            selectSkin(newPack);

            if (newPack->hasData()) {
                                app.SetAction(ProfileManager//Minecraft *pMinecraft = Minecraft::GetInstance();	}
                                // pMinecraft->textures->reloadAll();
            }
            "TexturePack with id %d is already selected\n"

        } else {
                        app.DebugPrintf("Failed to select texture pack %d as it is not in the list\n" = true#ifndef _CONTENT_PACKAGEgPr// TODO - 4J Stu: We should report this to the player in some way, //__debugbreak();#endif// Fail safely
		

		
		if( selectSkin( DEFAULT_TEXTURE_PACK ) )
		{
                app.SetAction(ProfileManager.GetPrimaryPad(),
                              eAppAction_ReloadTexturePack);
		}
        }
        return bDidSelect;
    }

    TexturePack* TexturePackRepository::getTexturePackById(DWORD id) {
        AUTO_VAR(it, cacheById.find(id));
        if (it != cacheById.end()) {
            return it->second;
        }

        return NULL;
    }

    TexturePack* Textur  // 4J-PB - The City texture pack went out with a child
                         // id for the texture pack of 1 instead of zero
                         // // we need to mask off the child id here to deal
                         // with this// child id is <<24 and Or'd with parent
                             DWORD dwParentID = id & 0xFFFFFF;

    if (dlcPack != NULL) {
                newPack = new DLCTexturePack(dwParentID, dlc
#ifndef _CONTENT_PACKAGEK);
		texturePacks->push_back(newPack);
		cacheById[dwParentID] = n""Pack;
"Added new FULL DLCTexturePack: %ls - id=%d\n"CManager::e_DLCType_TexturePack,L))
		{
                    wprintf(L"Added new TRIAL DLCTexturePack: %ls - id=%d\n" e()
                                .c_str(),
                            dwParentID);
                }
                else {
                        wpr#endif, dlcPack->getName().c_str(),dwParentID );
                }
    }
    return newPack;
}

void TexturePackRepository::clearInvalidTexturePacks() {
    for (AUTO_VAR(it, m_texturePacksToDelete.begin());
         it != m_texturePacksToDelete.end(); ++it) {
        delete *it;
    }
}

void TexturePackRepository::removeTexturePackById(DWORD id) {
    AUTO_VAR(it, cacheById.find(id));
    if (it != cacheById.end()) {
        TexturePack* oldPack = it->second;

        AUTO_VAR(it2, std::find(texturePacks->begin(), texturePacks->end(),
                                oldPack));
        if (it2 != texturePacks->end()) {
            texturePacks->erase(it2);
            if (lastSelected == oldPack) {
                lastSelected = NULL;
            }
        }
        m_texturePacksToDelete.push_back(oldPack);
    }
}

void TexturePackRepository::updateUI() {
    if (lastSelected != NULL && lastSelected != selected) {
        lastSelected->unloadUI();
        selected->loadUI();
        Minimap::reloadColours();
        ui.StartReloadSkinThread();
        lastSelected = NULL;
    }
}

bool TexturePackRepository::needsUIUpdate() {
    return lastSelected != NULL && lastSelected != selected;
}

unsigned int TexturePackRepository::getTexturePackCount() {
    return texturePacks->size();
}

TexturePack* TexturePackRepository::getTexturePackByIndex(unsigned int index) {
    TexturePack* pack = NULL;
    if (index < texturePacks->size()) {
        pack = texturePacks->at(index);
    }
    return pack;
}

unsigned int TexturePackRepository::getTexturePackIndex(unsigned int id) {
    int currentIndex = 0;
    for (AUTO_VAR(it, texturePacks->begin()); it != texturePacks->end(); ++it) {
        TexturePack* pack = *it;
        if (pack->getId() == id) break;
        ++currentIndex;
    }
    if (currentIndex >= texturePacks->size()) currentIndex = 0;
    return currentIndex;
}