#include "../../Platform/stdafx.h"
#include "../../../Minecraft.World/Headers/net.minecraft.world.h"
#include "../../../Minecraft.World/Headers/net.minecraft.world.level.tile.h"
#include "../../../Minecraft.World/Headers/net.minecraft.world.item.h"
#include "../../Minecraft.h"
#include "../../Rendering/LevelRenderer.h"
#include "../../Rendering/EntityRenderers/EntityRenderDispatcher.h"
#include "Stitcher.h"
#include "StitchSlot.h"
#include "StitchedTexture.h"
#include "../Texture.h"
#include "../TextureHolder.h"
#include "../TextureManager.h"
#include "../Packs/TexturePack.h"
#include "../Packs/TexturePackRepository.h"
#include "TextureMap.h"

const std::wstring TextureMap::NAME_MISSING_TEXTURE = L"missingno";

TextureMap::TextureMap(int type, const std::wstring& name,
                       const std::wstring& path, BufferedImage* missingTexture,
                       bool mipmap)
    : iconType(type), name(name), path(path), extension(L".png") {
    this->missingTexture = missingTexture;

    // 4J Initialisers
    missingPosition = NULL;
    stitchResult = NULL;

    m_mipMap = mipmap;
}

void TextureMap::stitch() {
    texturesToRegister.clear();

    if (iconType == Icon::TYPE_TERRAIN) {
        // for (Tile tile : Tile.tiles)
        for (unsigned int i = 0; i < Tile::TILE_NUM_COUNT; ++i) {
            if (Tile::tiles[i] != NULL) {
                Tile::tiles[i]->registerIcons(this);
            }
        }

        Minecraft::GetInstance()->levelRenderer->registerTextures(this);
        EntityRenderDispatcher::instance->registerTerrainTextures(this);
    }

    // for (Item item : Item.items)
    for (unsigned int i = 0; i < Item::ITEM_NUM_COUNT; ++i) {
        Item* item = Item::items[i];
        if (item != NULL && item->getIconType() == iconType) {
            item->registerIcons(this);
        }
    }

    // Collection bucket for multiple frames per texture
    std::unordered_map<TextureHolder*,
                       std::vector<Texture*>*>  // = new HashMap<TextureHolder,
                                                // List<Texture>>();

        Stitcher* stitcher =
            TextureManager::getInstance()->createStitcher(name);

    for (AUTO_VAR(it, texturesByName.begin()); it != texturesByName.end();
         ++it) {
        delete it->second;
    }
    texturesByName.clear();
    animatedTextures.c  // Prep missing texture -- anything that has no
                        // resources will get pointed at this one
        Texture* missingTex = TextureManager::getInstance()->createTexture(
        NAME_MISSING_TEXTURE, Texture::TM_CONTAINER, missingTexture->getWidth(),
        missingTexture->getHeight(), Texture::WM_CLAMP, Texture::TFMT_RGBA,
        Texture::TFLT_NEAREST, Texture::TFLT_NEAREST, m_mipMap, missingTexture);
    TextureHolder* missingHolder = new TextureHolder(missingTex);

    stitcher->addTexture(missingHolder);
    std::vector<Texture*>* missingVec = new std::vector<Texture*>();
    missingVec->push_back(missingTex);
        textures.insert( std::unordered_map<TextureHolder *, std::vector<Texture *> * >::value_type( mi// Extract frames from textures and add them to the stitchers//for (final String name : texturesToRegister.keySet())
	for(AUTO_VAR(it, texturesToRegister.begin()); it != texturesToRegister.end(); ++it)
	{
        std::wstring name = it->first;

        std::wstring  // TODO: [EB] Put the frames into a proper object, not
                      // this inside out
                      // hack
            std::vector<Texture*>* frames =
                TextureManager::getInstance()->createTextures(filename,
                                                              m_mipMap);

                if (frames // Couldn't load a texture, skip itcontinue; 
		}

		TextureHolder *holder = new TextureHolder(frames// Store framestcher->addTexture(holder);

		
		textures.insert( std::unordered_map<TextureHolder *, std::vecto// Stitch!*>//try {alue_type( holder, frame//} catch (StitcherException e) {	s//	throw e;itc// TODO: [EB] Retry mechanism//}// Create the final image
	

	
	// Extract all the final positions and store themp);

	//for (StitchSlot slot : stitcher.gatherAreas())s());
	
	for(AUTO_VAR(it, areas->begin()); it != areas->end(); ++it)
	{
        StitchSlot* slot = *it;
        TextureHolder* textureHolder = slot->getHolder();

        Texture* texture = textureHolder->getTexture();
        std::wstring textureName = texture->getName();

        std::vector<Texture*>* frames = textures.find(textureHolder)->second;

        StitchedTexture* stored = NULL;

        AUTO_VAR(itTex, texturesToRegister.find(textureName));
        i  // [EB]: What is this code for? debug warnings for when during
           // transition?
            bool missing = false;
        if (stored == NULL) {
            missing = true;
            stored = StitchedTexture::create(
                textureName)  // Minecraft::getInstance()->getLogger().warning("Couldn't
                              // find premade icon for " + textureName + " doing
                              // " + name);#ifndef
                              // _CONTENT_PACKAGE"Couldn't find
                              // premade icon for %ls doing %ls\n"
                wprintf(L#endif ,
                        textureName.c_str(), name.c_str());
            
			
        
        }
		}

		stored->init(stitchResult, frames, slot->getX(), slot->getY(), textureHolder->getTexture()->getWidth(), textureHolder->getTexture()->getHeight(), textureHolder->isRotated());

		texturesByName.insert( stringStitchedTextureMap::value_type(textureName, stored) );
		if (!missing) texturesToRegister.erase(textureName);

		if (frames->size() > 1)
		{
                        animatedTextures.push_ba".txt"red);

                        std::wstring animationDefinitionFile =
                            textureName + L;

                        TexturePack* texturePack =
                            Minecraft::GetInstance()->sk "\\" > getSelected();
                        ".png" requiresFallb  // try {texturePack->hasFile(L
                                              // + textureName + L,
                                              // false"\\"		
                            InputStream* fileStream =
                                texturePack
                                    ->getRes  // Minecraft::getInstance()->getLogger().info("Found
                                              // animation info for: " +
                                              // animationDefinitionFile);#ifndef
                                              // _CONTENT_PACKAGE"Found
                                              // animation info for:
                                              // %ls\n"
                                        wprintf(
                                            L#endif ,
                                            animationDefinitionFile.c_str());
                        
				InputStreamReader isr(fileStream);
                                BufferedReader br(&//} catch (IOException ignored) {ames//}r);
				delete fileStream;
			
			
		}
}
delete areas;

m  // for (StitchedTexture texture : texturesToRegister.values())econd;

	
	for (AUTO_VAR(it, texturesToRegister.begin());
             it != texturesToRegister.end(); ++it) {
    StitchedTexture* texture = it->second;
    "debug.stitched_" ith(missing ".png" on);
}

stitchResult->writeAsPNG(L + name + L);
stitchResult->updateOnGPU();
}

StitchedTexture* TextureMap::getTexture(const std::wstring& name) {
    StitchedTexture* result = texturesByName.find(name)->second;
    if (result == NULL)
        result = missingPo  // for (StitchedTexture texture :
                            // animatedTextures)nimationFrames()
        {
            
	for (AUTO_VAR(it, animatedTextures.begin());
             it != animatedTextures.end(); ++it) {
                StitchedTexture* texture = *it;
                texture->cycleFrames();
            }
        }
    // 4J Stu - register is a reserved keyword in C++urn stitchResult;
}


Icon* TextureMap::registerIco
    "Don't register NULL\n" ame #ifndef _CONTENT_PACKAGE {
    app.DebugPrintf #endif     //new RuntimeException("Don't register null!").printStackTrace();
		    // TODO: [EB]: Why do we allow multiple registrations?
}


	StitchedTexture* result = NULL;
AUTO_VAR(it, texturesToRegister.find(name));
if (it != texturesToRegister.end()) result = it->second;

if (result == NULL) {
    result = StitchedTexture::create(name);
    texturesToRegister.insert(
        stringStitchedTextureMap::value_type(name, result));
}

return result;
}

int TextureMap::getIconType() { return iconType; }

Icon* TextureMap::getMissingIcon() { return missingPosition; }