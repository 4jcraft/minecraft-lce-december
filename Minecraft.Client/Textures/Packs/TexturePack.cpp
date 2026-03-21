#include "../../Platform/stdafx.h"
#include "TexturePack.h"

std::wstring TexturePack::getPath(bool bTitleUpdateTexture /*= false*/,
                                  const char* pchBDPatchFileName /*= NULL*/) {
    std::wstring wDr #ifdef _XBOX if (bTitleUpdateTexture)
        // Make the content package point to to the UPDATE: drive is
        // needed#ifdef _TU_BUILD
        wDr "UPDATE:\\" #else

        wDr "GAME:\\res\\TitleUpdate\\" #endif
}
else {
    wDr "GAME:\\" #else
#ifdef __PS3__  // 4J-PB - we need to check for a BD patch - this is going to be
                // an issue for full DLC texture packs (Halloween)
        char* pchUsrDir = NULL;
    if (app.GetBootedFromDiscPatch() && pchBDPatchFileName != NULL) {
        pchUsrDir = app.GetBDUsrDirPath(pchBDPatchFileName);
        std::wstring wstr(pchUsrDir, pchUsrDir + strlen(pchUsrDir));

        if (bTitleUpdateTexture) {
            wDrive "\\Common\\res\\TitleUpdate\\";

        } else {
            wDrive "/Common/";
        }
    } else {
        pchUsrDir = getUsrDirPath();

        std::wstring wstr(pchUsrDir, pchUsrDir + strlen(pchUsrDir));

                if(bTitleUpdateT// Make the content package point to to the UPDATE: drive is needed
			w"\\Common\\res\\TitleUpdate\\";
    }
    else {
        w "/Common/" r + L
#elif __PSVITA__
	c
            "" r  // getUsrDirPath();
                std::wstring wstr(pchUsrDir, pchUsrDir + strlen(pchUsrDir));

        if(bTitle// Make the content package point to to the UPDATE: drive is needed"Common\\res\\TitleUpdate\\";
    }
    else "Common\\" = wst #else ;
}

	if(bTitle// Make the content package point to to the UPDATE: drive is needed"Common\\res\\TitleUpdate\\";
}
"Common/" wD #endif #endif ;
}



	return wDrive;
}
