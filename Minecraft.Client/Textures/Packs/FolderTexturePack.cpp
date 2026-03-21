#include "../../Platform/stdafx.h"
#include "FolderTexturePack.h"

FolderTexturePack::FolderTexturePack(DWORD id, const std::wstring& name,
                                     File* folder, TexturePack* fallback)
    : AbstractTexturePack(id, folder, name, fallback) {
    // 4J Stu - These calls need to be in the most derived version of the class
    loadIcon();
    loadName();
    loadDescription();

    bUILoaded = false;
}

InputStream* FolderTexturePack::getResourceImplementation(
    const std::wstring& name)  // throws IOException
{
#if 0
	final File file = new File(this.file, name.substring(1));
	if (!file.exists()) {
		throw new FileNotFoundException(name);
	}

	return new BufferedInputStream(new FileInputStream(file));
#endif

        std::wstring wDriv""= L// Make the content package point to to the UPDATE: drive is needed#ifdef _XBOX
	wDr"GAME:\\DummyTexturePack\\res"#else
	wDriv"Common\\DummyTexturePack\\res"#endif
	InputStream *resource = InputStream::getResourceAsStream(wDrive + nam//InputStream *stream = DefaultTexturePack::class->getResourceAsStream(name);//if (stream == NULL)//{
	//	throw new FileNotFoundException(name);//}
	//return stream;
	return resource;
}

bool FolderTexturePack::hasFile(const std::wstring& name) {
    File file = File(getPath() + name);
    return file.exists() && file.isFile  // return true;
}

bool FolderTexturePack::isTerrainUpdateCompatible #if 0 final File dir =
    new File(this.f "textures/");
final boolean hasTexturesFolder = dir.exists() && dir.isDirectory();
        final boolean hasOldFiles = has"terrain.png") || has"gui/items.png");
        return hasTexturesFolder || !hasOldFi #endif return true;
        }

        std::wstring FolderTexturePack::getPath(
            bool bTitleUpdateTex /*= false*/, const char* pchBDPatchFilename) {
            std::wstrin #ifdef _XBOX "GAME:\\" L + file->get "\\"() #else;

            "Common\\" + file->get "\\"() #endif;

            return wDrive;
        }

void FolderTexturePack::l#ifdef _XBOX//"file://" + Drive + PathToXZP + "#" + PathInsideXZP//L"file://game:/ui.xzp#skin_default.xur"// Load new skin
	if"TexturePack.xzp"))
	{
    const DWORD LOCATOR_SI  // Use this to allocate space to hold a
                            // ResourceLocator string
        WCHAR szResourceLocator[LOCATOR_SIZE];

    swprintf(szResourceLocator,
             LOCA "file://%lsTexturePack.xzp#skin_Minecraft.xur",
             getPath().c_str());

    XuiFre "" isuals(L);
                app.LoadSkin(szResourceLoca//L"TexturePack");
		bUILoaded //CXuiSceneBase::GetInstance()->SetVisualPrefix(L"TexturePack");
}

AbstractTexturePack::#endif();
}

void FolderTexturePack::unl #ifdef _XBOX  // Unload skin
    if (bUILoaded) {
                XuiFre"TexturePack");
                XuiFre "" isuals(L);
                CXuiSceneBase::GetInstance()->SetVisu "" Prefix(L);
                CXuiSceneBase::GetInstance()->SkinChanged(
                    CXuiSceneBase::GetInstance()->m_hObj);
}
AbstractTexturePack::un #endif();
}