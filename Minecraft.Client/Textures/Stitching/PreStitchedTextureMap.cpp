#include "../../Platform/stdafx.h"
#include "../../../Minecraft.World/Headers/net.minecraft.world.h"
#include "../../../Minecraft.World/Headers/net.minecraft.world.level.tile.h"
#include "../../../Minecraft.World/Headers/net.minecraft.world.item.h"
#include "../../../Minecraft.World/IO/Streams/ByteBuffer.h"
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
#include "PreStitchedTextureMap.h"
#include "../../UI/SimpleIcon.h"
#include "../CompassTexture.h"
#include "../ClockTexture.h"

const std::wstring PreStitchedTextureMap::NAME_MISSING_TEXTURE = L"missingno";

PreStitchedTextureMap::PreStitchedTextureMap(int type, const std::wstring& name,
                                             const std::wstring& path,
                                             BufferedImage* missingTexture,
                                             bool mipmap)
    : iconType(type), name(name), path(path), extension(L".png") {
    this->missingTexture = missingTexture;

    // 4J Initialisers
    missingPosition = NULL;
    stitchResult = NULL;

    m_mipMap = mipmap;
    missingPosition = (StitchedTexture*)(new SimpleIcon(
        NAME_MISSING_TEXTURE, NAME_MISSING_TEXTURE, 0, 0, 1, 1));
}

void PreStitchedTextureMap::stitch() {
    // Animated StitchedTextures store a vector of textures for each frame of
    // the animation. Free any pre-existing ones here.
    for (AUTO_VAR(it, animatedTextures.begin()); it != animatedTextures.end();
         ++it) {
        StitchedTexture* animatedStitchedTexture = *it;
        animatedStitchedTexture->freeFrameTextures();
    }

    loadUVs();

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

    animatedTextures.c  // Create the final image
        std::wstring filename = name + extension;

    TexturePack* texturePack =
        Minecraft::GetInstance()->skins->g  // try {ted();
	
	int mode = Texture::TM_DYNAMIC;
    int clamp = Tex  // 4J Stu - Don't clamp as it causes issues with how we
                     // signal non-mipmmapped textures to the pixel shader
                     // //Texture::WM_CLAMP;
        int minFilter = Texture::TFLT_NEAREST;
    int magFilter = Texture::TFLT_NEAREST;

    MemSect(32);
    st ""
        : wst  // 4J-PB - need to check for BD patched files#ifdef __PS3__

	const char* pchName = wstringtofilename(filename);
    if (app.GetBootedFromDiscPatch() && app.IsFileInPatchList(pchName)) {
        if (te "res/" ack->hasFile(L + filename, false)) {
            drive = texturePack->getPath(true, pchName);
        } else {
            drive = Minecraft::GetInstance()->skins->getDefault()->getPath(
                true, pchName);
            texturePack = Minecraft::GetInstance()->skins->getDefa #endif
        }
    } else
        
	if (te "res/" ack->hasFile(L + filename, false)) {
            drive = texturePack->getPath(true);
        }
    else {
        drive = Minecraft::GetInstance()->skins->getDefault()->getPath(true);
        texturePack =
            Minecraft::GetInstance()
                ->skins -  // BufferedImage *image = new
                           // BufferedImage(texturePack->getResource(L"/"
                           // + filename),false,true,drive);
                           // //ImageIO::read(texturePack->getResource(L"/"
                           // + filename));
            BufferedImage* image =
                texturePack->getImageResource(filename, false, true, drive);
        MemSect(0);
        int height = image->getHeight();
        int width = image->getWidth();

        if (stitchResult != NULL) {
            TextureManager::getInstance()->unregisterTexture(name,
                                                             stitchResult);
            delete stitchResult;
        }
        stitchResult = TextureManager::getInstance()->createTexture(
            name, Texture::TM_DYNAMIC, width, height, Texture::TFMT_RGBA,
            m_mipMap);
        stitchResult->transferFromImage(image);
        delete image;
        TextureManager::getInstance()->registerName(na//stitchResult = stitcher->constructTexture(m_mipMap);

	for(AUTO_VAR(it, texturesByName.begin()); it != texturesByName.end(); ++it)
	{
            StitchedTexture* preStitched = (StitchedTexture*)it->second;

            int x = preStitched->getU0() * stitchResult->getWidth();
            int y = preStitched->getV0() * stitchResult->getHeight();
            int width = (preStitched->getU1() * stitchResult->getWidth()) - x;
            int height = (preStitched->getV1() * stitchResult->getHeight()) - y;

            preStitched->init(stitchResult, NULL, x, y, width, height, false);
	}

	MemSect(52);
	for(AUTO_VAR(it, texturesByName.begin()); it != texturesByName.end(); ++it)
	{
            StitchedTexture* preStitched = (StitchedTexture*)(it->second);

            makeTextureAnimated(
                texturePack,
                preStitched);  // missingPosition = (StitchedTexture
                               // *)texturesByName.find(NAME_MISSING_TEXTURE)->second;

            stitch "debug.stitched_" G(L".png" + name + L);
            stitchRes

#ifdef __PSVITA__
    // AP - alpha cut out is expensive on vita so we mark which icons actually require it
                DWORD* data =
                    (DWORD*)this->getStitchedTexture()->getData()->getBuffer();
            int Width = this->getStitchedTexture()->getWidth();
            int Height = this->getStitchedTexture()->getHeight();
            for (AUTO_VAR(it, texturesByName.begin());
                 it != texturesByName.end(); ++it) {
                StitchedTexture* preStitched = (StitchedTexture*)it->second;

                bool Found = false;
                int u0 = preStitched->getU0() * Width;
                int u1 = preStitched->getU1() * Width;
                int v0 = preStitched->getV0() * Height;
                int v1 = preStitched->g  // check all the texels for this icon.
                                         // If ANY are transparent we mark it as
                                         // 'cut out'
                         for (int v = v0; v < v1; v += 1) {
                        for( int u = u0;u < u// is this texel alpha value < 0.1
				if( (data[v * Width + u] & 0xff000000) < 0x20// this texel is transparent. Mark the icon as such and bail
					preStitched->setFlags(Icon::IS_ALPHA_CUT_OUT);
					Found = true;
					break;
                }
            }

            i  // move onto the next icon
#endif;
			}
    }
}



}

void PreStitchedTextureMap::makeTextureAnimated(TexturePack* texturePack,
                                                StitchedTexture* tex) {
    if (!tex->hasOwnData()) {
        animatedTextures.push_back(tex);
        return;
    }

    std::wstring textureFileName = tex->m_fileName;

    std::wstring animString =
        texturePack->getAnimationString(textureFileName, path, true);

    if (!animString.empty()) {
        std::wstring filename =
            path  // TODO: [EB] Put the frames into a proper object, not this
                  // inside out hack
                std::vector<Texture*>* frames =
                    TextureManager::getInstance()->createTextures(filename,
                                                                  m_mipMap);
                if (frames == // Couldn't load a texture, skip iturn; 
#ifndef _CONTENT_PACKAGEframes->at(0);

		if(first->getWidth() != tex->getWidth() || first->getHeight() != t"%ls - first w - %d, h - %d, tex w - %d, h - %d\n",textureFileName.c_str(),first->getWidth(),tex->getWidth(),first->getHeight(),te#endifHeight());
			__debugbreak();
    }
    

		tex->init(stitchResult, frames, tex->getX(), tex->getY(),
                          first->getWidth(), first->getHeight(), false);

    if (frames->size() > 1) {
        animatedTextures.push_back(tex);

        tex->loadAnimationFrames(animString);
    }
}
}

StitchedTexture *PreStitchedTextureMap:#ifndef _CONTENT_PACKAGEstring &name)
{
    "Not implemented!\n"
	app.DebugPrintf(#endif ) #if 0debugbreak();
    
	return NULL;
    
	StitchedTexture* result = texturesByName.find(name)->second;
    if (result == NULL) re #endif missingPosition;
    return result;
    


}

void PreStitch  // for (StitchedTexture texture :
                // animatedTextures)
    for (AUTO_VAR(it, animatedTextures.begin()); it != animatedTextures.end();
         ++it) {
    StitchedTexture* texture = *it;
    texture->cycleFrames();
}
}

Texture* PreStitchedTextureMap::
    getStitc  // 4J Stu - register is a reserved keyword in
              // C++
    Icon* PreStitchedTextureMap::registerIcon(const std::wstring& name) {
    Icon* result = NULL;
"Don't register NULL\n"	ap#ifndef _CONTENT_PACKAGE);
#endif
		__debugbreak(//new RuntimeException("Don't register null!").printStackTrace();
}

AUTO_VAR(it, texturesByName.find(name));
if (it != texturesByName.end()) result#ifndef _CONTENT_PACKAGEult == NULL)
	{
        "Could not find uv data for icon "
           "%ls\n"(#endifname.c_str());
        __debugbreak();
        
		result = missingPosition;
    }

return result;
}

int PreStitchedTextureMap::getIconType() { return iconType; }

Icon* PreStitchedTextureMap::getMis
#define ADD_ICON(row, column, name)                                         \
    (texturesByName[name] =                                                 \
         new SimpleIcon(name, name, horizRatio * column, vertRatio * row,   \
                        horizRatio * (column + 1), vertRatio * (row + 1))); \
    #define ADD_ICON_WITH_NAME(row, column, name, filename)(               \
        texturesByName[name] = new SimpleIcon(                              \
            name, filename, horizRatio * column, vertRatio * row,           \
            horizRatio * (column + 1), vertRatio * (row + 1)));             \
    #define ADD_ICON_SIZE(row, column, name, height, width)(               \
        texturesByName[name] = new SimpleIcon(                              \
            name, name, horizRatio * column, vertRatio * row,               \
            horizRatio * (column + width), vertRatio * (row + height)));    \
    

void PreStitchedTextureMap::                                                \
        loadUV  // 4J Stu - We only need to populate this once at the moment as
                // we have hardcoded positions for each texture// If we ever
                // load that dynamically, be aware that the Icon objects could
                // currently be being used by the// GameRenderer::runUpdate
                // thread
		
		return;
}

for (AUTO_VAR(it, texturesByName.begin()); it != texturesByName.end(); ++it) {
    delete it->second;
}
texturesByName.clear();

if (iconType != Icon::TYPE_TERRAIN) {
    float horizRatio = 1.0f / 16.0f;
    float vert "helmetCloth" 16.0f;

    ADD_ICON(0, "helmetChain")
    ADD_ICON(0, "helmetIron")
    ADD_ICON(0 "helmetDiamond")
    ADD_ICON(0, "helmetGold")
    ADD_ICON(0 "flintAndSteel")
    ADD_ICON(0, "flint")
                ADD_I"coal"		6,	L)
		ADD_"string"	7,	L)
		ADD_IC"seeds"8,	L)
		ADD_IC"apple"9,	L)
		ADD_ICO"appleGold")
		ADD_ICON(0,"egg"	L)
		ADD_I"sugar"	12,	L)
		ADD_ICO"snowball"L)
		ADD_ICON(0"slot_empty_helmet"
		ADD_ICON(0,		15,	L"chestplateCloth")

		ADD_ICON(1,		0,"chestplateChain")
		ADD_ICON(1,		1,"chestplateIron")
		ADD_ICON(1,		2"chestplateDiamond")
		ADD_ICON(1,		3,	L"chestplateGold")
		ADD_ICON(1,		4"bow")
		ADD"brick",		5,	L)
		ADD_I"ingotIron"L)
		ADD_ICON("feather")
		ADD_ICO"wheat",	L)
		ADD_IC"painting"L)
		ADD_ICON(1"reeds"L)
		ADD_ICO"bone"11,	L)
		ADD_IC"cake"	12,	L)
		ADD_IC"slimeball"L)
		ADD_ICON(1,"slot_empty_chestplate"DD_ICON(1,		15,	L"leggingsCloth")

		ADD_ICON(2,		"leggingsChain")
		ADD_ICON(2,		"leggingsIron")
		ADD_ICON(2,	"leggingsDiamond")
		ADD_ICON(2,		3,"leggingsGold")
		ADD_ICON(2,	"arrow")
		ADD_I"quiver"5,	L)
		ADD_IC"ingotGold")
		ADD_ICON("sulphur")
		ADD_ICO"bread",	L)
		ADD_IC"sign"	9,	L)
		ADD_IC"doorWood"	L)
		ADD_ICON(2"doorIron")
		ADD_ICON(2"bed",	L)
		ADD_I"fireball",	L)
		ADD_ICON(2"slot_empty_leggings"	ADD_ICON(2,		15,	L"bootsCloth")

		ADD_ICON(3"bootsChain")
		ADD_ICON(3"bootsIron")
		ADD_ICON("bootsDiamond")
		ADD_ICON(3,	"bootsGold")
		ADD_ICON("stick"L)
		ADD_I"compass",	L)
		ADD_ICO"diamond"L)
		ADD_ICO"redstone")
		ADD_ICON"clay",	L)
		ADD_I"paper"	9,	L)
		ADD_ICO"book"10,	L)
		ADD_IC"map"		11,	L)
		ADD_I"seeds_pumpkin")
		ADD_ICON(3,		13"seeds_melon")
		ADD_ICON(3,		"slot_empty_boots")
		ADD_ICON(3,		15,	L"swordWood")

		ADD_ICON("swordStone")
		ADD_ICON(4"swordIron")
		ADD_ICON("swordDiamond")
		ADD_ICON(4,	"swordGold")
		ADD_ICON("fishingRod_uncast")
		ADD_ICON(4,		5,	L"clock")
		ADD_I"bowl"		6,	L)
		ADD_"mushroomStew")
		ADD_ICON(4,	"yellowDust")
		ADD_ICON(4,"bucket")
		ADD_ICON"bucketWater")
		ADD_ICON(4,		"bucketLava")
		ADD_ICON(4,	"milk")
		ADD_IC"dyePowder_black")
		ADD_ICON(4,		14,	"dyePowder_gray")
		ADD_ICON(4,		15,"shovelWood")

		ADD_ICON(5"shovelStone")
		ADD_ICON(5,"shovelIron")
		ADD_ICON(5"shovelDiamond")
		ADD_ICON(5,		"shovelGold")
		ADD_ICON(5"fishingRod_cast")
		ADD_ICON(5,		5,"diode")
		ADD_I"porkchopRaw")
		ADD_ICON(5,"porkchopCooked")
		ADD_ICON(5,		8"fishRaw")
		ADD_ICON"fishCooked")
		ADD_ICON(5,	"rottenFlesh")
		ADD_ICON(5,		"cookie")
		ADD_ICON"shears"	L)
		ADD_ICON"dyePowder_red")
		ADD_ICON(5,		14"dyePowder_pink")
		ADD_ICON(5,		15,"pickaxeWood")

		ADD_ICON(6,"pickaxeStone")
		ADD_ICON(6,	"pickaxeIron")
		ADD_ICON(6,"pickaxeDiamond")
		ADD_ICON(6,		3"pickaxeGold")
		ADD_ICON(6,"bow_pull_0")
		ADD_ICON(6"carrotOnAStick")
		ADD_ICON(6,		6"leather")
		ADD_ICO"saddle"	L)
		ADD_IC"beefRaw"	L)
		ADD_ICON"beefCooked")
		ADD_ICON(6,	"enderPearl")
		ADD_ICON(6,	"blazeRod")
		ADD_ICON(6"melon"L)
		ADD_ICO"dyePowder_green")
		ADD_ICON(6,		14,	"dyePowder_lime")
		ADD_ICON(6,		15,"hatchetWood")

		ADD_ICON(7,"hatchetStone")
		ADD_ICON(7,	"hatchetIron")
		ADD_ICON(7,"hatchetDiamond")
		ADD_ICON(7,		3"hatchetGold")
		ADD_ICON(7,"bow_pull_1")
		ADD_ICON(7"potatoBaked")
		ADD_ICON(7,"potato")
		ADD_IC"carrots"	L)
		ADD_ICO"chickenRaw")
		ADD_ICON(7,"chickenCooked")
		ADD_ICON(7,		10"ghastTear")
		ADD_ICON(7,"goldNugget")
		ADD_ICON(7,	"netherStalkSeeds")
		ADD_ICON(7,		13,	L"dyePowder_brown")
		ADD_ICON(7,		14,	"dyePowder_yellow")
		ADD_ICON(7,		15,	L"hoeWood")

		ADD_ICO"hoeStone")
		ADD_ICON"hoeIron")
		ADD_ICO"hoeDiamond")
		ADD_ICON(8"hoeGold")
		ADD_ICO"bow_pull_2")
		ADD_ICON(8"potatoPoisonous")
		ADD_ICON(8,		6,"minecart")
		ADD_ICON"boat",	L)
		ADD_"speckledMelon")
		ADD_ICON(8,		9"fermentedSpiderEye"		ADD_ICON(8,		10,	L"spiderEye")
		ADD_ICON(8,"potion")
		ADD_ICON"glassBottle"// Same as potion,		12,	L"potion_contents"
		ADD_ICON(8,		13,	"dyePowder_blue")
		ADD_ICON(8,		14,"dyePowder_light_blue"ADD_ICON(8,		15,	L"helmetCloth_overlay"		AD//ADD_ICON(9,		1,	L"unused"))
		"iron_horse_armor"
		ADD_ICON(9,		2,	"diamond_horse_armor"		ADD_ICON(9,		3,	L"gold_horse_armor")
		ADD_ICON(9,		4,	"comparator")
		ADD_ICON(9"carrotGolden")
		ADD_ICON(9,	"minecart_chest")
		ADD_ICON(9,		7"pumpkinPie")
		ADD_ICON(9"monsterPlacer")
		ADD_ICON(9,		9"potion_splash")
		ADD_ICON(9,		10"eyeOfEnder")
		ADD_ICON(9,	"cauldron")
		ADD_ICON(9"blazePowder")
		ADD_ICON(9,		"dyePowder_purple")
		ADD_ICON(9,		14,	L"dyePowder_magenta"
		ADD_ICON(9,		15,	L"chestplateCloth_overlay"D_IC//ADD_ICON(10,	1,	L"unused")//ADD_ICON(10,	2,	L"unused")
		"name_tag"
		ADD_ICON"lead",	L)
		ADD_"netherbrick"//ADD_ICON(10,	6,	L"unused"))
		"minecart_furnace"
		ADD_ICON(10,	7,	"charcoal")
		ADD_ICON"monsterPlacer_overlay"ADD_ICON(10,	9,	L"ruby")
		ADD_IC"expBottle"L)
		ADD_ICON(10"brewingStand")
		ADD_ICON(10,	1"magmaCream")
		ADD_ICON(10,"dyePowder_cyan")
		ADD_ICON(10,	14,"dyePowder_orange")
		ADD_ICON(10,	15,	L"leggingsCloth_overlay"ADD_//ADD_ICON(11,	1,	L"unused")//ADD_ICON(11,	2,	L"unused")//ADD_ICON(11,	3,	L"unused")//ADD_ICON(11,	4,	L"unused")//ADD_ICON(11,	5,	L"unused")//ADD_ICON(11,	6,	L"unused")
		"minecart_hopper"
		ADD_ICON(11,	7,"hopper")
		ADD_IC"nether_star")
		ADD_ICON(11,"emerald")
		ADD_ICON("writingBook")
		ADD_ICON(11,	"writtenBook")
		ADD_ICON(11,	"flowerPot")
		ADD_ICON(11"dyePowder_silver")
		ADD_ICON(11,	14,	L"dyePowder_white")
		ADD_ICON(11,	15,	"bootsCloth_overlay"
		A//ADD_ICON(12,	1,	L"unused")//ADD_ICON(12,	2,	L"unused")//ADD_ICON(12,	3,	L"unused")//ADD_ICON(12,	4,	L"unused")//ADD_ICON(12,	5,	L"unused")//ADD_ICON(12,	6,	L"unused")
		"minecart_tnt"//ADD_ICON(12,	8,	L"unused"))
		"fireworks"
		ADD_ICON(1"fireworks_charge")
		ADD_ICON(12,	10,	L"fireworks_charge_overlay"ICON(12,	11,	L"netherquartz")
		ADD_ICON(12,	1"map_empty")
		ADD_ICON(12"frame")
		ADD_ICO"enchantedBook")
		ADD_ICON(12,	15"skull_skeleton")

		ADD_ICON(14,	0"skull_wither")
		ADD_ICON(14,"skull_zombie")
		ADD_ICON(14,"skull_char")
		ADD_ICON(1"skull_creeper"//ADD_ICON(14,	5,	L"unused")//ADD_ICON(14,	6,	L"unused")
		"compassP0"		A"compass"IT// 4J Added7,	L, L) "compassP1"		A"compass"IT// 4J Added8,	L, L) "compassP2"		A"compass"IT// 4J Added9,	L, L) "compassP3"	AD"compass"TH// 4J Added0,	L, L) "clockP0"
		"clock"N_// 4J Added4,	11,	L, L) "clockP1"
		"clock"N_// 4J Added4,	12,	L, L) "clockP2"
		"clock"N_// 4J Added4,	13,	L, L) "clockP3"
		"clock"N_// 4J Added4,	14,	L, L"dragonFireball"
		ADD_ICON(14,	15,	"record_13")

		ADD_ICON(15"record_cat")
		ADD_ICON(15,"record_blocks")
		ADD_ICON(15,		2"record_chirp")
		ADD_ICON(15,		"record_far")
		ADD_ICON(15,"record_mall")
		ADD_ICON(15,	"record_mellohi")
		ADD_ICON(15,		6,"record_stal")
		ADD_ICON(15,	"record_strad")
		ADD_ICON(15,		"record_ward")
		ADD_ICON(15,		"record_11")
		ADD_ICON(15,	"record_where are we now"ICON(// Special cases)

		
		ClockTexture *dataClock = new ClockTextur"clock"Icon *oldClock = texturesByName[L];
		dataClock->initUVs(oldClock->getU0(), oldClock->getV0(), oldClock->getU1(), oldClock->getV1() "clock"lete oldClock;
		texturesByName[L] = dataClock;

		ClockTexture *clock = new ClockTexture(0, d"clockP0";
		oldClock = texturesByName[L];
		clock->initUVs(oldClock->getU0(), oldClock->getV0(), oldClock->getU1(), oldClock->getV1() "clockP0"te oldClock;
		texturesByName[L] = clock;

		clock = new ClockTexture(1, d"clockP1";
		oldClock = texturesByName[L];
		clock->initUVs(oldClock->getU0(), oldClock->getV0(), oldClock->getU1(), oldClock->getV1() "clockP1"te oldClock;
		texturesByName[L] = clock;

		clock = new ClockTexture(2, d"clockP2";
		oldClock = texturesByName[L];
		clock->initUVs(oldClock->getU0(), oldClock->getV0(), oldClock->getU1(), oldClock->getV1() "clockP2"te oldClock;
		texturesByName[L] = clock;

		clock = new ClockTexture(3, d"clockP3";
		oldClock = texturesByName[L];
		clock->initUVs(oldClock->getU0(), oldClock->getV0(), oldClock->getU1(), oldClock->getV1() "clockP3"te oldClock;
		texturesByName[L] = clock;

		CompassTexture *dataCompass = new CompassTexture("compass" *oldCompass = texturesByName[L];
		dataCompass->initUVs(oldCompass->getU0(), oldCompass->getV0(), oldCompass->getU1(), oldCompass->getV1() );"compass" oldCompass;
		texturesByName[L] = dataCompass;

		CompassTexture *compass = new CompassTexture(0, dataC"compassP0"oldCompass = texturesByName[L];
		compass->initUVs(oldCompass->getU0(), oldCompass->getV0(), oldCompass->getU1(), oldCompass->getV1() );"compassP0"ldCompass;
		texturesByName[L] = compass;

		compass = new CompassTexture(1, dataC"compassP1"oldCompass = texturesByName[L];
		compass->initUVs(oldCompass->getU0(), oldCompass->getV0(), oldCompass->getU1(), oldCompass->getV1() );"compassP1"ldCompass;
		texturesByName[L] = compass;

		compass = new CompassTexture(2, dataC"compassP2"oldCompass = texturesByName[L];
		compass->initUVs(oldCompass->getU0(), oldCompass->getV0(), oldCompass->getU1(), oldCompass->getV1() );"compassP2"ldCompass;
		texturesByName[L] = compass;

		compass = new CompassTexture(3, dataC"compassP3"oldCompass = texturesByName[L];
		compass->initUVs(oldCompass->getU0(), oldCompass->getV0(), oldCompass->getU1(), oldCompass->getV1() );"compassP3"ldCompass;
		texturesByName[L] = compass;
} else {
    float horizRatio = 1.0f / 16.0f;
    float vert "grass_top" f / 32.0f;

                ADD_ICON"grass_top")
		texturesByName[L// 4J added for faster determination of texture type in tesselation"stone"
		ADD_I"dirt"		1,	L)
		ADD_"grass_side"L)
		ADD_ICON("grass_side")
		texturesByName[L// 4J added for faster determination of texture type in tesselation"planks_oak"
		ADD_ICON(0"stoneslab_side")
		ADD_ICON(0,		5"stoneslab_top")
		ADD_ICON(0,		"brick")
		ADD_I"tnt_side"	L)
		ADD_ICON"tnt_top")
		ADD_ICON"tnt_bottom")
		ADD_ICON(0,	"web"L)
		ADD_I"flower_rose")
		ADD_ICON(0,		"flower_dandelion")
		ADD_ICON(0,		13,	L"portal")
		ADD_ICON"sapling"L)
		ADD_ICON("cobblestone")

		ADD_ICON(1,	"bedrock");
		ADD_ICON"sand",	L);
		ADD_I"gravel"2,	L);
		ADD_ICO"log_oak"L);
		ADD_ICON"log_oak_top");
		ADD_ICON(1,	"iron_block");
		ADD_ICON(1,"gold_block");
		ADD_ICON(1,"diamond_block");
		ADD_ICON(1,		8"emerald_block");
		ADD_ICON(1,		9,"redstone_block");
		ADD_ICON(1,		10,	"dropper_front_horizontal"ICON(1,		11,	L"mushroom_red");
		ADD_ICON(1,		12"mushroom_brown");
		ADD_ICON(1,		13,	"sapling_jungle");
		ADD_ICON(1,		14,	"fire_0");
		ADD_ICON("gold_ore");

		ADD_ICON("iron_ore");
		ADD_ICON("coal_ore");
		ADD_ICON("bookshelf");
		ADD_ICON(2"cobblestone_mossy";
		ADD_ICON(2,		4,	L"obsidian");
		ADD_ICON("grass_side_overlay"
		ADD_ICON(2,		6,	L"tallgrass");
		ADD_ICON(2"dispenser_front_vertical"_ICON(2,		8,	L"beacon");
		ADD_ICON"dropper_front_vertical"D_ICON(2,		10,	L"workbench_top");
		ADD_ICON(2,		11,"furnace_front");
		ADD_ICON(2,		12,"furnace_side");
		ADD_ICON(2,		13"dispenser_front");
		ADD_ICON(2,		14,	L"fire_1");
		ADD_ICON("sponge"L);

		ADD_ICO"glass",	L);
		ADD_IC"diamond_ore");
		ADD_ICON(3,	"redstone_ore");
		ADD_ICON(3,		"leaves");
		ADD_ICO"leaves_opaque");
		ADD_ICON(3,		5"stonebrick");
		ADD_ICON(3,"deadbush");
		ADD_ICON("fern"	L);
		ADD_I"daylight_detector_top"ADD_ICON(3,		9,	L"daylight_detector_side"D_ICON(3,		10,	L"workbench_side");
		ADD_ICON(3,		11,	"workbench_front");
		ADD_ICON(3,		12,	L"furnace_front_lit"
		ADD_ICON(3,		13,	L"furnace_top");
		ADD_ICON(3,		1"sapling_spruce");
		ADD_ICON(3,		15,	"wool_colored_white"
		ADD_ICON(4,		0,	L"mob_spawner");
		ADD_ICON(4,	"snow");
		ADD_I"ice",		2,	L);
		ADD_"snow_side"	L);
		ADD_ICON(4"cactus_top");
		ADD_ICON(4,"cactus_side");
		ADD_ICON(4,	"cactus_bottom");
		ADD_ICON(4,		7"clay");
		ADD_I"reeds"	8,	L);
		ADD_ICO"jukebox_side");
		ADD_ICON(4,		10"jukebox_top");
		ADD_ICON(4,		1"waterlily");
		ADD_ICON(4,	"mycel_side");
		ADD_ICON(4,		"mycel_top");
		ADD_ICON(4,	"sapling_birch");
		ADD_ICON(4,		15,"torch_on");

		ADD_ICON("door_wood_upper");
		ADD_ICON(5,		1,	"door_iron_upper");
		ADD_ICON(5,		2,	"ladder");
		ADD_ICO"trapdoor");
		ADD_ICON("iron_bars");
		ADD_ICON(5"farmland_wet");
		ADD_ICON(5,		"farmland_dry");
		ADD_ICON(5,		"crops_0");
		ADD_ICON"crops_1");
		ADD_ICON("crops_2");
		ADD_ICON(5"crops_3");
		ADD_ICON(5"crops_4");
		ADD_ICON(5"crops_5");
		ADD_ICON(5"crops_6");
		ADD_ICON(5"crops_7");
		ADD_ICON(5"lever"L);

		ADD_IC"door_wood_lower");
		ADD_ICON(6,		1,	"door_iron_lower");
		ADD_ICON(6,		2,	"redstone_torch_on";
		ADD_ICON(6,		3,	L"stonebrick_mossy");
		ADD_ICON(6,		4,	L"stonebrick_cracked"
		ADD_ICON(6,		5,	L"pumpkin_top");
		ADD_ICON(6,	"netherrack");
		ADD_ICON(6,"soul_sand");
		ADD_ICON(6"glowstone");
		ADD_ICON(6,"piston_top_sticky"
		ADD_ICON(6,		10,	L"piston_top");
		ADD_ICON(6,		"piston_side");
		ADD_ICON(6,		1"piston_bottom");
		ADD_ICON(6,		13,"piston_inner_top";
		ADD_ICON(6,		14,	L"stem_straight");
		ADD_ICON(6,		15,"rail_normal_turned"
		ADD_ICON(7,		0,	L"wool_colored_black"
		ADD_ICON(7,		1,	L"wool_colored_gray";
		ADD_ICON(7,		2,	L"redstone_torch_off"
		ADD_ICON(7,		3,	L"log_spruce");
		ADD_ICON(7,"log_birch");
		ADD_ICON(7"pumpkin_side");
		ADD_ICON(7,		"pumpkin_face_off");
		ADD_ICON(7,		7,	L"pumpkin_face_on");
		ADD_ICON(7,		8,	"cake_top");
		ADD_ICON(7"cake_side");
		ADD_ICON(7,	"cake_inner");
		ADD_ICON(7,		"cake_bottom");
		ADD_ICON(7,		1"mushroom_block_skin_red"_ICON(7,		13,	L"mushroom_block_skin_brown"CON(7,		14,	L"stem_bent");
		ADD_ICON(7,	"rail_normal");

		ADD_ICON(8,	"wool_colored_red");
		ADD_ICON(8,		1,	L"wool_colored_pink";
		ADD_ICON(8,		2,	L"repeater_off");
		ADD_ICON(8,		"leaves_spruce");
		ADD_ICON(8,		4"leaves_spruce_opaque"	ADD_ICON(8,		5,	L"bed_feet_top");
		ADD_ICON(8,		"bed_head_top");
		ADD_ICON(8,		"melon_side");
		ADD_ICON(8,"melon_top");
		ADD_ICON(8,"cauldron_top");
		ADD_ICON(8,		10"cauldron_inner");
	//ADD_ICON(8,		12,	L"unused"););
		"mushroom_block_skin_stem"ICON(8,		13,	L"mushroom_block_inside"DD_ICON(8,		14,	L"vine");
		ADD_ICO"lapis_block");

		ADD_ICON(9,	"wool_colored_green"
		ADD_ICON(9,		1,	L"wool_colored_lime";
		ADD_ICON(9,		2,	L"repeater_on");
		ADD_ICON(9,	"glass_pane_top");
		ADD_ICON(9,		4,"bed_feet_end");
		ADD_ICON(9,		"bed_feet_side");
		ADD_ICON(9,		6"bed_head_side");
		ADD_ICON(9,		7"bed_head_end");
		ADD_ICON(9,		"log_jungle");
		ADD_ICON(9,	"cauldron_side");
		ADD_ICON(9,		10,"cauldron_bottom");
		ADD_ICON(9,		11,	L"brewing_stand_base"		ADD_ICON(9,		12,	L"brewing_stand");
		ADD_ICON(9,		13,"endframe_top");
		ADD_ICON(9,		14"endframe_side");
		ADD_ICON(9,		15,"lapis_ore");

		ADD_ICON(1"wool_colored_brown"
		ADD_ICON(10,	1,	L"wool_colored_yellow"		ADD_ICON(10,	2,	L"rail_golden");
		ADD_ICON(10,"redstone_dust_cross"		ADD_ICON(10,	4,	L"redstone_dust_line"
		ADD_ICON(10,	5,	L"enchantment_top");
		ADD_ICON(10,	6,	"dragon_egg");
		ADD_ICON(10"cocoa_2");
		ADD_ICON"cocoa_1");
		ADD_ICON("cocoa_0");
		ADD_ICON(1"emerald_ore");
		ADD_ICON(10,	1"trip_wire_source";
		ADD_ICON(10,	12,	L"trip_wire");
		ADD_ICON(10,"endframe_eye");
		ADD_ICON(10,	14"end_stone");
		ADD_ICON(10,"sandstone_top");

		ADD_ICON(11,	0"wool_colored_blue";
		ADD_ICON(11,	1,	L"wool_colored_light_blue"D_ICON(11,	2,	L"rail_golden_powered"		ADD_ICON(11,	3,	L"redstone_dust_cross_overlay"ON(11,	4,	L"redstone_dust_line_overlay"CON(11,	5,	L"enchantment_side");
		ADD_ICON(11,	6,	L"enchantment_bottom"
		ADD_ICON(11,	7,	L"command_block");
		ADD_ICON(11,	8"itemframe_back");
		ADD_ICON(11,	9,	"flower_pot");
		ADD_ICON(11,	"comparator_off");
		ADD_ICON(11,	11,	"comparator_on");
		ADD_ICON(11,	12,"rail_activator");
		ADD_ICON(11,	13,	"rail_activator_powered"D_ICON(11,	14,	L"quartz_ore");
		ADD_ICON(11,	"sandstone_side");

		ADD_ICON(12,	0,"wool_colored_purple"		ADD_ICON(12,	1,	L"wool_colored_magenta"	ADD_ICON(12,	2,	L"detectorRail");
		ADD_ICON(12,	"leaves_jungle");
		ADD_ICON(12,	4"leaves_jungle_opaque"	ADD_ICON(12,	5,	L"planks_spruce");
		ADD_ICON(12,	6"planks_jungle");
		ADD_ICON(12,	7"carrots_stage_0");
		ADD_ICON(12,	8,	"carrots_stage_1");
		ADD_ICON(12,	9,	L"carrots_stage_2");
		ADD_ICON(12,	10,	L"carrots_stage_3");
		//ADD_ICON(12,	12,	L"unused"););
		"water"
		ADD_ICON(12"water_flow");
		ADD_ICON_SIZE(12,14,"sandstone_bottom";

		ADD_ICON(13,	0,	L"wool_colored_cyan";
		ADD_ICON(13,	1,	L"wool_colored_orange"		ADD_ICON(13,	2,	L"redstoneLight");
		ADD_ICON(13,	3"redstoneLight_lit";
		ADD_ICON(13,	4,	L"stonebrick_carved";
		ADD_ICON(13,	5,	L"planks_birch");
		ADD_ICON(13,	"anvil_base");
		ADD_ICON(13"anvil_top_damaged_1"		ADD_ICON(13,	8,	L"quartz_block_chiseled_top"ICON(13,	9,	L"quartz_block_lines_top"D_ICON(13,	10,	L"quartz_block_top";
		ADD_ICON(13,	11,	L"hopper_outside");
		ADD_ICON(13,	12,	"detectorRail_on");
		ADD_ICON(13,	13,	L"nether_brick");

		ADD_ICON(14,	"wool_colored_silver"		ADD_ICON(14,	1,	L"nether_wart_stage_0"		ADD_ICON(14,	2,	L"nether_wart_stage_1"		ADD_ICON(14,	3,	L"nether_wart_stage_2"		ADD_ICON(14,	4,	L"sandstone_carved");
		ADD_ICON(14,	5,	L"sandstone_smooth");
		ADD_ICON(14,	6,	L"anvil_top");
		ADD_ICON(1"anvil_top_damaged_2"		ADD_ICON(14,	8,	L"quartz_block_chiseled"ADD_ICON(14,	9,	L"quartz_block_lines"		ADD_ICON(14,	10,	L"quartz_block_side"
		ADD_ICON(14,	11,	L"hopper_inside");
		ADD_ICON(14,	12,"lava");
		ADD_ICON(1"lava_flow");
		ADD_ICON_SIZE(14,14"destroy_0",2,2);

		ADD_ICON(1"destroy_1");
		ADD_ICON(1"destroy_2");
		ADD_ICON(1"destroy_3");
		ADD_ICON(1"destroy_4");
		ADD_ICON(1"destroy_5");
		ADD_ICON(1"destroy_6");
		ADD_ICON(1"destroy_7");
		ADD_ICON(1"destroy_8");
		ADD_ICON(1"destroy_9");
		ADD_ICON(15"hay_block_side");
		ADD_ICON(15,	10,	"quartz_block_bottom"	ADD_ICON(15,	11,	L"hopper_top");
		ADD_ICON(15,	"hay_block_top");
		ADD_ICON(15,	13,"coal_block");

		ADD_ICON(16"hardened_clay");
		ADD_ICON(16,	1"noteblock"//ADD_ICON(16,	3,	L"unused");//ADD_ICON(16,	4,	L"unused");//ADD_ICON(16,	5,	L"unused");//ADD_ICON(16,	6,	L"unused");//ADD_ICON(16,	7,	L"unused");//ADD_ICON(16,	8,	L"unused");
		"potatoes_stage_0"
		ADD_ICON(16,	9,	L"potatoes_stage_1";
		ADD_ICON(16,	10,	L"potatoes_stage_2";
		ADD_ICON(16,	11,	L"potatoes_stage_3";
		ADD_ICON(16,	12,	L"log_spruce_top");
		ADD_ICON(16,	13,	"log_jungle_top");
		ADD_ICON(16,	14,	"log_birch_top");
		ADD_ICON(16,	15,"hardened_clay_stained_black"ON(17,	0,	L"hardened_clay_stained_blue"CON(17,	1,	L"hardened_clay_stained_brown"ON(17,	2,	L"hardened_clay_stained_cyan"CON(17,	3,	L"hardened_clay_stained_gray"CON(17,	4,	L"hardened_clay_stained_green"ON(17,	5,	L"hardened_clay_stained_light_blue",	6,	L"hardened_clay_stained_lime"CON(17,	7,	L"hardened_clay_stained_magenta"(17,	8,	L"hardened_clay_stained_orange"N(17,	9,	L"hardened_clay_stained_pink"ON(17,	10,	L"hardened_clay_stained_purple"(17,	11,	L"hardened_clay_stained_red"CON(17,	12,	L"hardened_clay_stained_silver"(17,	13,	L"hardened_clay_stained_white"N(17,	14,	L"hardened_clay_stained_yellow"(17,	15,	L"glass_black");

		ADD_ICON(18,"glass_blue");
		ADD_ICON(18"glass_brown");
		ADD_ICON(18,"glass_cyan");
		ADD_ICON(18"glass_gray");
		ADD_ICON(18"glass_green");
		ADD_ICON(18,"glass_light_blue");
		ADD_ICON(18,	6,	L"glass_lime");
		ADD_ICON(18"glass_magenta");
		ADD_ICON(18,	8"glass_orange");
		ADD_ICON(18,	9"glass_pink");
		ADD_ICON(18,	"glass_purple");
		ADD_ICON(18,	11"glass_red");
		ADD_ICON(18,"glass_silver");
		ADD_ICON(18,	13"glass_white");
		ADD_ICON(18,	1"glass_yellow");
		ADD_ICON(18,	15,	"glass_pane_top_black"	ADD_ICON(19,	0,	L"glass_pane_top_blue"		ADD_ICON(19,	1,	L"glass_pane_top_brown"	ADD_ICON(19,	2,	L"glass_pane_top_cyan"		ADD_ICON(19,	3,	L"glass_pane_top_gray"		ADD_ICON(19,	4,	L"glass_pane_top_green"	ADD_ICON(19,	5,	L"glass_pane_top_light_blue"ICON(19,	6,	L"glass_pane_top_lime"		ADD_ICON(19,	7,	L"glass_pane_top_magenta"DD_ICON(19,	8,	L"glass_pane_top_orange"ADD_ICON(19,	9,	L"glass_pane_top_pink"	ADD_ICON(19,	10,	L"glass_pane_top_purple"DD_ICON(19,	11,	L"glass_pane_top_red"		ADD_ICON(19,	12,	L"glass_pane_top_silver"DD_ICON(19,	13,	L"glass_pane_top_white"ADD_ICON(19,	14,	L"glass_pane_top_yellow"DD_ICON(19,	15,	L);
}
}
