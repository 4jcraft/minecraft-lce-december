#include "../Platform/stdafx.h"
#include "../Platform/System.h"
#include "../Headers/net.minecraft.world.entity.h"
#include "../Headers/net.minecraft.world.level.h"
#include "../Headers/net.minecraft.world.level.dimension.h"
#include "../Headers/net.minecraft.world.level.tile.h"
#include "../Headers/net.minecraft.world.phys.h"
#include "../Headers/net.minecraft.world.level.biome.h"
#include "../Headers/net.minecraft.world.entity.monster.h"
#include "Storage/DataLayer.h"
#include "Storage/SparseLightStorage.h"
#include "BlockReplacements.h"
#include "LevelChunk.h"
#include "../Util/BasicTypeContainers.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/Level/ServerLevel.h"
#include "../../Minecraft.Client/Network/ServerChunkCache.h"
#include "../../Minecraft.Client/Rendering/GameRenderer.h"
#include "../Entities/ItemEntity.h"
#include "../Entities/Mobs/Minecart.h"

#ifdef __PS3__
#include "C4JSpursJob.h"
#endif  //__PS3__

#ifdef SHARING_ENABLED
CRITICAL_SECTION LevelChunk::m_csSharing;
#endif
#ifdef _ENTITIES_RW_SECTION
// AP - use a RW critical section so we can have multiple threads reading the
// same data to avoid a clash
CRITICAL_RW_SECTION LevelChunk::m_csEntities;
#else
CRITICAL_SECTION LevelChunk::m_csEntities;
#endif
CRITICAL_SECTION LevelChunk::m_csTileEntities;
bool LevelChunk::touchedSky = false;

void LevelChunk::staticCtor() {
#ifdef SHARING_ENABLED
    InitializeCriticalSection(&m_csSharing);
#endif
#ifdef _ENTITIES_RW_SECTION
    InitializeCriticalRWSection(&m_csEntities);
#else
    InitializeCriticalSection(&m_csEntities);
#endif
    InitializeCriticalSection(&m_csTileEntities);
}

void LevelChunk::init(Level* level, int x, int z) {
    biomes = byteArray(16 * 16);
    for (int i = 0; i < 16 * 16; i++) {
        biomes[i] = 0xff;
    }
#ifdef _ENTITIES_RW_SECTION
    EnterCriticalRWSection(&m_csEntities, true);
#else
    EnterCriticalSection(&m_csEntities);
#endif
    entityBlocks = new std::vector<std::shared_ptr<Entity> >*[ENTITY_BLOCKS#ifdef _ENTITIES_RW_SECTION
    LeaveCriticalRWSection(&m_csEntitie#elseue);

    LeaveCriticalSection(&m_csE#endifs);


    terrainPopulated = 0;
    m_unsaved = false;
    lastSaveHadEntities = false;
    lastSaveTime = 0;
    dontSave = false;
    loaded = false;
    minHeight = 0;
    hasGapsToCheck = false;
    seenByPlayer// 4J Stu - Always true// 4J Stu - Not using this
    checkLightPosit// LIGHT_CHECK_MAX_POS;

    this->level = level;
    this->x = x;
    this->z = z;
    MemSect(1);
    heightmap = byteArray(#ifdef _ENTITIES_RW_SECTION
    EnterCriticalRWSection(&m_csEntitie#elseue);

    EnterCriticalSection(&m_csE#endifs);

    for (int i = 0; i < ENTITY_BLOCKS_LENGTH; i++) {
        entityBlocks[i] =
            new std::vector < std::shared_ptr #ifdef _ENTITIES_RW_SECTION
    LeaveCriticalRWSection(&m #elsetities, true);

    LeaveCriticalSect#endif_csEntities);


    MemSect(0);

lowestHeightmap = 256;
inha  // Optimisation brought forward from 1.8.2, change from int to unsigned
      // char// & this special value changed from -999 to
      // 255
    for (int i = 0; i < 16 * 16; i++) {
        rainHeights[// 4J - lighting change brought forward from 1.8.2, introduced an array of// bools called gapsToRecheck, which are now a single bit in array of nybble// flags in this version
    for (int i = 0; i < 8 * 16; i++) {
                columnFlags  // 4J added - to flag if any emissive tile has been
                             // added to this chunk// (will be cleared when
                             // lighting has been successfully completed for
                             // this// chunk). Defaulting to true as
                             // emissive things can be made and passed in//
                             // the the initialisation block
                             // array.
                    em
#ifdef _LARGE_WORLDS
    m_                                         // 4J Added false;  
                        m_unloaded #endifesTa  // This ctor is used
                                               // for loading a
                                               // save
                                               // into
                    LevelChunk::LevelChunk(Level * level, int x, int z)
                    : ENTITY_BLOCKS_LENGTH(Level::maxBuildHeight / 16) {
                    init(level, x, z);
                    lowerBlocks = new CompressedTileStorage();
                    lowerData = NULL;
                    lowerSkyLight = NULL;
                    lowerBlockLight = NULL;
                    serverTerrainPopulated = NULL;

    if (Level::maxBuildHeight > Level::COMPRESSED_CHUNK_SECTIO// Create all these as empty, as we may not be loading any data into// them
        
        upperBlocks = new CompressedTileStorage(true);
        upperData = new SparseDataStorage(true);
        upperSkyLight = new SparseLightStorage(true, true);
        upperBlockLight = new SparseLightStorage(false, true);
                }
                else {
                    upperBlocks = NULL;
                    upperData = NULL;
                    upperSkyLight = NULL;
                    upperBlock
#ifdef SHARING_ENABLED
    sharingTi #endifData  // 4J - note that since we now compress the block
                          // storage, the parameter blocks// is used as a
                          // source of data, but doesn't get used As the source
                          // data so needs// to be deleted after calling this
                          // ctor.
                    LevelChunk::LevelChunk(Level * level, byteArray blocks,
                                           int x, int z)
                        : ENTITY_BLOCKS_LENGTH(Level::maxBuildHeight / 16) {
                        init  // We'll be creating this as "empty" when this
                              // ctor is called on the client,// as a
                              // result of a chunk becoming visible (but we
                              // don't have the data yet// for it). In this
                              // case, we want to keep memory usage down and so
                              // create// all data as empty/compressed as
                              // possible. On the client we get the full//
                              // data for the chunk as a single update in a
                              // block region update packet,// and so there
                              // is a single point where it is good to compress
                              // the data.
                            bool createEmpty = (blocks.data == NULL);

                        if (createEmpty) {
                            lowerBlocks = new CompressedTileStorage(true);
                            lowerData = new SparseDataStorage(true);

                            lowerSkyLight = new SparseLightStorage(true, true);
                            lowerBlockLight =
                                new SparseLightStorage(false, true);
                        } else {
                            lowerBlocks = new CompressedTileStorage(blocks, 0);
                            lowerData =
                                new SparseData  // 4J - changed to new
                                                // SpareLightStorage class for
                                                // these
                                    lowerSkyLight =
                                        new SparseLightStorage(true);
                            lowerBlockLight =
                                new SparseLightStorag  //    skyLight = new
                                                       //    DataLayer(blocks.length,
                                                       //    level->depthBits);//
                                                       //    blockLight = new
                                                       //    DataLayer(blocks.length,
                                                       //    level->depthBits);

                                if (Level::maxBuildHeight >
                                    Level::COMPRESSED_CHUNK_SECTION_HEIGHT) {
                                if (blocks.length >
                                    Level::COMPRESSED_CHUNK_SECTION_TILES)
                                    upperBlocks = new CompressedTileStorage(
                                        blocks,
                                        Level::COMPRESSED_CHUNK_SECTION_TILES);
                                else
                                    upperBlocks =
                                        new CompressedTileStorage(true);
                                upperData = new SparseDataStorage(true);
                                upperSkyLight =
                                    new SparseLightStorage(true, true);
                                upperBlockLight =
                                    new SparseLightStorage(false, true);
                            }
                            else {
                                upperBlocks = NULL;
                                upperData = NULL;
                                upperSkyLight = NULL;
                                upperBlockLight = NULL;
                            }

                            serverTerra #ifdef SHARING_ENABLED
    sharingTi #endifData        // 4J - this ctor added to be able to make a
                                // levelchunk that shares its// underlying
                                // block data between the server chunk cache &
                                // the multiplayer chunk// cache. The original
                                // version this is shared from owns all the data
                                // that is// shared into this copy, so it isn't
                                // deleted in the dtor.
                                LevelChunk::LevelChunk(Level * level, int x,
                                                       int z, LevelChunk* lc)
                                : ENTITY_BLOCKS_LENGTH(Level::maxBuildHeight /
                                                       16) {
    init// 4J Stu - Copy over the biome data
    memcpy(biomes.data, lc->biomes.da
#ifdef SHARING_ENABLED
    lowerBlocks = lc->lowerBlocks;
    lowerData = lc->lowerData;
    lowerSkyLight = new SparseLightStorage(lc->lowerSkyLight);
    lowerBlockLight = new SparseLightStorage(lc->lowerBlockLight);
    upperBlocks = lc->upperBlocks;
    upperData = lc->upperData;
    upperSkyLight = new SparseLightStorage(lc->upperSkyLight);
    upperBlockLight = new SparseLightStorage(lc->upperBlockLight);

    sharingTilesAndData = true;
    serverTerrainPopulated = &lc#elserainPopulated;

    this->blocks = new CompressedTileStorage(lc->blocks);
    this->data = new SparseDataStorage(lc->data);
    this->skyLight = new SparseLightStorage(lc->skyLight);
    this->blockLight = new SparseLightStorage(lc->blockLight);
    serverTerra#endiflate// 4J Added so we can track unsaved chunks better
void LevelChunk::setUnsa#ifdef _LARGE_WORLDS
    if (m_unsaved != unsaved) {
                                    if (unsaved)
                                        level->incrementUnsavedChunkCount();
                                    else
                                        level->decrementUnsaved #endifount();
    }

    m_unsaved = unsaved;
                            }

                            void LevelChunk::stopShar #ifdef SHARING_ENABLED
    EnterCriticalSection(&m_csSharing);
                            lastUnsharedTime = System::currentTimeMillis();
                            if (!sharingTilesAndData) {
                                LeaveCriticalSection(&m_csSharing);
                                // If we've got a reference to a server chunk's
                                // terrainPopulated flag that// this
                                // LevelChunk is sharing with, then don't
                                // consider unsharing if it// hasn't been
                                // set. This is because post-processing things
                                // that update the// server chunks won't
                                // actually cause the server to send any updates
                                // to the// tiles that they alter, so they
                                // completely depend on the data not
                                // being// shared for it to get from the
                                // server to here
                                if ((serverTerrainPopulated) &&
                                    (((*serverTerrainPopulated) &
                                      sTerrainPopulatedAllAffecting) !=
                                     sTerrainPopulatedAllAffecting)) {
                                    LeaveCriticalSection(&m_csSharing);
                                    // If this is the empty chunk, then it will
                                    // have a x & z of 0,0 - if we// don't
                                    // drop out here we'll end up unsharing the
                                    // chunk at this location for// no
                                    // reason
                                    
    if (isEmpty()) {
                                        LeaveCriticalSection(&m_csSharing);
                                        return;
                                    }

                                    // Changed to used compressed storage -
                                    // these CTORs make deep copies of
                                    // the// storage passed as a
                                    // parameter
                                    lowerBlocks =
                                        new CompressedTileStorage  // Changed to
                                                                   // use new
                                                                   // sparse
                                                                   // data
                                                                   // storage -
                                                                   // this CTOR
                                                                   // makes a
                                                                   // deep copy
                                                                   // of//
                                                                   // the
                                                                   // storage
                                                                   // passed as
                                                                   // a
                                                                   // parameter
                                            lowerData = new SparseDataStorage(
                                                lowerData);

                                    if (Level::maxBuildHeight >
                                        Level::
                                            COMPRESSED_CHUNK_SECTION_HEIGHT) {
                                        upperBlocks = new CompressedTileStorage(
                                            upperBlocks);
                                        upperData =
                                            new SparseDataStorage(upperData);
                                    } else {
                                        upperBlocks = NULL;
                                        upperData /*
                                     newDataLayer =
                                     new
                                     DataLayer(skyLight->data.length*2,
                                     level->depthBits);
                                     XMemCpy(newDataLayer->data.data,
                                     skyLight->data.data,
                                     skyLight->data.length); skyLight
                                     =
                                     newDataLayer;

                                     newDataLayer =
                                     new
                                     DataLayer(blockLight->data.length*2,
                                     level->depthBits);
                                     XMemCpy(newDataLayer->data.data,
                                     blockLight->data.data,
                                     blockLight->data.length); blockLight
                                     =
                                     newDataLayer;
                                     */

    sharingTilesAndData = false;
                                        MemSect(0);
                                        LeaveCriticalSec #endifm_cs  // This is
                                                                     // a slight
                                                                     // variation
                                                                     // on the
                                                                     // normal
                                                                     // start/stop
                                                                     // sharing
                                                                     // methods
                                                                     // here
                                                                     // as// in
                                                                     // general
                                                                     // we
                                                                     // aren't
                                                                     // sharing
                                                                     // lighting
                                                                     // anymore.
                                                                     // This
                                                                     // method
                                                                     // discards
                                                                     // the//
                                                                     // client
                                                                     // lighting
                                                                     // information,
                                                                     // and sets
                                                                     // up new
                                                                     // (non-shared)
                                                                     // lighting
                                                                     // to
                                                                     // match//
                                                                     // the
                                                                     // server.
                                                                     // So
                                                                     // generally
                                                                     // like
                                                                     // stop
                                                                     // sharing,
                                                                     // for the
                                                                     // case
                                                                     // where
                                                                     // we're
                                                                     // already//
                                                                     // not
                                                                     // sharing

void LevelChunk : #ifdef SHARING_ENABLED
    EnterCriticalSection(&m_csSharing);

                                        if (isEmpty()) {
                                            LeaveCriticalSection(&m_csSharing);

#ifdef _LARGE_WORLDS
    LevelChunk* lc = MinecraftServer::getInstance()
                                            ->getLevel(level->dimension->id)
                                                ->cache
                                                ->getChunkLoad #elsenloaded(x,
                                                                            z);
                                            
    LevelChunk* lc = MinecraftServer::getInstance()
                                                ->getLevel(level->dimension->id)
                                                ->cac #endiftChunk(x, z);
                                            

    GameRenderer::AddForDelete(lowerSkyLight);
                                            lowerSkyLight =
                                                new SparseLightStorage(
                                                    lc->lowerSkyLight);
                                            GameRenderer::FinishedReassigning();
                                            GameRenderer::AddForDelete(
                                                lowerBlockLight);
                                            lowerBlockLight =
                                                new SparseLightStorage(
                                                    lc->lowerBlockLight);
                                            GameRenderer::FinishedReassigning();

                                            if (Level::maxBuildHeight >
                                                Level::
                                                    COMPRESSED_CHUNK_SECTION_HEIGHT) {
                                                GameRenderer::AddForDelete(
                                                    upperSkyLight);
                                                upperSkyLight =
                                                    new SparseLightStorage(
                                                        lc->upperSkyLight);
                                                GameRenderer::
                                                    FinishedReassigning();
                                                GameRenderer::AddForDelete(
                                                    upperBlockLight);
                                                upperBlockLight =
                                                    new SparseLightStorage(
                                                        lc->upperBlockLight);
                                                GameRenderer::
                                                    FinishedReassigning();
                                            }
    LeaveCriticalSec#endifm_csSharing);
    

                                        
                                        }

                                        void LevelChunk::
                                            startSharingTilesAnd #ifdef SHARING_ENABLED
    EnterCriticalSection(&m_csSharing);
                                        if (sharingTilesAndData) {
                                            LeaveCriticalSection(&m_csSharing);
                                            // If this is the empty chunk, then
                                            // it will have a x & z of 0,0 -
                                            // we'll end// up potentially
                                            // loading the 0,0 block if we
                                            // proceed. And it obviously//
                                            // doesn't make sense to go
                                            // resharing the 0,0 block on behalf
                                            // of an empty// chunk either
                                            
    if (isEmpty()) {
                                                LeaveCriticalSection(
                                                    &m_csSharing);

#ifdef _LARGE_WORLDS
    LevelChunk* lc = MinecraftServer::getInstance()
                                                ->getLevel(level->dimension->id)
                                                    ->cache
                                                    ->getChunkLoad #elsenloaded(
                                                        x, z);

    LevelChunk* lc = MinecraftServer::getInstance()
                         ->getLevel(level->dimension->id)
                         ->cac#endiftChunk// In normal usage, chunks should only reshare if their local data matched// that on the server. The forceMs parameter though can be used to force a// share if resharing hasn't happened after a period of time
    if (forc// Normal behaviour - just check that the data matches, and don't start// sharing data if it doesn't (yet)
        if (!lowerBlocks->isSameAs(lc->lowerBlocks) ||
            (upperBlocks && lc->upperBlocks &&
             !upperBlocks->isSameAs(lc->upperBlocks))) {
                                                    LeaveCriticalSection(
                                                        &m_csSharing);
                                                    return;
        }
 // Only force if it has been more than forceMs milliseconds since we// last wanted to unshare this chunk
        __int64 timenow = System::currentTimeMillis();
        if ((timenow - lastUnsharedTime) < forceMs) {
                                                    LeaveCriticalSection(
                                                        &m_csSharing);
                                                    return;
                                                    // Note - data that was
                                                    // shared isn't directly
                                                    // deleted here, as it
                                                    // might// still be in
                                                    // use in the game render
                                                    // update thread. Let that
                                                    // thread delete// it
                                                    // when it is safe to do so
                                                    // instead.
                                                    GameRenderer::AddForDelete(
                                                        lowerBlocks);
                                                    lowerBlocks =
                                                        lc->lowerBlocks;
                                                    GameRenderer::
                                                        FinishedReassigning();

                                                    GameRenderer::AddForDelete(
                                                        lowerData);
                                                    lowerData = lc->lowerData;
                                                    GameRenderer::
                                                        FinishedReassigning();

                                                    if (Level::maxBuildHeight >
                                                        Level::
                                                            COMPRESSED_CHUNK_SECTION_HEIGHT) {
                                                        GameRenderer::
                                                            AddForDelete(
                                                                upperBlocks);
                                                        upperBlocks =
                                                            lc->upperBlocks;
                                                        GameRenderer::
                                                            FinishedReassigning();

                                                        GameRenderer::
                                                            AddForDelete(
                                                                upperData);
                                                        upperData =
                                                            lc->upperData;
                                                        GameRenderer::
                                                            FinishedReassigning();
                                                    }

                                                    sharingTilesAndData = true;
    LeaveCriticalSec#endifm_csSharing);
    
}

LevelChu#ifdef SHARING_ENABLED
    if (!s#endifTilesAndData)

    {
                                                    delete lowerData;
                                                    delete lowerBlocks;
                                                    if (upperData)
                                                        delete upperData;
                                                    if (upperBlocks)
                                                        delete upperBlocks;
    }

    delete lowerSkyLight;
    delete lowerBlockLight;
    if (upperSkyLight) delete upperSkyLight;
    if (upperBlockLight) delete upperBlockLight;

    delete[] heightmap.data;

    for (int i = 0; i < ENTITY_BLOCKS_LENGTH; ++i) delete entityBlocks[i];
    delete[] entityBlocks;

    de
#ifdef _LARGE_WORLDS
    delete m_u #endifdEntitiesTag;


                                            
                                            }

                                            bool LevelChunk::isAt(int x,
                                                                  int z) {
                                                return x == this->x &&
                                                       z == this->z;
                                            }

                                            int LevelChunk::getHeightmap(
                                                int x, int z) {
                                                return heightmap[z << 4 | x] &
                                                       0xff;
                                            }

                                            int LevelChunk::
                                                getHighestSectionPosition() {
                                                return Level::maxBu  // 4J Stu -
                                                                     // Unused
                                                                     // //
                                                                     // for (int
                                                                     // i =
                                                                     // sections.length
                                                                     // - 1; i
                                                                     // >= 0;
                                                                     // i--)
                                                                     // {//
                                                                     // if
                                                                     // (sections[i]
                                                                     // != null)
                                                                     // { // &&
                                                                     // !sections[i].isEmpty())
                                                                     // {//
                                                                     // return
                                                                     // sections[i].getYPosition();//
                                                                     // }//}
                                                    // return 0;
    

                                            
                                            }

                                            void
                                            LevelChunk::recalcBlockLights() {}

                                            void LevelChunk::
                                                reca #ifdef __PSVITA__ {
                                                    // AP - lets fetch ALL the chunk data at the same time for a good speed up
                                                    byteArray blockData =
                                                        byteArray(
                                                            Level::
                                                                CHUNK_TILE_COUNT);
                                                getBl #endifa(blockData);
                                                

    int min = Level::maxBuildHeight - 1;
                                                for (int x = 0; x < 16; x++)
                                                    for (int z = 0; z < 16;
                                                         z++) {
                                                        rainHeights[x +
                                                                    (z << 4)] =
                                                            // 4J - changed from
                                                            // int to unsigned
                                                            // char & this
                                                            // special
                                                            // value changed
                                                            // from -999 to
                                                            // 255

                                                            int y = Level::
                                                                maxBuild  //            int p = x << level->depthBitsPlusFour | z <<//            level->depthBits;		// 4J - removed#ifdef __PSVITA__

            int Index = (x << 11) + (z << 7);
                                                        int offset = Level::
                                                            COMPRESSED_CHUNK_SECTION_TILES;
                                                        y = 127;
            while (y > 0 &&
                   Tile::lightBlock[blockData[Index + offset + (y - 1)]] ==
       // 4J - was blocks->get() was blocks[p + y - 1]
            {
                                                            y--;
            }
            if (y == 0) {
                                                            offset = 0;
                                                            y = 127;
                while (y > 0 &&
                       Tile::lightBlock[blockData[Index + offset + (y - 1)]] ==
           // 4J - was blocks->get() was blocks[p + y - 1]
                {
                                                                y--;
                }
            } else {
                                                            y += #else
            }

            CompressedTileStorage* blocks =
                (y - 1) >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT ? upperBlocks
                                                                  : lowerBlocks;
            while (
                y > 0 &&
                Tile::lightBlock[blocks->get(
                                     x,
                                     (y - 1) %
                                         Level::COMPRESSED_CHUNK_SECTION_HEIGHT,
                                     z) &
                                 0xff] ==
    // 4J - was blocks->get() was blocks[p + y - 1]
            {
                                                            y--;
                                                            blocks =
                                                                (y - 1) >=
                                                                        Level::
                                                                            COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                    ? upperBlocks
                                                                    : lowerBl #endif            }

            heightmap[z << 4 | x] = (uint8_t)y;
            if (y < min) min = y;
                                                    }

                                                this->minHeight = min;
                                                this
#ifdef __PSVITA__;

    del #endifockData.data;
                                                

                                            
                                            }

                                            void LevelChunk::#ifdef __PSVITA__ {
                                                    // AP - lets fetch ALL the chunk data at the same time for a good speed up
                                                    byteArray blockData =
                                                        byteArray(
                                                            Level::
                                                                CHUNK_TILE_COUNT);
                                                getBl #endifa(blockData);
                                                
    lowestHeightmap = Integer::MAX_VALUE;

                                                int min =
                                                    Level::maxBuildHeight - 1;
                                                for (int x = 0; x < 16; x++)
                                                    for (int z = 0; z < 16;
                                                         z++) {
                                                        int y =
                                                            Level::maxBuild  //            int p = x << level->depthBitsPlusFour | z <<//            level->depthBits;			// 4J - removed
#ifdef __PSVITA__

            int Index = (x << 11) + (z << 7);
                                                            int offset = Level::
                                                                COMPRESSED_CHUNK_SECTION_TILES;
                                                        y = 127;
            while (y > 0 &&
                   Tile::lightBlock[blockData[Index + offset + (y - 1)]] ==
       // 4J - was blocks->get() was blocks[p + y - 1]
            {
                                                            y--;
            }
            if (y == 0) {
                                                            offset = 0;
                                                            y = 127;
                while (y > 0 &&
                       Tile::lightBlock[blockData[Index + offset + (y - 1)]] ==
           // 4J - was blocks->get() was blocks[p + y - 1]
                {
                                                                y--;
                }
            } else {
                                                            y += #else
            }

            CompressedTileStorage* blocks =
                (y - 1) >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT ? upperBlocks
                                                                  : lowerBlocks;
            while (
                y > 0 &&
                Tile::lightBlock[blocks->get(
                                     x,
                                     (y - 1) %
                                         Level::COMPRESSED_CHUNK_SECTION_HEIGHT,
                                     z) &
                                 0xff] ==
    // 4J - was blocks->get() was blocks[p + y - 1]
            {
                                                            y--;
                                                            blocks =
                                                                (y - 1) >=
                                                                        Level::
                                                                            COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                    ? upperBlocks
                                                                    : lowerBl #endif            }

            heightmap[z << 4 | x] = (uint8_t)y;
            if (y < min) min = y;
            if (y < lowestHeightmap) lowestHeightmap = y;

            if (!level->dimension->hasCeiling) {
                                                            int br = Level::
                                                                MAX_BRIGHTNESS;
                                                            int yy = Level::
#ifdef __PSVITA__1;
                                                            
                int offset = Level::COMPRESSED_CHUNK_SECTION_TILES;
                                                            SparseLightStorage*
                                                                skyLight =
                                                                    upperSkyLight;
                                                            yy = 127;
                                                            do {
                    br -=
                        Tile::lightBlock[blockData[Index + offset +
                                      // 4J - blocks->get()
                                      // was blocks[p + yy]
                    if (br > 0) {
                                                                    skyLight
                                                                        ->set(
                                                                            x,
                                                                            yy,
                                                                            z,
                                                                            br);
                    }
                    yy--;
                                                            } while (yy > 0 &&
                                                                     br > 0);

                                                            if (yy == 0 &&
                                                                br > 0) {
                                                                offset = 0;
                                                                skyLight =
                                                                    lowerSkyLight;
                                                                yy = 127;
                                                                do {
                        br -= Tile::lightBlock
                            [blockData[Index + offset +
                          // 4J - blocks->get() was blocks[p
                          // + yy]            
                        if (br > 0) {
                                                                        skyLight->set(
                                                                            x,
                                                                            yy,
                                                                            z,
                                                                            br);
                        }
                        yy--;
                                                                } while (
                                                                    yy > 0 &&
                                                                    br >
                                                                        0) #else
                                                            }
                                                            
                CompressedTileStorage* blocks = yy >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                ? upperBlocks
                                                                : lowerBlocks;
                                                            SparseLightStorage*
                                                                skyLight =
                                                                    yy >= Level::
                                                                                COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                        ? upperSkyLight
                                                                        : lowerSkyLight;
                                                            do {
                    br -= Tile::lightBlock
                        [blocks->get(
                             x, (yy % Level::COMPRESSED_CHUNK_SECTION_HEIGHT),
                             z) &
             // 4J - blocks->get() was blocks[p + yy]
                    if (br > 0) {
                                                                    skyLight->set(
                                                                        x,
                                                                        (yy %
                                                                         Level::
                                                                             COMPRESSED_CHUNK_SECTION_HEIGHT),
                                                                        z, br);
                    }
                    yy--;
                    blocks = yy >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                 ? upperBlocks
                                 : lowerBlocks;
                    skyLight = yy >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                   ? upperSkyLight
                                   : lowerSkyLight;
                } while #endif0 && br > 0);
                
            }
                                                    }

                                                this->minHeight = min;

                                                for (int x = 0; x < 16; x++)
                                                    for (int z = 0; z < 16;
                                                         z++) {
                                                        lightGaps(x, z);
                                                    }

                                                this
#ifdef __PSVITA__;

    del #endifockD  // 4J - this code is fully commented out in the java
                    // version, but we have// reimplemented something here to
                    // try and light lava as chunks are created, as// otherwise
                    // they get shared before being lit, and then their lighting
                    // gets// updated on the client and causes framerate
                    // stutters.
                                                    void
                                                    LevelChunk::lightLava() {
                                                    if (!emissiveAdded) return;

                                                    for (int x = 0; x < 16; x++)
        for (int z = 0; z < 16;//			int p = x << 11 | z << 7;//// 4J - removed   
            int ymax = getHeightmap(x, z);
            for (int y = 0; y < Level::COMPRESSED_CHUNK_SECTION_HEIGHT; y++) {
                                                            CompressedTileStorage*
                                                                blocks =
                                                                    lowerBlocks;
                int emit = Tile::lightEmission[blocks->get(
            // 4J - blocks->get() was blocks[p + y]
                if (emit > 0) {
                                                                //					printf("(%d,%d,%d)",this->x
                                                                //* 16 + x, y,
                                                                // this->z * 16
                                                                // +
                                                                // z);
                                                                // We'll be
                                                                // calling this
                                                                // function for
                                                                // a lot of
                                                                // chunks
                                                                // as
                                                                // they are
                                                                // post-processed.
                                                                // For every
                                                                // chunk that
                                                                // is
                                                                // post-processed
                                                                // we're calling
                                                                // this for each
                                                                // of
                                                                // its
                                                                // neighbours in
                                                                // case some
                                                                // post-processing
                                                                // also
                                                                // created
                                                                // something
                                                                // that needed
                                                                // lighting
                                                                // outside the
                                                                // starting
                                                                // chunk.
                                                                // Because of
                                                                // this, do a
                                                                // quick test on
                                                                // any
                                                                // emissive
                                                                // blocks that
                                                                // have been
                                                                // added to see
                                                                // if checkLight
                                                                // has
                                                                // already been
                                                                // run on this
                                                                // particular
                                                                // block - this
                                                                // is
                                                                // straightforward
                                                                // to check as
                                                                // being
                                                                // emissive
                                                                // blocks
                                                                // they'll
                                                                // have their
                                                                // block
                                                                // brightness
                                                                // set to their
                                                                // lightEmission
                                                                // level in this
                                                                // case.
                                                                if (getBrightness(
                                                                        LightLayer::
                                                                            Block,
                                                                        x, y,
                                                                        z) <
                                                                    emit) {
                                                                    level->checkLight(
                                                                        LightLayer::
                                                                            Block,
                                                                        this->x *
                                                                                16 +
                                                                            x,
                                                                        y,
                                                                        this->z *
                                                                                16 +
                                                                            z,
                                                                        true);
                                                                }
                }
            }
                                                }
                                                emissiveAdded = false;
                                            }

void LevelChunk::lightGaps(// 4J - lighting change brought forward from 1.8.2, introduced an array of// bools called gapsToRecheck, which are now a single bit in array of// nybbles in this version
    int slot = (x >> 1) | (z * 8);
    int shift = (x & 1) * 4;
    columnFlags[slot] |= (eColumnFlag_recheck << shift);
    hasGapsToCheck = true;
                                        }
void LevelChunk::recheckGaps// 4J added - otherwise we can end up doing a very broken kind of lighting// since for an empty chunk, the heightmap is all zero, but it still has an// x and z of 0 which means that the level->getHeightmap references in here// find a real chunk near the origin, and then attempt to light massive gaps// between the height of 0 and whatever heights are in those.
    if (isEm// 4J addedrn;

    
    int minXZ = -(level->dimension->getXZSize() * 16) / 2;
    int maxXZ = (level->dimension->getXZSize() // 4J - note - this test will currently return true for chunks at the edge// of our world. Making further checks inside the loop now to address this// issue.
    
    if (level->hasChunksAt(x * 16 + 8, Level::maxBuildHeight / 2, z * 16 + 8,
                           16)) {
                                            for (int x = 0; x < 16; x++)
                                                for (int z = 0; z < 16; z++) {
                                                    int slot =
                                                        (x >> 1) | (z * 8);
                                                    int shift = (x & 1) * 4;
                                                    if (bForce ||
                                                        (columnFlags[slot] &
                                                         (eColumnFlag_recheck
                                                          << shift))) {
                                                        columnFlags[slot] &= ~(
                                                            eColumnFlag_recheck
                                                            << shift);
                                                        int height =
                                                            getHeightmap(x, z);
                                                        int xOffs =
                                                            (this->x * 16) + x;
                                                        int zOffs =
                                                            (this->z * 16) + z;

                                                        // 4J - rewritten this
                                                        // to make sure that the
                                                        // minimum
                                                        // neighbour height
                                                        // which is calculated
                                                        // doesn't
                                                        // involve
                                                        // getting any heights
                                                        // from beyond the edge
                                                        // of the
                                                        // world,
                                                        // which can lead to
                                                        // large, very
                                                        // expensive,
                                                        // non-existent
                                                        // cliff edges to be
                                                        // lit
                                                        int nmin =
                                                            level->getHeightmap(
                                                                xOffs, zOffs);
                                                        if (xOffs - 1 >=
                                                            minXZ) {
                                                            int n =
                                                                level
                                                                    ->getHeightmap(
                                                                        xOffs -
                                                                            1,
                                                                        zOffs);
                                                            if (n < nmin)
                                                                nmin = n;
                                                        }
                                                        if (xOffs + 1 <=
                                                            maxXZ) {
                                                            int n =
                                                                level
                                                                    ->getHeightmap(
                                                                        xOffs +
                                                                            1,
                                                                        zOffs);
                                                            if (n < nmin)
                                                                nmin = n;
                                                        }
                                                        if (zOffs - 1 >=
                                                            minXZ) {
                                                            int n =
                                                                level
                                                                    ->getHeightmap(
                                                                        xOffs,
                                                                        zOffs -
                                                                            1);
                                                            if (n < nmin)
                                                                nmin = n;
                                                        }
                                                        if (zOffs + 1 <=
                                                            maxXZ) {
                                                            int n =
                                                                level
                                                                    ->getHeightmap(
                                                                        xOffs,
                                                                        zOffs +
                                                                            1);
                                                            if (n < nmin)
                                                                nmin = n;
                                                        }
                                                        lightGap(xOffs, zOffs,
                                                                 nmin);

                                                        // 4J - if doing a full
                                                        // forced thing
                                                        // over
                                                        // every single column,
                                                        // we don't need to
                                                        // do
                                                        // these offset checks
                                                        // too
                                                        {
                                                            if (xOffs - 1 >=
                                                                minXZ)
                                                                lightGap(
                                                                    xOffs - 1,
                                                                    zOffs,
                                                                    height);
                                                            if (xOffs + 1 <=
                                                                maxXZ)
                                                                lightGap(
                                                                    xOffs + 1,
                                                                    zOffs,
                                                                    height);
                                                            if (zOffs - 1 >=
                                                                minXZ)
                                                                lightGap(
                                                                    xOffs,
                                                                    zOffs - 1,
                                                                    height);
                                                            if (zOffs + 1 <=
                                                                maxXZ)
                                                                lightGap(
                                                                    xOffs,
                                                                    zOffs + 1,
                                                                    height);
                                                        }
                                                        hasGapsToCheck = false;
                                                    }
                                                }
    }
                                    }

                                    void LevelChunk::lightGap(int x, int z,
                                                              int source) {
                                        int height = level->getHeightmap(x, z);

                                        if (height > source) {
                                            lightGap(x, z, source, height + 1);
                                        } else if (height < source) {
                                            lightGap(x, z, height, source + 1);
                                        }
                                    }

                                    void LevelChunk::lightGap(int x, int z,
                                                              int y1, int y2) {
                                        if (y2 > y1) {
                                            if (level->hasChunksAt(
                                                    x,
                                                    Level::maxBuildHeight / 2,
                                                    z, 16)) {
                                                for (int y = y1; y < y2; y++) {
                                                    level->checkLight(
                                                        LightLayer::Sky, x, y,
                                                        z);
                                                }
                                                this->setUnsaved(true);
                                            }
                                        }
                                    }

                                    void LevelChunk::recalcHeight(
                                        int x, int yStart, int z) {
                                        int yOld = heightmap[z << 4 | x] & 0xff;
                                        int y = yOld;
    if (yStart > yOl//    int p = x << level->depthBitsPlusFour | z << level->depthBits;//    // 4J - removed

    CompressedTileStorage* blocks =
        (y - 1) >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT ? upperBlocks
                                                          : lowerBlocks;
    while (
        y > 0 &&
        Tile::lightBlock
                [blocks->get(
                     x, (y - 1) % Level::COMPRESSED_CHUNK_SECTION_HEIGHT, z) &
          // 4J - blocks->get() was blocks[p + y - 1]
    {
                                            y--;
                                            blocks =
                                                (y - 1) >=
                                                        Level::
                                                            COMPRESSED_CHUNK_SECTION_HEIGHT
                                                    ? upperBlocks
                                                    : lowerBlocks;
    }
    if (y ==//    level->lightColumnChanged(x, z, y, yOld);		// 4J - this//    call moved below & corrected - see comment further down
    heightmap[z << 4 | x] = (uint8_t)y;

    if (y < minHeight) {
                                            minHeight = y;
    } else {
                                            int min = Level::maxBuildHeight - 1;
                                            for (int _x = 0; _x < 16; _x++)
                                                for (int _z = 0; _z < 16;
                                                     _z++) {
                                                    if ((heightmap[_z << 4 |
                                                                   _x] &
                                                         0xff) < min)
                                                        min =
                                                            (heightmap[_z << 4 |
                                                                       _x] &
                                                             0xff);
                                                }
                                            this->minHeight = min;
    }

    int xOffs = (this->x * 16) + x;
    int zOffs = (this->z * 16) + z;
    if (!level->dimension->hasCeiling) {
                                            if (y < yOld) {
                                                SparseLightStorage* skyLight =
                                                    y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                        ? upperSkyLight
                                                        : lowerSkyLight;
                                                for (int yy = y; yy < yOld;
                                                     yy++) {
                                                    skyLight =
                                                        yy >= Level::
                                                                    COMPRESSED_CHUNK_SECTION_HEIGHT
                                                            ? upperSkyLight
                                                            : lowerSkyLight;
                                                    skyLight->set(
                                                        x,
                                                        (yy %
                                                         Level::
                                                             COMPRESSED_CHUNK_SECTION_HEIGHT),
                                                        z, 15);
                                                }
                                            }  // 4J - lighting change brought
                                               // forward
                                               // from 1.8.2//
                                               // level->updateLight(LightLayer::Sky,
                                               // xOffs, yOld,
                                               // zOffs,// xOffs,
                                               // y, zOffs);
                                            SparseLightStorage* skyLight =
                                                y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                    ? upperSkyLight
                                                    : lowerSkyLight;
                                            for (int yy = yOld; yy < y; yy++) {
                                                skyLight =
                                                    yy >= Level::
                                                                COMPRESSED_CHUNK_SECTION_HEIGHT
                                                        ? upperSkyLight
                                                        : lowerSkyLight;
                                                skyLight->set(
                                                    x,
                                                    (yy %
                                                     Level::
                                                         COMPRESSED_CHUNK_SECTION_HEIGHT),
                                                    z, 0);
                                            }
        }

        int br = 15;

        SparseLightStorage* skyLight =
            y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT ? upperSkyLight
                                                        : lowerSkyLight;
        while (y > 0 && br > 0) {
                                            y--;
                                            skyLight =
                                                y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                    ? upperSkyLight
                                                    : lowerSkyLight;
                                            int block =
                                                Tile::lightBlock[getTile(x, y,
                                                                         z)];
                                            if (block == 0) block = 1;
                                            br -= block;
                                            if (br < 0) br = 0;
            skyLight->set(x, (y % Level::COMPRESSED_CHUNK_SECTION_HEIGHT), z,
                       // level.updateLightIfOtherThan(LightLayer.Sky, xOffs, y, zOffs,// -1);
            
// 4J - changed to use xOffs and zOffs rather than the (incorrect) x and z// it used to, and also moved so that it happens after all the lighting// should be done by this stage, as this will trigger our asynchronous// render updates immediately (potentially) so don't want to say that the// lighting is done & then do it
    level->lightColumnChanged(xOffs, zO// 4J -  lighting changes brought forward from 1.8.2
    int height = heightmap[z << 4 | x];
    int y1 = yOld;
    int y2 = height;
    if (y2 < y1) {
                                                int tmp = y1;
                                                y1 = y2;
                                                y2 = tmp;
    }
    if (height < lowestHeightmap) lowestHeightmap = height;
    if (!level->dimension->hasCeiling) {
                                                PI "Light gaps" vent(
                                                    0, );
                                                lightGap(xOffs - 1, zOffs, y1,
                                                         y2);
                                                lightGap(xOffs + 1, zOffs, y1,
                                                         y2);
                                                lightGap(xOffs, zOffs - 1, y1,
                                                         y2);
                                                lightGap(xOffs, zOffs + 1, y1,
                                                         y2);
                                                lightGap(xOffs, zOffs, y1, y2);
                                                PIXEndNamedEvent();
    }

    this->s/**
 * The purpose of this method is to allow the EmptyLevelChunk to be all air
 * but still block light. See EmptyLevelChunk.java
 *
 * @param x
 * @param y
 * @param z
 * @return
 */
int LevelChunk::getTileLightBlock(int x, int y, int z) {
                                                return Tile::lightBlock[getTile(
                                                    x, y, z)];
}

int LevelChunk::getTile(int x, int y, int z) {
                                                CompressedTileStorage* blocks =
                                                    y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                        ? upperBlocks
                                                        : lowerBlocks;
                                                return blocks->get(
                                                    x,
                                                    y % Level::
                                                            COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                    z);
}

bool LevelChunk::setTileAndData(int x, int y, int z, int _tile, int _data) {
                                                uint8_t
                                                    tile  // Optimisation
                                                          // brought forward
                                                          // from 1.8.2, change
                                                          // from int to
                                                          // unsigned
                                                          // char// & this
                                                          // special value
                                                          // changed from -999
                                                          // to
                                                          // 255
                                                    int slot = z << 4 | x;

                                                if (y >=
                                                    ((int)rainHeights[slot]) -
                                                        1) {
                                                    rainHeights[slot] = 255;
                                                }

                                                int oldHeight =
                                                    heightmap[slot] & 0xff;

                                                CompressedTileStorage* blocks =
                                                    y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                        ? upperBlocks
                                                        : lowerBlocks;
                                                SparseDataStorage* data =
                                                    y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                        ? upperData
                                                        : lowerData;
                                                int old = blocks->get(
                                                    x,
                                                    y % Level::
                                                            COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                    z);
                                                int oldData = data->get(
                                                    x,
                                                    y % Level::
                                                            COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                    z);
    if (old == _tile && oldData // 4J Stu - Need to do this here otherwise double chests don't always// work correctly   
        std::shared_ptr<TileEntity> te = getTileEntity(x, y, z);
        if (te != NULL) {
                                                    te->clearCache();
        }

        return false;
    }
    int xOffs = this->x * 16 + x;
    int zOffs = this->z * 16 + z;
    if (old != 0 && !level->isClientSide) {
                                                Tile::tiles[old]->onRemoving(
                                                    level, xOffs, y, zOffs,
                                                    oldData);
    }
 "Chunk setting tile"t(0, );
    blocks->set(x, y % Level::COMPRESSED_CHUNK_SECTION_HEIGHT, z, tile);
    PIXEndNamedEvent();
    if (old != 0) {
                                                if (!level->isClientSide) {
                                                    Tile::tiles[old]->onRemove(
                                                        level, xOffs, y, zOffs,
                                                        old, oldData);
                                                } else if (
                                                    Tile::tiles[old]
                                                        ->isEntityTile() &&
                                                    old != _tile) {
                                                    level->removeTileEntity(
                                                        xOffs, y, zOffs);
                                                }
    }
 "Chunk setting data"t(0, );
    data->set(x, y % Level::COMPRESSED_CHUNK_SECTION_HEIGHT, z, _data);
    // 4J added - flag if something emissive is being added. This is used during// level creation to determine what chunks need extra lighting processing
    if (Tile::lightEmission[tile & 0xff] > 0) {
                                                emissiveAdded = true;
    }

 "Updating lighting"nt(0, // 4J - There isn't any point in recalculating heights or updating sky// lighting if this tile has the same light-blocking capabilities as the one// it is replacing
    
    if (Tile::lightBlock[tile & 0xff] != Tile::lightBlock[old & 0xff]) {
                                                if (!level->dimension
                                                         ->hasCeiling) {
                                                    if (Tile::lightBlock
                                                            [tile & 0xff] !=
                                                        0) {
                                                        if (y >= oldHeight) {
                                                            "Recalc height "
                                                            "1" vent(
                                                                0, );
                                                            recalcHeight(
                                                                x, y + 1, z);
                                                            PIXEndNamedEvent();
                                                        }
                                                    } else {
                                                        if (y ==
                                                            oldHeight - 1) {
                                                            "Recalc height "
                                                            "2" vent(
                                                                0, );
                                                            recalcHeight(x, y,
                                                                         z);
                                                            PIXEndNamedEvent();
                                                        }
                                                        // level.updateLight(LightLayer.Carried,
                                                        // xOffs, y, zOffs,
                                                        // xOffs, y,//
                                                        // zOffs);
                                                        
     "Lighting gaps" dEvent(0, );
                                                        lightGaps(x, z);
                                                        PIXEndNamedEvent();
                                                    }
                                                    PIXEndNamedEvent();
                                                    data->set(
                                                        x,
                                                        y % Level::
                                                                COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                        z, _data);
                                                    if (_tile != 0) {
                                                        if (!level
                                                                 ->isClientSide) {
                                                            Tile::tiles[_tile]
                                                                ->onPlace(
                                                                    level,
                                                                    xOffs, y,
                                                                    zOffs);
                                                            // 4J - in general
                                                            // we don't want to
                                                            // run the onPlace
                                                            // method on
                                                            // the//
                                                            // client, but do a
                                                            // specific bit of
                                                            // the fireTile
                                                            // onPlace code
                                                            // here,//
                                                            // otherwise we'll
                                                            // place fire on the
                                                            // client and if it
                                                            // isn't
                                                            // a//
                                                            // suitable location
                                                            // then we have to
                                                            // wait a few frames
                                                            // before
                                                            // the//
                                                            // server updates us
                                                            // to say it wasn't
                                                            // right. In the
                                                            // meantime,
                                                            // the//
                                                            // client will have
                                                            // done some local
                                                            // lighting etc. and
                                                            // we can end
                                                            // up//
                                                            // with errors when
                                                            // the update from
                                                            // the server comes
                                                            // in.
                                                            if (_tile ==
                                                                Tile::fire_Id) {
                                                                if (!Tile::tiles[_tile]
                                                                         ->mayPlace(
                                                                             level,
                                                                             xOffs,
                                                                             y,
                                                                             zOffs)) {
                    blocks->set(x, y % Level::COMPRESSED_CHUNK_SECTION_HEIGHT,
                                z,//					blocks[x <<       //level->depthBitsPlusFour | z << level->depthBits | y] = 0;
                                                                }
                                                                // AP - changed
                                                                // the method of
                                                                // EntityTile
                                                                // detection cos
                                                                // it's well
                                                                // slow
                                                                // on//
                                                                // Vita mate
                                                                //		if
                                                                //(_tile > 0 &&
                                                                // dynamic_cast<EntityTile//*>(Tile::tiles[_tile])
                                                                //!=
                                                                // NULL)
                                                                if (_tile > 0 &&
                                                                    Tile::tiles
                                                                            [_tile] !=
                                                                        NULL &&
                                                                    Tile::tiles[_tile]
                                                                        ->isEntityTile()) {
                                                                    std::shared_ptr<
                                                                        TileEntity>
                                                                        te = getTileEntity(
                                                                            x,
                                                                            y,
                                                                            z);
                                                                    if (te ==
                                                                        NULL) {
                                                                        te =
                                                                            dynamic_cast<
                                                                                EntityTile*>(
                                                                                Tile::tiles
                                                                                    [_tile])
                                                                                ->newTileE  // app.DebugPrintf("%s: Setting tile id %d, created tileEntity// type %d\n", level->isClientSide?"Client":"Server", _tile,// te->GetType());            
                                                                                    level
                                                                                ->setTileEntity(
                                                                                    xOffs,
                                                                                    y,
                                                                                    zOffs,
                                                                                    te);
                                                                    }
            if (// app.DebugPrintf("%s: Setting tile id %d, found tileEntity// type %d\n", level->isClientSide?"Client":"Server", _tile,// te->GetType());            
                te->clearCache();
    // AP - changed the method of EntityTile detection cos it's well slow on// Vita mate//	else if (old > 0 && dynamic_cast<EntityTile *>(Tile::tiles[old]) !=//NULL)
    
    else if (old > 0 && Tile::tiles[_tile] != NULL &&
             Tile::tiles[_tile]->isEntityTile()) {
                                                                        std::shared_ptr<
                                                                            TileEntity>
                                                                            te = getTileEntity(
                                                                                x,
                                                                                y,
                                                                                z);
                                                                        if (te !=
                                                                            NULL) {
                                                                            te->clearCache();
                                                                        }
    }

    this->setUnsaved(true);
    return true;
                                                                }

bool LevelChunk::setTile(int// 4J Stu - Now using setTileAndData (like in 1.5 Java) so there is only one// place we have to fix things
    
    return setTileAndData(x, y, z, _tile, 0);
                                                            }

                                                            int
                                                            LevelChunk::getData(
                                                                int x, int y,
                                                                int z) {
                                                                SparseDataStorage* data =
                                                                    y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                        ? upperData
                                                                        : lowerData;
                                                                return data->get(
                                                                    x,
                                                                    y % Level::
                                                                            COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                    z);
                                                            }

                                                            bool
                                                            LevelChunk::setData(
                                                                int x, int y,
                                                                int z, int val,
                                                                int mask,
                                                                bool*
                                                                    maskedBitsChanged) {
                                                                SparseDataStorage* data =
                                                                    y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                        ? upperData
                                                                        : lowerData;
                                                                this->setUnsaved(
                                                                    true);
                                                                int old = data->get(
                                                                    x,
                                                                    y % Level::
                                                                            COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                    z);

                                                                *maskedBitsChanged =
                                                                    ((old &
                                                                      mask) !=
                                                                     (val &
                                                                      mask));

                                                                if (old ==
                                                                    val) {
                                                                    return false;
                                                                }

                                                                data->set(
                                                                    x,
                                                                    y % Level::
                                                                            COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                    z, val);
                                                                int _tile =
                                                                    getTile(x,
                                                                            y,
                                                                            z);
                                                                if (_tile > 0 &&
                                                                    dynamic_cast<
                                                                        EntityTile*>(
                                                                        Tile::tiles
                                                                            [_tile]) !=
                                                                        NULL) {
                                                                    std::shared_ptr<
                                                                        TileEntity>
                                                                        te = getTileEntity(
                                                                            x,
                                                                            y,
                                                                            z);
                                                                    if (te !=
                                                                        NULL) {
                                                                        te->clearCache();
                                                                        te->data =
                                                                            val;
                                                                    }
                                                                }
                                                                return true;
                                                            }

                                                            int LevelChunk::
                                                                getBrightness(
                                                                    LightLayer::
                                                                        variety
                                                                            layer,
                                                                    int x,
                                                                    int y,
                                                                    int z) {
                                                                if (layer ==
                                                                    LightLayer::
                                                                        Sky) {
                                                                    if (level
                                                                            ->dimension
                                                                            ->hasCeiling) {
                                                                        return 0;
                                                                    }
                                                                    SparseLightStorage*
                                                                        skyLight =
                                                                            y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                                ? upperSkyLight
                                                                                : lowerSkyLight;
                                                                    if (!skyLight)
                                                                        return 0;
                                                                    return skyLight
                                                                        ->get(
                                                                            x,
                                                                            y % Level::
                                                                                    COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                            z);
                                                                } else if (
                                                                    layer ==
                                                                    LightLayer::
                                                                        Block) {
                                                                    SparseLightStorage*
                                                                        blockLight =
                                                                            y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                                ? upperBlockLight
                                                                                : lowerBlockLight;
                                                                    if (!blockLight)
                                                                        return 0;
        return blockLight->get(x, y % Level::COMPRESSED_CHUNK_SECTION_HEIGHT,
                           // 4J added } else
        return 0;
                                                                }

                                                                
void LevelChunk::getNeighbourBrightnesses(int* brightnesses,
                                                                    LightLayer::
                                                                        variety
                                                                            layer,
                                                                    int x,
                                                                    int y,
                                                                    int z) {
                                                                    SparseLightStorage*
                                                                        light;
                                                                    if (layer ==
                                                                        LightLayer::
                                                                            Sky)
                                                                        light =
                                                                            y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                                ? upperSkyLight
                                                                                : lowerSkyLight;
                                                                    else
                                                                        light =
                                                                            y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                                ? upperBlockLight
                                                                                : lowerBlockLight;

                                                                    if (light) {
                                                                        brightnesses[0] =
                                                                            light
                                                                                ->get(
                                                                                    x - 1,
                                                                                    y % Level::
                                                                                            COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                                    z);
                                                                        brightnesses[1] =
                                                                            light
                                                                                ->get(
                                                                                    x + 1,
                                                                                    y % Level::
                                                                                            COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                                    z);
                                                                        brightnesses[4] =
                                                                            light
                                                                                ->get(
                                                                                    x,
                                                                                    y % Level::
                                                                                            COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                                    z - 1);
                                                                        brightnesses[5] =
                                                                            light
                                                                                ->get(
                                                                                    x,
                                                                                    y % Level::
                                                                                            COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                                    z + 1);
                                                                    }

                                                                    if (layer ==
                                                                        LightLayer::
                                                                            Sky)
                                                                        light =
                                                                            (y -
                                                                             1) >= Level::
                                                                                        COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                                ? upperSkyLight
                                                                                : lowerSkyLight;
                                                                    else
                                                                        light =
                                                                            (y -
                                                                             1) >= Level::
                                                                                        COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                                ? upperBlockLight
                                                                                : lowerBlockLight;
                                                                    if (light)
                                                                        brightnesses[2] =
                                                                            light
                                                                                ->get(
                                                                                    x,
                                                                                    (y -
                                                                                     1) %
                                                                                        Level::
                                                                                            COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                                    z);

                                                                    if (layer ==
                                                                        LightLayer::
                                                                            Sky)
                                                                        light =
                                                                            (y +
                                                                             1) >= Level::
                                                                                        COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                                ? upperSkyLight
                                                                                : lowerSkyLight;
                                                                    else
                                                                        light =
                                                                            (y +
                                                                             1) >= Level::
                                                                                        COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                                ? upperBlockLight
                                                                                : lowerBlockLight;
                                                                    if (light)
                                                                        brightnesses[3] =
                                                                            light
                                                                                ->get(
                                                                                    x,
                                                                                    (y +
                                                                                     1) %
                                                                                        Level::
                                                                                            COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                                    z);
                                                                }

                                                                void
                                                                LevelChunk::setBrightness(
                                                                    LightLayer::
                                                                        variety
                                                                            layer,
                                                                    int x,
                                                                    int y,
                                                                    int z,
                                                                    int brightness) {
                                                                    this->setUnsaved(
                                                                        true);
                                                                    if (layer ==
                                                                        LightLayer::
                                                                            Sky) {
                                                                        if (!level
                                                                                 ->dimension
                                                                                 ->hasCeiling) {
                                                                            SparseLightStorage* skyLight =
                                                                                y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                                    ? upperSkyLight
                                                                                    : lowerSkyLight;
                                                                            skyLight
                                                                                ->set(
                                                                                    x,
                                                                                    y % Level::
                                                                                            COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                                    z,
                                                                                    brightness);
                                                                        }
                                                                    } else if (
                                                                        layer ==
                                                                        LightLayer::
                                                                            Block) {
                                                                        SparseLightStorage* blockLight =
                                                                            y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                                ? upperBlockLight
                                                                                : lowerBlockLight;
                                                                        blockLight
                                                                            ->set(
                                                                                x,
                                                                                y % Level::
                                                                                        COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                                z,
                                                                                brightness);
                                                                    }
                                                                }

                                                                int LevelChunk::
                                                                    getRawBrightness(
                                                                        int x,
                                                                        int y,
                                                                        int z,
                                                                        int skyDampen) {
                                                                    SparseLightStorage*
                                                                        skyLight =
                                                                            y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                                ? upperSkyLight
                                                                                : lowerSkyLight;
                                                                    int light =
                                                                        level->dimension
                                                                                ->hasCeiling
                                                                            ? 0
                                                                            : skyLight
                                                                                  ->get(
                                                                                      x,
                                                                                      y % Level::
                                                                                              COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                                      z);
                                                                    if (light >
                                                                        0)
                                                                        touchedSky =
                                                                            true;
                                                                    light -=
                                                                        skyDampen;
                                                                    SparseLightStorage*
                                                                        blockLight =
                                                                            y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT
                                                                                ? upperBlockLight
                                                                                : lowerBlockLight;
                                                                    int block =
                                                                        blockLight
                                                                            ->get(
                                                                                x,
                                                                                y % Level::
                                                                                        COMPRESSED_CHUNK_SECTION_HEIGHT,
                                                                                z);
                                                                    /*
                                                                     * int xd
                                                                     * =
                                                                     * (absFloor(level.player.x-(this->x*16+x)));
                                                                     * int
                                                                     * yd
                                                                     * =
                                                                     * (absFloor(level.player.y-(y)));
                                                                     * int
                                                                     * zd
                                                                     * =
                                                                     * (absFloor(level.player.z-(this->z*16+z)));
                                                                     * int dd
                                                                     * =
                                                                     * xd+yd+zd;
                                                                     * if
                                                                     * (dd<15){
                                                                     * int
                                                                     * carried
                                                                     * =
                                                                     * 15-dd;
                                                                     * if
                                                                     * (carried<0)
                                                                     * carried
                                                                     * =
                                                                     * 0;
                                                                     * if
                                                                     * (carried>15)
                                                                     * carried
                                                                     * =
                                                                     * 15;
                                                                     * if
                                                                     * (carried
                                                                     * >
                                                                     * light)
                                                                     * light
                                                                     * =
                                                                     * carried;
                                                                     * }
                                                                     */

    return light;
                                                                }

                                                                void
                                                                LevelChunk::addEntity(
                                                                    std::shared_ptr<
                                                                        Entity>
                                                                        e) {
                                                                    lastSaveHadEntities =
                                                                        true;

                                                                    int xc = Mth::
                                                                        floor(
                                                                            e->x /
                                                                            16);
                                                                    int zc = Mth::
                                                                        floor(
                                                                            e->z /
                                                                            16);
    if (xc != this->x || z"Wrong location!"       app.//        System.out.println("Wrong location! " + e);//        Thread.dumpStack();
        
    
                                                                
                                                                }
                                                                int yc =
                                                                    Mth::floor(
                                                                        e->y /
                                                                        16);
                                                                if (yc < 0)
                                                                    yc = 0;
                                                                if (yc >=
                                                                    ENTITY_BLOCKS_LENGTH)
                                                                    yc =
                                                                        ENTITY_BLOCKS_LENGTH -
                                                                        1;
                                                                e->inChunk =
                                                                    true;
                                                                e->xChunk = x;
#ifdef _ENTITIES_RW_SECTIONzChunk = z;
                                                                
    Enter #elsecalRWSection(&m_csEntities, true);
                                                                
 #endiferCriticalSection(&m_csEntities);
                                                                #ifdef _ENTITIES_RW_SECTIONush_back(
                                                                    e);
                                                                
    Leave #elsecalRWSection(&m_csEntities, true);
                                                                
 #endifveCriticalSection(&m_csEntities);
                                                                

                                                            
                                                            }

                                                            void LevelChunk::
                                                                removeEntity(
                                                                    std::shared_ptr<
                                                                        Entity>
                                                                        e) {
                                                                removeEntity(
                                                                    e,
                                                                    e->yChunk);
                                                            }

                                                            void LevelChunk::
                                                                removeEntity(
                                                                    std::shared_ptr<
                                                                        Entity>
                                                                        e,
                                                                    int yc) {
                                                                if (yc < 0)
                                                                    yc = 0;
    if (yc >= ENTITY_BLOCKS
#ifdef _ENTITIES_RW_SECTIONLENGTH - 1;

    Enter#elsecalRWSection(&m_csEntities, true);

 #endiferCrit// 4J - was entityBlocks[yc]->remove(e);
    AUTO_VAR(it, std::find(entityBlocks[yc]->begin(), entityBlocks[yc]->end(), e));
    if (it != entityBlocks[yc]->end()) {
                                                                    // 4J - we
                                                                    // don't
                                                                    // want
                                                                    // storage
                                                                    // creeping
                                                                    // up here
                                                                    // as
                                                                    // thinkgs
                                                                    // move
                                                                    // round
                                                                    // the//
                                                                    // world
                                                                    // accumulating
                                                                    // up spare
                                                                    // space
                                                                    #ifdef __PS3__    // MGH - have to sort this C++11 code        
                                                                        static bool
                                                                            bShowMsg =
                                                                                true;
       "Need to add C++11 shrink_to_fit for PS3\n"f(#else            bShowMsg = false;
        }
#endif    entityBlocks[yc]->shrin
#ifdef _ENTITIES_RW_SECTIONemSect(0);
                                                            }
                                                            
    #elseCriticalRWSection(&m_csEntities, true);
                                                            #endif LeaveCriticalSection(
                                                                &m_csEntities);
                                                            

                                                        
                                                        }

                                                        bool
                                                        LevelChunk::isSkyLit(
                                                            int x, int y,
                                                            int z) {
                                                            return y >=
                                                                   (heightmap
                                                                        [z << 4 |
                                                                         x] &
                                                                    0xff);
                                                        }

                                                        void LevelChunk::
                                                            skyBrightnessChanged() {
                                                            int x0 =
                                                                this->x * 16;
                                                            int y0 =
                                                                this->minHeight -
                                                                16;
                                                            int z0 =
                                                                this->z * 16;
                                                            int x1 =
                                                                this->x * 16 +
                                                                16;
                                                            int y1 =
                                                                Level::
                                                                    maxBuildHeight -
                                                                1;
                                                            int z1 =
                                                                this->z * 16 +
                                                                16;

                                                            level
                                                                ->setTilesDirty(
                                                                    x0, y0, z0,
                                                                    x1, y1, z1);
                                                        }

std::shared_ptr<TileEntity> LevelChunk::getTileEntity(int x, in// 4J Stu - Changed as we should not be using the [] accessor (causes an// insert when we don't want one)// shared_ptr<TileEntity> tileEntity = tileEntities[pos];
    EnterCriticalSection(&m_csTileEntities);
    std::shared_ptr<TileEntity> tileEntity = nullptr;
    AUTO_VAR(it, tileEntities.find(pos));

    if (it == tileEntities.end()) {
                                                            Leave  // Note:
                                                                   // don't
                                                                   // assume
                                                                   // iterator
                                                                   // is valid
                                                                   // fories);
                                                                   // //
                                                                   // tileEntities
                                                                   // after this
                                                                   // point //
                                                                   // Fix for
                                                                   // #48450 -
                                                                   // All: Code
                                                                   // Defect:
                                                                   // Hang: Game
                                                                   // hangs in
                                                                   // tutorial,
                                                                   // when//
                                                                   // player
                                                                   // arrive at
                                                                   // the
                                                                   // particular
                                                                   // coordinate
                                                                   // 4J Stu -
                                                                   // Chests try
                                                                   // to
                                                                   // get//
                                                                   // their
                                                                   // neighbours
                                                                   // when being
                                                                   // destroyed,
                                                                   // which then
                                                                   // causes new
                                                                   // tile//
                                                                   // entities
                                                                   // to be
                                                                   // created if
                                                                   // the
                                                                   // neighbour
                                                                   // has
                                                                   // already
                                                                   // been
                                                                   // destroyed
                                                                if (level
                                                                        ->m_bDisableAddNewTileEntities) return nullptr;

                                                            int t = getTile(
                                                                x, y, z);
        if (t <= 0 || !Tile::t// 4J-PB changed from this in 1.7.3lptr;

  // EntityTile *et = (EntityTile *) Tile::tiles[t];// et->onPlace(level, this->x * 16 + x, y, this->z * 16 + z);// if (tileEntity == NULL)//{

        
        
        tileEntity =
            dynamic_cast<EntityTile*>(Tile::tiles[t])->newTileEntity(level);
        level->setTileEntity(this->x * //}+ x, y, th// tileEntity = tileEntities[pos];		// 4J - TODO - this// doesn't seem right - assignment wrong way? Check// 4J Stu - It should have been inserted by now, but check to be sure
        EnterCriticalSection(&m_csTileEntities);
        AUTO_VAR(newIt, tileEntities.find(pos));
        if (newIt != tileEntities.end()) {
                                                                tileEntity =
                                                                    newIt
                                                                        ->second;
        }
        LeaveCriticalSection(&m_csTileEntities);
    } else {
                                                            tileEntity =
                                                                it->second;
                                                            LeaveCriticalSection(
                                                                &m_csTileEntities);
    }
    if (tileEntity != NULL && tileEntity->isRemoved()) {
                                                            EnterCriticalSection(
                                                                &m_csTileEntities);
                                                            tileEntities.erase(
                                                                pos);
                                                            LeaveCriticalSection(
                                                                &m_csTileEntities);
                                                            return nullptr;
    }

    return tileEntity;
                                                    }

                                                    void
                                                    LevelChunk::addTileEntity(
                                                        std::shared_ptr<
                                                            TileEntity>
                                                            te) {
                                                        int xx =
                                                            (int)(te->x -
                                                                  this->x * 16);
                                                        int yy = (int)te->y;
                                                        int zz =
                                                            (int)(te->z -
                                                                  this->z * 16);
                                                        setTileEntity(xx, yy,
                                                                      zz, te);
                                                        if (loaded) {
                                                            EnterCriticalSection(
                                                                &level
                                                                     ->m_tileEntityListCS);
                                                            level
                                                                ->tileEntityList
                                                                .push_back(te);
                                                            LeaveCriticalSection(
                                                                &level
                                                                     ->m_tileEntityListCS);
                                                        }
                                                    }

                                                    void
                                                    LevelChunk::setTileEntity(
                                                        int x, int y, int z,
                                                        std::shared_ptr<
                                                            TileEntity>
                                                            tileEntity) {
                                                        TilePos pos(x, y, z);

                                                        tileEntity->setLevel(
                                                            level);
                                                        tileEntity->x =
                                                            this->x * 16 + x;
                                                        tileEntity->y = y;
                                                        tileEntity->z =
                                                            this->z * 16 + z;

    if (getTile(x, y, z) == 0 ||
        !Tile::tiles[// 4J - was !(Tile.tiles[getTile(x, y, z)]ile())  // instanceof EntityTile))                        "Attempted to place a tile entity where there was no entity ""tile!\n"
            )
        ;
    return;
                                                    }
                                                    AUTO_VAR(
                                                        it,
                                                        tileEntities.find(pos));
                                                    if (it !=
                                                        tileEntities.end())
                                                        it->second
                                                            ->setRemoved();

                                                    tileEntity->clearRemoved();

                                                    EnterCriticalSection(
                                                        &m_csTileEntities);
                                                    tileEntities[pos] =
                                                        tileEntity;
                                                    LeaveCriticalSection(
                                                        &m_csTileEntities);
                                                }

                                                void
                                                LevelChunk::removeTileEntity(
                                                    int x, int y, int z) {
                                                    // 4J - was:(x, y, z)//
                                                    // TileEntity removeThis =
                                                    // tileEntities.remove(pos);//
                                                    // if (removeThis != null)
                                                    // {//
                                                    // removeThis.setRemoved();
                                                    //   }
                                                    //   
                                                    
        EnterCriticalSection(&m_csTileEntities);
                                                    AUTO_VAR(
                                                        it,
                                                        tileEntities.find(pos));
                                                    if (it !=
                                                        tileEntities.end()) {
                                                        std::shared_ptr<
                                                            TileEntity>
                                                            te = tileEntities
                                                                [pos];
                                                        tileEntities.erase(pos);
                                                        if (te != NULL) {
                if (leve"Removing tile entity of type %d\n"    app.DebugPrintf(,
                                    te->GetType());
                                                        }
                                                        te->setRemoved();
                                                    }
                                                }
                                                LeaveCriticalSection(
                                                    &m_csTileEntities);
    }
}

void LevelChunk::load()#ifdef _LARGE_WORLDS;

    if (!level->isClientSide) {
                                            
        if (m_bUnloaded && m_unloadedEntitiesTag) {
            ListTag<CompoundTag>* entityTags =
                (ListTag<CompoundTag>"Entities"edEntitiesTag->getList(
                    L);
            if (entityTags != NULL) {
                                                    for (int i = 0;
                                                         i < entityTags->size();
                                                         i++) {
                                                        CompoundTag* teTag =
                                                            entityTags->get(i);
                                                        std::shared_ptr<Entity>
                                                            ent = EntityIO::
                                                                loadStatic(
                                                                    teTag,
                                                                    level);
                                                        if (ent != NULL) {
                                                            ent->onLoadedFromSave();
                                                            addEntity(ent);
                                                        }
                                                    }
            }

            ListTag<CompoundTag>* tileEntityTags =
                (ListTag<Compoun"TileEntities"dedEntitiesTag->getList(
                    L);
            if (tileEntityTags != NULL) {
                                                    for (int i = 0;
                                                         i <
                                                         tileEntityTags->size();
                                                         i++) {
                                                        CompoundTag* teTag =
                                                            tileEntityTags->get(
                                                                i);
                                                        std::shared_ptr<
                                                            TileEntity>
                                                            te = TileEntity::
                                                                loadStatic(
                                                                    teTag);
                                                        if (te != NULL) {
                                                            addTileEntity(te);
                                                        }
                                                    }
            }
            delete m_unloadedEntitiesTag;
            m_unload#endiftiesTag = NULL;
            m_bUnloaded = false;
                                            }
                                            

        std::vector<std::shared_ptr<TileEntity> >
                                                values;
                                            EnterCriticalSection(
                                                &m_csTileEntities);
                                            for (AUTO_VAR(it,
                                                          tileEntities.begin());
                                                 it != tileEntities.end();
                                                 it++) {
                                                values.push_back(it->second);
                                            }
        LeaveCriticalSec
#ifdef _ENTITIES_RW_SECTION level->addAllPendingTileEntities(values);
#else
        EnterCriticalRWSectio#endifsEntities, true);
        
        EnterCriticalSection(&m_csEntities);

        for (int i = 0; i < ENTITY_BLOCKS#ifdef _ENTITIES_RW_SECTION level->addEntities(entityBlocks[i]);
        }
#else
        LeaveCriticalRWSectio#endifsEntities, tru#ifdef _LARGE_WORLDSaveCriticalSection(&m_csEntiti#endif
    
                                    
                                    }
                                    else {
                                        
        m_bUnloaded  // 4J - added parameter

                                            void LevelChunk::unload(
                                                bool unloadTileEntities)  
{
                                            loaded = false;
                                            if (unloadTileEntities) {
                                                EnterCriticalSection(
                                                    &m_csTileEntities);
        for (AUTO_VAR(it, tileEnt// 4J-PB -m 1.7.3 was it->second->setRemoved();        it++) {
            
            level->markForRemoval(it-
#ifdef _ENTITIES_RW_SECTIONLeaveCriticalSection(&m_csTileEntities);
                                            }
#else
    EnterCriticalRWSe#endif&m_csEntities, true);

    EnterCriticalSection(&m_csEntities);

    for (int i = 0; i < ENTITY_B#ifdef _ENTITIES_RW_SECTION  level->removeEntities(entityBlocks[i]);
                                        }
                                        #else 
    LeaveCriticalRWSe #endif& m_cs      // app.DebugPrintf("Unloaded chunk %d,
                                        // %d\n", x, z);s
#ifdef _LARGE_WORLDS     // 4J-JEV: If we unload a chunk twice, we delete all theif (!m_bUnloaded)  // entities/tile-entities its saved in the entitiesTag.                    
                                        {
                                            m_bUnloaded = true;
        if (!le// 4J Stu - Save out entities to a cached format that won'tag;
         // interfere with other systems
            
            m_"Saving entities"ag = new CompoundTag();
            PIXBeginNamedEvent(0, );
            ListTag<CompoundTag>* entityTags = new ListTag<CompoundTag>();

            EnterCriticalSection(&m_csEntities);
            for (int i = 0; i < ENTITY_BLOCKS_LENGTH; i++) {
                                                AUTO_VAR(
                                                    itEnd,
                                                    entityBlocks[i]->end());
                                                for (std::vector<
                                                         std::shared_ptr<
                                                             Entity> >::iterator
                                                         it = entityBlocks[i]
                                                                  ->begin();
                                                     it != itEnd; it++) {
                                                    std::shared_ptr<Entity> e =
                                                        *it;
                                                    CompoundTag* teTag =
                                                        new CompoundTag();
                                                    if (e->save(teTag)) {
                                                        // Clear out this
                                                        // listdd(teTag);
                                                    }
                                                }

                                                
                entityBlocks[i]
                                                    ->clear();
            }
"Entities"  LeaveCriticalSection(&m_csEntities);

            m_unloadedEntitiesTag->put(L"Saving tile entities"
            PIXEndNamedEvent();

            PIXBeginNamedEvent(0, );
            ListTag<CompoundTag>* tileEntityTags = new ListTag<CompoundTag>();

            AUTO_VAR(itEnd, tileEntities.end());
            for (std::unordered_map<TilePos, std::shared_ptr<TileEntity>, TilePosKeyHash,
                               TilePosKeyEq>::iterator it =
                     tileEntities.begin();
                 it != itEnd; it++) {
                                                std::shared_ptr<TileEntity> te =
                                                    it->second;
                                                CompoundTag* teTag =
                                                    new CompoundTag()  // Clear
                                                                       // out
                                                                       // the
                                                                       // tileEntities
                                                                       // list
                                                    tileEntityTags->add(teTag);
            }
            "TileEntities"
            tileEntities.clear();

            m_un#endifEntitiesTag->put(L, tileEn#ifdef _ENTITIES_RW_SECTIONndNamedEvent();
                                        }
                                    }
                                    

                                
                                }

                                bool Lev #elsenk::containsPlayer() {
                                    #endif 
    EnterCriticalRWSection(&m_csEntities, true);
                                    
    EnterCriticalSection(&m_csEntities);
                                    
    for (int i = 0; i < ENTITY_BLOCKS_LENGTH; i++) {
                                        std::vector<std::shared_ptr<Entity> >*
                                            vecEntity = entityBlocks[i];
#ifdef _ENTITIES_RW_SECTIONty->size(); j++) {
                                        if (vecEntity->at(j)
                                                ->GetType() #elseTYPE_SERVERPLAYER) {
                                            
     #endif LeaveCriticalRWSection(&m_csEntities, true);
                                            
    #ifdef _ENTITIES_RW_SECTIONction(&m_csEntities);
                                            
                retur #elsee;
                                        }
                                    }
                                }
                                #endif 
    Leav
#ifdef _LARGE_WORLDS_csEntities, true);

    LeaveCriticalSection(&m_c #endifies);
                                
    return false;
                            }
                            
bool LevelChunk::isUnloaded() {
                                return m_bUnloaded;
                            }
                            

void LevelChunk::markUnsaved() {
                                this->setUnsaved(true);
                            }

                            void LevelChunk::getEntities(
                                std::shared_ptr<Entity> except, AABB * bb,
                                std::vector<std::shared_ptr<Entity> > & es,
                                const EntitySelector* selector) {
                                int yc0 = Mth::floor((bb->y0 - 2) / 16);
                                int yc1 = M
#ifndef __PSVITA__ 2)  /// AP - RW critical sections are expensive so enter once
                       /// inS_LEN// Level::getEntitiesCKS_LENGTH - 1;

    #endif 
    
    EnterCriticalSection(&m_csEntities);
                                
    for (int yc = yc0; yc <= yc1; yc++) {
                                    std::vector<std::shared_ptr<Entity> >*
                                        entities = entityBlocks[yc];

                                    AUTO_VA  // entities->at(i);end());
                                        for (AUTO_VAR(it, entities->begin());
                                             it != itEnd; it++) {
                                        std::shared_ptr<Entity> e = *it;
                                        
            if (e != except && e->bb->intersects(bb) &&
                (selector == NULL || selector->matches(e))) {
                                            es.push_back(e);
                                            std::vector<
                                                std::shared_ptr<Entity> >*
                                                subs = e->getSubEntities();
                                            if (subs != NULL) {
                                                for (int j = 0;
                                                     j < subs->size(); j++) {
                                                    e = subs->at(j);
                        if (e != except && e->bb->intersects(bb) &&
                            (selector == NULL || selector->matche#ifndef __PSVITA__                   es.push_back(e);
#endif }
                                                }
                                            }
                                        }
                                    }
                                }
                                
    LeaveCriticalSection(&m_csEntities);
                                

                            
                            }

                            void LevelChunk::getEntitiesOfClass(
                                const std::type_info& ec, AABB* bb,
                                std::vector<std::shared_ptr<Entity> >& es,
                                const EntitySelector* selector) {
                                int yc0 = Mth::floor((bb->y0 - 2) / 16);
                                int yc1 = Mth::floor((bb->y1 + 2) / 16);

                                if (yc0 < 0) {
                                    yc0 = 0;
                                } else if (yc0 >= ENTITY_BLOCKS_LENGTH) {
                                    yc0 =
#ifndef __PSVITA__H - \
    1  // AP - RW critical sections are expensive so enter once in= ENT//
       // Level::getEntitiesOfClass else if (yc1 < 0) {
                                        yc1 = 0;
#endif 
    
    
    EnterCriticalSection(&m_csEntities);
                                    
    for (int yc = yc0; yc <= yc1; yc++) {
                                        std::vector<std::shared_ptr<Entity> >*
                                            entities =  // entities->at(i);
                                            AUTO_VAR(itEnd, entities->end());
        for (A// Some special cases where the base class is a general type thatstd::shared_p// our class may be derived from, otherwise do a direct comparisonnableFrom = f// of type_info   
            
            
            if (ec == typeid(Player))
                isAssignableFrom = e->instanceof(eTYPE_PLAYER);
            else if (ec == typeid(Entity))
                isAssignableFrom = e->instanceof(eTYPE_ENTITY);
            else if (ec == typeid(Mob))
                isAssignableFrom = e->instanceof(eTYPE_MOB);
            else if (ec == typeid(LivingEntity))
                isAssignableFrom = e->instanceof(eTYPE_LIVINGENTITY);
            else if (ec == typeid(ItemEntity))
                isAssignableFrom = e->instanceof(eTYPE_ITEMENTITY);
            else if (ec == typeid(Minecart))
                isAssignableFrom = e->instanceof(eTYPE_MINECART);
            else if (ec == typeid(Monster))
                isAssignableFrom = e->instanceof(eTYPE_MONSTER);
            else if (ec == typeid(Zombie))
                isAssignableFrom = e->instanceof(eTYPE_ZOMBIE);
            else if (e != NULL && ec == typeid(*(e.get())))
                isAssignableFrom = true;
            if (isAssignableFro// 4J - note needs to be equivalent to        if (s// baseClass.isAssignableFrom(e.getClass())                 #ifndef __PSVITA__;
                                    }
                                }
#endif 
            #ifdef _ENTITIES_RW_SECTION
        }
                            }
                            
    LeaveCrit #elseection(&m_csEntities);
                            

                        
                        }

                        int Level #endif : countEntities() {
                            int entityCount = 0;

    EnterCriticalRWSection(&m_csEntities, fal#ifdef _ENTITIES_RW_SECTIONlSection(&m_csEntities);

    for (int yc = 0#else< ENTITY_BLOCKS_LENGTH; yc++) {
                                en #endifunt += (int)entityBlocks[yc]->size();
    }

    LeaveCriticalRWSection(&m_csEntities, false);

    LeaveCriticalSection(&m_csEntities);

    return entityCount;
                        }

                        bool LevelChunk::shouldSave(bool force) {
                            if (dontSave) return false;
                            if (force) {
                                if ((lastSaveHadEntities &&
                                     level->getGameTime() != lastSaveTime) ||
                                    m_unsaved) {
                                    return true;
                                }
                            } else {
                                if (lastSaveHadEntities &&
                                    level->getGameTime() >=
                                        lastSaveTime + 20 * 30)
                                    return true;
                            }

                            return m_unsaved;
                        }

                        int LevelChunk::g /* = true*/ Data(
                            byteArray * data, int x0, int y0, int z0,
                            // 4J Stu - Added this because some "min" functions
                            // don't let us use our incl// constants :()
                            // {
                            int xs = x1 - x0;
                            int ys = y1 - y0;
                            int zs =
                                z1 -  // 4J - replaced block storage as now
                                      // using
                                      // CompressedTileStorage
    
    int compressedHeight = Level::COMPRESSED_CHUNK_SECTION_HEIGHT;

    
    if (y0 < Level::COMPRESSED_CHUNK_SECTION_HEIGHT) p +=
                            lowerBlocks->getDataRegion(
                                *data, x0, y0, z0, x1,
                                std::min(compressedHeight, y1), z1, p);
                            if (y1 >
                                    Level::COMPRESSED_CHUNK_SECTION_H  // 4J -
                                                                       // replaced
                                                                       // data
                                                                       // storage
                                                                       // as now
                                                                       // using
                                                                       // SparseDataStorage*data,
                                                                       // x0,
                                                                       // std::max(y0
                                                                       // -
                                                                       // compressedHeight,
                                                                       // 0),
                                                                       // z0,
                                                                       // x1,
                                            y1 -
                                        Level::COMPRESSED_CHUNK_SECTION_HEIGHT,
                                z1, p);

    
    if (y0 < Level::COMPRESSED_CHUNK_SECTION_HEIGHT) p +=
                            lowerData->getDataRegion(
                                *data, x0, y0, z0, x1,
                                std::min(compressedHeight, y1), z1, p);
                            if (y1 > Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
                                p +=  // 4J - replaced block and skylight
                                      // storage as these now use ourcompresse//
                                      // SparseLightStorage            y1 -
                                      // Level::COMPRESSED_CHUNK_SECTION_HEIGHT,
                                      // z1, p);

                            if (includeLighting) {
                                
        
        if (y0 < Level::COMPRESSED_CHUNK_SECTION_HEIGHT) p +=
                                    lowerBlockLight->getDataRegion(
                                        *data, x0, y0, z0, x1,
                                        std::min(compressedHeight, y1), z1, p);
                                if (y1 > Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
                                    p += upperBlockLight->getDataRegion(
                                        *data, x0,
                                        std::max(y0 - compressedHeight, 0), z0,
                                        x1,
                                        y1 -
                                            Level::
                                                COMPRESSED_CHUNK_SECTION_HEIGHT,
                                        z1, p);

                                if (y0 < Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
                                    p += lowerSkyLight->getDataRegion(
                                        *data, x0, y0, z0, x1,
                                        std::min(compressedHeight, y1), z1, p);
        if (y1 > Level::COMPRESSED_CHUNK/*
    for (int x = x0; x < x1; x++)
    for (int z = z0; z < z1; z++)
    {
    int slot = (x << level->depthBitsPlusFour | z << level->depthBits | y0) >>
    1; int len = (y1 - y0) / 2; System::arraycopy(blockLight->data, slot, data,
    p, len); p += len;
    }

    for (int x = x0; x < x1; x++)
    for (int z = z0; z < z1; z++)
    {
    int slot = (x << level->depthBitsPlusFour | z << level->depthBits | y0) >>
    1; int len = (y1 - y0) / 2; System::arraycopy(skyLight->data, slot, data, p,
    len); p += len;
    }
    */// 4J added - return true if setBlocksAndData would change any blocks

    return p;
                            }

    // 4J Stu - Added this because some "min" functions don't let us use our(uint8_t// constants :( x0, int y0, int z0,
                            int x1,
                            int y1, int z1, int p) {
                            bool changed = false;

                            
    
    int compressedHeight = Level::COMPRESSED_CHUNK_SECTION_HEIGHT;

                            if (y0 < Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
                                changed = lowerBlocks->testSetDataRegion(
                                    data, x0, y0, z0, x1,
                                    std::min(compressedHeight, y1), z1, p);
                            if (y1 > Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
                                changed =
                                    changed ||
                                    upperBlocks->testSetDataRegion(
                                        data, x0,
                                        std::max(y0 - compressedHeight, 0), z0,
                                        x1,
                                        y1 -
                                            Level::
                                                COMPRESSED_CHUNK_SECTION_HEIGHT,
                                        z1, p);

                            return changed;
                        }

                        void LevelChunk::tileUpdatedCallback(
                            int x, int y, int z, void* param, int yparam) {
                            LevelChunk* lc = (LevelChunk*)param;
                            int xx = lc->x * 16 + x;
                            int yy = y + yparam;
                            int zz = lc->z * 16 + z;
    lc->/* = true*/kLight(x// If includeLighting is set, then this is a full chunk's worth of data thaty0, i// we are receiving on the client. We'll have made this chunk initially as     // compressed, so throw that data away and make some fully uncompressed// storage now to improve the speed up writing to it. Only doing this for// lower chunks as quite likely that the upper chunk doesn't have anything// in anyway.
    
    
    
    if (includeLighting) {
                                GameRenderer::AddForDelete(lowerBlocks);
                                byteArray emptyByteArray;
                                lowerBlocks = new CompressedTileStorage(
                                    emptyByteArray, 0);
                                GameRenderer::FinishedReassigning();

                                GameRenderer::AddForDelete(lowerSkyLight);
                                lowerSkyLight =
                                    new SparseLightStorage(true, false);
                                GameRenderer::FinishedReassigning();

                                GameRenderer::AddForDelete(lowerBlockLight);
        lowerBlockLight = new SparseLi// 4J Stu - Added this because some "min" functions don't let us use our     // constants :(::AddForDelete(lowerData);
        lowerData = new SparseDataStorage(fals// 4J - replaced block storage as now uses CompressedTileStorage
    
    int compressedHeight = Level::COMPRESSED_CHUNK_SECTION_HEIGHT;

    
    if (y0 < Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
        p += lowerBlocks->setDataRegion(
            data, x0, y0, z0, x1, std::min(compressedHeight, y1), z1, p,
            includeLighting ? NULL : tileUpdatedCallback, this, 0);
    if (y1 > Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
        p += upperBlocks->setDataRegion(
            data/*
    for (int x = x0; x < x1; x++)
    for (int z = z0; z < z1; z++)
    {
    int slot = x << level->depthBitsPlusFour | z << level->depthBits | y0;
    int len = y1 - y0;
    System::arraycopy(data, p, &blocks, slot, len);
    p += len;
    }*/// 4J - replaced data storage as now uses SparseDataStorage

    recalcHeightmapOnly();

    
    if (y0 < Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
        p += lowerData->setDataRegion(
            data, x0, y0, z0, x1, std::min(compressedHeight, y1), z1, p,
            includeLighting ? NULL : tileUpdatedCallback, this, 0);
    if (y1 > Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
        p += upperData->setDataRegion(
            data, x0, std::max(y0 - co// 4J - replaced block and skylight storage as these now use ourHUNK_SECT// SparseLightStorage          includeLighting ? NULL : tileUpdatedCallback, this,
            Level::COMPRESSED_CHUNK_SECTION_HEIGHT);

    if (includeLighting) {
                                    
        
        if (y0 < Level::COMPRESSED_CHUNK_SECTION_HEIGHT) p +=
                                        lowerBlockLight->setDataRegion(
                                            data, x0, y0, z0, x1,
                                            std::min(compressedHeight, y1), z1,
                                            p);
                                    if (y1 >
                                        Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
                                        p += upperBlockLight->setDataRegion(
                                            data, x0,
                                            std::max(y0 - compressedHeight, 0),
                                            z0, x1,
                                            y1 -
                                                Level::
                                                    COMPRESSED_CHUNK_SECTION_HEIGHT,
                                            z1, p);

                                    if (y0 <
                                        Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
                                        p += lowerSkyLight->setDataRegion(
                                            data, x0, y0, z0, x1,
                                            std::min(compressedHeight, y1), z1,
                                            p);
                                    if (y1 >
                                        Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
                                        p +=
                                            upperSkyLight  // Because the host's
                                                           // local client
                                                           // shares data with
                                                           // it, the
                                                           // lightingeight,
                                                           // 0)// updates that
                                                           // are done via
                                                           // callbacks in the
                                                           // setDataRegion
                                                           // calls abovez1, p);

                                            // when things don't work, as they
                                            // don't detect changes because
                                            // they've biomes.l// already
                                            // happened just because the data
                                            // was being shared when
                                            // the// server updated
                                            // them. This will leave the
                                            // lighting information out
                                            // of// sync on the client,
                                            // so resync for this & surrounding
                                            // chunks that// might have
                                            // been affected
                                            // 
        
        
        
        if (level->isClientSide && g_NetworkManager.IsHost()) {
                                            reSyncLighting();
                                            level->getChunk(x - 1, z - 1)
                                                ->reSyncLighting();
                                            level->getChunk(x - 0, z - 1)
                                                ->reSyncLighting();
                                            level->getChunk(x + 1, z - 1)
                                                ->reSyncLighting();
                                            level->getChunk(x - 1, z + 0)
                                                ->reSyncLighting();
                                            /*
    for (int x = x0; x < x1; x++)
    for (int z = z0; z < z1; z++)
    {
    int slot = (x << level->depthBitsPlusFour | z << level->depthBits | y0) >>
    1; int len = (y1 - y0) / 2; System::arraycopy(data, p, &blockLight->data,
    slot, len); p += len;
    }

    for (int x = x0; x < x1; x++)
    for (int z = z0; z < z1; z++)
    {
    int slot = (x << level->depthBitsPlusFour | z << level->depthBits | y0) >>
    1; int len = (y1 - y0) / 2; System::arraycopy(data, p, &skyLight->data,
    slot, len); p += len;
    }
    *///        recalcHeightmap();// If the includeLighting flag is set, then this is a full chunk's worth of// data. This is a good time to compress everything that we've just set up. != tileEntities.end(); ++it) {
        it->second->clearCache();
                                        }
                                    

    
    
    if (includeLighting) {
                                        compressLighting();
                                        compressBlocks();
                                        compressData();
                                    }

                                    return p;
}

void LevelChunk::setCheckAllLight() {
                                    checkLightPosition = 0; }

Random* LevelChunk::getRan// 4J - removed{#if 0return new Random((level->getSeed() + x * x * 4987142 + x * 5947611 +
                       z * z * 4392871l + z * 389711) ^
                      l);
}

bool LevelChunk::isEmpty() {
                                return false; }
void LevelChun"Compressed size: "on() {
                                

	try {
                                    ByteArrayOutputStr #endifaos =
                                        new ByteArrayOutputStream();
                                    GZIPOutputStream* gzos =
                                        new GZIPOutputStream(baos);
                                    DataOutputStream* dos =
                                        new DataOutputStream(gzos);
                                    dos.close();
                                    System.out
                                        .println( +
                                                 baos.toByteArray().length);
                                } catch (Exception e) {
                                }
                                
}

void LevelChunk::checkPostProcess(ChunkSource* source, ChunkSource* parent,
                                  int x, int z) {
                                if (((terrainPopulated &
                                      sTerrainPopulatedFromHere) == 0) &&
                                    source->hasChunk(x + 1, z + 1) &&
                                    source->hasChunk(x, z + 1) &&
                                    source->hasChunk(x + 1, z)) {
                                    source->postProcess(parent, x, z);
                                }
                                if (source->hasChunk(x - 1, z) &&
                                    ((source->getChunk(x - 1, z)
                                          ->terrainPopulated &
                                      sTerrainPopulatedFromHere) == 0) &&
                                    source->hasChunk(x - 1, z + 1) &&
                                    source->hasChunk(x, z + 1) &&
                                    source->hasChunk(x - 1, z + 1)) {
                                    source->postProcess(parent, x - 1, z);
                                }
                                if (source->hasChunk(x, z - 1) &&
                                    ((source->getChunk(x, z - 1)
                                          ->terrainPopulated &
                                      sTerrainPopulatedFromHere) == 0) &&
                                    source->hasChunk(x + 1, z - 1) &&
                                    source->hasChunk(x + 1, z)) {
                                    source->postProcess(parent, x, z - 1);
                                }
    if (sour// 4J added - check for any pre-1.8.2 chests in the chunk at (x,z), and>// calculate their facing direction & relight to bring up to date with theo// post 1.8.2 build - 1) && source->hasChunk(x - 1, z)) {
        source->postProcess(parent, x - 1, z - 1);
    }
                        }

                        


void LevelChunk::checkChests(ChunkSource * source, int x, int z) {
                            LevelChunk* lc = source->getChunk(x, z);

                            for (int xx = 0; xx < 16; xx++)
                                for (int zz = 0; zz < 16; zz++)
                                    for (int yy = 0; yy < 128; yy++) {
                                        if (lc->getTile(xx, yy, zz) ==
                                            Tile::chest_Id) {
                                            if (lc->getData(xx, yy, zz) == 0) {
                                                int xOffs = x * 16 + xx;
                                                int zOffs = z * 16 + zz;
                                                ChestTile*
                                                    tile  // 4J - lighting
                                                          // change brought
                                                          // forward
                                                          // from 1.8.2les[Tile::chest_Id];
                                                        tile->recalcLockDir(
                                                            level, xOffs, yy,
                                                            zOffs);
                                                level->checkLight(xOffs, yy,
                                                                  zOffs, true);
                                            }
                                        }
                                    }
                        }

                            // 4J Unused/*if (y1 < 0) {
                            y1 = 0;
                    }
                    if (y2 >= Level.maxBuildHeight) {
                        y2 = Level.maxBuildHeight - 1;
                    }
                    for (int y = y1; y <= y2; y += 16) {
                        LevelChunkSection section = sections[y >> 4];
                        if (section != null && !section.isEmpty()) {
                            return false;
                        }
                    }
                    return true;
                    */    // 4J Added
                }

                
void LevelChunk::reloadBiomes() {
                    BiomeSource* biomeSource = level->dimension->biomeSource;
                    for (unsigned int x = 0; x < 16; ++x) {
                        for (unsigned int z = 0; z < 16; ++z) {
                            Biome* biome = biomeSource->getBiome(
                                (this->x << 4) + x, (this->z << 4) + z);
                            biomes[(z << 4) | x] = (uint8_t)((biome->id) & 0xff);
                        }
                    }
                }

                Biome* LevelChunk::getBiome(int x, int z,
                                            BiomeSource* biomeSource) {
                    int value = biomes[(z << 4) | x] & 0xff;
                    if (value == 0xff) {
                        Biome* biome = biomeSource->getBiome(
                            (this->x << 4) + x, (this->z << 4) + z);
                        value = biome->id;
                        biomes[(z << 4) | x] = (uint8_t)(value & 0xff);
                    }
                    if (Biome::biomes[value] == NULL) {
                        // 4J - optimisation brought forward
                        // from 1.8.2iome::biomes[value];
                    }

                    byteArray LevelChunk::getBiomes() { return biomes; }

                    void LevelChunk::setBiomes(byteArray biomes) {
                        if (this->biomes.data != NULL)
                            delete[] this->biomes.data;
                        this->biomes = biomes;
                    }

                    
int LevelChunk::getTopRainBlock(int x, int z) {
                        int slot = x | (z << 4);
                        int h = rainHeights[slot];

                        if (h == 255) {
                            int y = Level::maxBuildHeight - 1;
                            h = -1;
                            while (y > 0 && h == -1) {
                                // 255 indicates that the rain height needs
                                // recalculated. If the rainerial::ai// height
                                // ever actually Does get to 255, then it will
                                // just keep not !m->isLi// being cached, so
                                // probably better just to let the rain height
                                // be 254= y + 1;
                                // in this instance and suffer a slightly
                                // incorrect
                                // results
                                    // 4J added as optimisation, these biome checks are expensive so caching through// flags in levelchunk
        
        if (h == 255) h = 254;
                                rainHeights[slot] = h;
                            }

                            return h;
                        }

                            // 4J added as optimisation, these biome checks are expensive so caching through // flags in levelchunkBiomeFlags(x, z);
                            int slot = (x >> 1) | (z * 8);
                        int shift = (x & 1) * 4;
                        return ((columnFlags[slot] &
                                 (eColumnFlag_biomeHasRain << shift)) != 0);
                    }

                    

bool LevelChunk::biomeHasSnow(int x, int z) {
                        updateBiomeFlags(x, z);
                        int slot = (x >> 1) | (z * 8);
                        int shift = (x & 1) * 4;
                        return ((columnFlags[slot] &
                                 (eColumnFlag_biomeHasSnow << shift)) != 0);
                    }

                    void LevelChunk::updateBiomeFlags(int x, int z) {
                        int slot = (x >> 1) | (z * 8);
                        int shift = (x & 1) * 4;
                        if ((columnFlags[slot] &
                             (eColumnFlag_biomeOk << shift)) == 0) {
                            int xOffs = (this->x * 16) + x;
                            int zOffs = (this->z * 16) + z;
                            BiomeArray biomes;
                            level->getBiomeSource()->getBiomeBlock(
                                biomes, xOffs, zOffs, 1, 1, true);
                            if (biomes[0]->hasRain())
                                // Get a uint8_t array of length 16384 ( 128 x 16 x
                                // 16 x 0.5 ), containing data.o// Ordering same
                                // as java version if originalOrder
                                // set;ColumnFlag_biomeHasSnow << shift);
                                columnFlags[slot] |=
                                    (eColumnFlag_biomeOk << shift);
                            delete biomes.data;
                        }
                    }

                    
    // Set data to data passed in input uint8_t array of length 16384. This data must // be in original (java version) order if originalOrder set. > Level::COMPRESSED_CHUNK_SECTION_TILES / 2)
                        upperData->getData(
                            data, Level::COMPRESSED_CHUNK_SECTION_TILES / 2);
                }

                

void LevelChunk::setDataData(byteArray data) {
                    if (lowerData == NULL)
                        lowerData =
                            new Sp  // Get a uint8_t array of length 16384 ( 128 x
                                    // 16 x 16 x 0.5 ), containing skyr// light
                                    // data. Ordering same as java version if
                                    // originalOrder
                                    // set;Level::COMPRESSED_CHUNK_SECTION_TILES
                                    // / 2)
                                upperData->setData(
                                    data,
                                    Level::COMPRESSED_CHUNK_SECTION_TILES / 2);
                }

                
    // Get a uint8_t array of length 16384 ( 128 x 16 x 16 x 0.5 ), containing block{// light data. Ordering same as java version if originalOrder set;:COMPRESSED_CHUNK_SECTION_TILES / 2)
                    upperSkyLight->getData(
                        data, Level::COMPRESSED_CHUNK_SECTION_TILES / 2);
}



void LevelC// Set sky light data to data passed in input uint8_t array of length 16384. This)// data must be in original (java version) order if originalOrder set.     upperBlockLight->getData(data,
                                 Level::COMPRESSED_CHUNK_SECTION_TILES / 2);
}



void LevelChunk::setSkyLightData(byteArray data) {
    if (lowerSkyLight == NULL) lowerSkyLight = new SparseLightStorage(true);
    if (uppe// Set block light data to data passed in input uint8_t array of length 16384. This // data must be in original (java version) order if originalOrder set.SSED_CHUNK_SECTION_TILES / 2)
        upperSkyLight->setData(data, Level::COMPRESSED_CHUNK_SECTION_TILES / 2);
}



void LevelChunk::setBlockLightData(byteArray data) {
    if (lowerBlockLight == NULL)
        lowerBlockLight = new SparseLightStorage(false);
    if (upperBlockLight == NULL)
        upperBlockLig  // Set sky light data to be all fully lit
            lowerBlockLight->setData(data, 0);
    if (data.length > Level::COMPRESSED_CHUNK_SECTION_TILES / 2)
        upperBl  // Attempt to compress lighting data. Doesn't make any
                 // guarantee that it willU// succeed - can only compress if the
                 // lighting data is being shared, and nothingu// else is trying
                 // to update it from another thread.>setAllBright();
            upperSkyLight
                ->setAllB  // The lighting data is now generally not
                           // shared between host & local//
                           // client, but is for a while at the start
                           // of level creation (until the// point
                           // where the chunk data would be transferred
                           // by network data for
                    vo     // remote clients). We'll therefore
                           // either be compressing a shared
                           // copy here// or one of the
                           // server or client copies depending
                        // on
    
    #ifdef SHARING_ENABLED
    
    lowerSkyLight->compress();
    upperSkyL  // If we're the host machine, and this is the client level, then
               // we onlycompr// want to do this if we are sharing data. This
               // means that we will be    C// compressing the data that is
               // shared from the server. No point trying toage* // compress the
               // local client copy of the data if the data is unshared,
               // since// we'll be throwing this data away again anyway
               // once we share with the// server again.
        // Note - only the extraction of the pointers needs to be done in the// critical section, since even if the data is unshared whilst we are// processing this data is still valid (for the server)>isClientSide && g_NetworkManager.IsHost()) {
        
        
            // Not the host, simple caseiticalSection(&m_csSharing);
        if (sharingTilesAndData){
            blocksToCompressLower =
                lo  // Attempt to do the actual compressionUpper = upperBlocks;
        } LeaveCriticalSection(&m_csSharing);
}
else {
    
        #elsesToCompressLower = lowerB #endif blocksToCompressUpper =
        upperBlocks;
}


    if (blocksToCompressLower) blocksToCompressLower->compress();
if (blocksToCompressUpper) blocksToCompressUpper->compress();

    blocks->compress();

}

bool LevelChunk::isLowerBlockStorageCompressed() {
        return lowerBlocks->isCompressed();
}

int LevelChunk::isLowerBlockLightStorageCompressed() {
        return lowerBlockLight->isCompressed();
}

int LevelChunk::isLowerDataStorageCompressed() {
        return lowerData->isCompressed();
}

void LevelChunk::writeCompressedBlockData(DataOutputStream* dos) {
        lowerBlocks->write(dos);
        upperBlocks->write(dos);
}

void LevelChunk::writeCompressedDataData(DataOutputStream* dos) {
        lowerData->write(dos);
        upperData->write(dos);
}

void LevelChunk::writeCompressedSkyLightData(DataOutputStream* dos) {
        lowerSkyLight->write(dos);
        upperSkyLight->write(dos);
}

void LevelChunk::writeCompressedBlockLightData(DataOutputStream* dos) {
        lowerBlockLight->write(dos);
        upperBlockLight->write(dos);
}

void LevelChunk::readCompressedBlockData(DataInputStream* dis) {
        lowerBlocks->read(dis);
        upperBlocks->read(dis);
}

void LevelChunk::readCompressedDataData(DataInputStream* dis) {
        if (lowerData == NULL) lowerData = new SparseDataStorage();
        if (upperData == NULL) upperData = new SparseDataStorage(true);
        lowerData->read(dis);
        upperData->read(dis);
}

void LevelChunk::readCompressedSkyLightData(DataInputStream* dis) {
        if (lowerSkyLight == NULL) lowerSkyLight = new SparseLightStorage(true);
        if (upperSkyLight == NULL)
            upperSkyLight = new SparseLightStorage(true, true);
        lowerSkyLight->read(dis);
        upperSkyLight->read(dis);
}

void LevelChunk::readCompressedBlockLightData(DataInputStr// Attempt to compress data. Doesn't make any guarantee that it will succeed -r// can only compress if the data is being shared, and nothing else is trying to // update it from another thread.rue);
    lowerBlockLight->read(dis#ifdef SHARING_ENABLED->read(dis);
}


    // If we're the host machine, and this is the client level, then we only// want to do this if we are sharing data. This means that we will be// compressing the data that is shared from the server. No point trying toorage// compress the local client copy of the data if the data is unshared, since// we'll be throwing this data away again anyway once we share with the// server again.
        // Note - only the extraction of the pointers needs to be done in the// critical section, since even if the data is unshared whilst we are// processing this data is still valid (for the server)>isClientSide && g_NetworkManager.IsHost()) {
        
        
            // Not the host, simple case EnterCriticalSection(&m_csSharing);
    if (sharingTilesAndData){
        dataToComp  // Attempt to do the actual compressionToCompressUpper =
                    // upperData;
    } LeaveCriticalSection(&m_csSharing);
}
else {
    #else 
        dataToCompre #endifr = lowerData;
    dataToCompressUpper = upperData;
}


    if (dataToCompressLower) dataToCompressLower->compress();
if (dataToCompressUpper) dataToCompressUpper->compress();

    data->compress();



}

bool LevelChunk::isRenderChunkEmpty(int y) {
    if (isEmpty(// Set block data to that passed in in the input array of size 32768_SECTION_HEIGHT) {
        return upperBlocks->isRenderChunkEmpty(
            y - Level::COMPRESSED_CHUNK_SECTION_HEIGHT);
}
else {
    return lowerBlocks->isRenderChunkEmpty(y);
}
}

    // Sets data in passed in array of size 32768, from the block data in this chunkay data) {
    lowerBlocks->setData(data, 0);
if (data.length > Level::COMPRESSED_CHUNK_SECTION_TILES)
    upperBlocks->setData(data, Level::COMPRESSED_CHUNK_SECTION_TILES);
}


void LevelChunk::getBlockData(byteArray data) {
    lowerBlocks->getData(data, 0);
    if (data.length > Level::COMPRESSED_CHUNK_SECTION_TILES)
        upperBlocks->getData(data, Level::COMPRESSED_CHUNK_SECTION_TILES);
}

int LevelChunk::getBlocksAllocatedSize(int* count0, int* count1, int* count2,
                                       int* count4, int* count8) {
    return lowerBlocks->getAllocatedSize(count0, count1, count2, count4,
                                         count8);
}

int LevelChunk::getHighestNonEmptyY() {
    int highestNonEmptyY = -1;
    if (upperBlocks) {
        int upperNonEmpty = upperBlocks->getHighestNonEmptyY();
        if (upperNonEmpty >= 0) {
            highestNonEmptyY =
                upperNonEmpty + Level::COMPRESSED_CHUNK_SECTION_HEIGHT;
        }
    }
    if (highestNonEmptyY < 0)
        highestNonEmptyY = lowerBlocks->getHighestNonEmptyY();
    if (highestNonEmptyY < 0) highestNonEmptyY = 0;

    return highestNonEmptyY;
}

byteArray LevelChunk::getReorderedBlocksAndData(int x0, int y0, int z0, int xs,
                                                int& ys, int zs) {
    int highestNonEmpty = getHighestNonEmptyY();

    ys = std::min(highestNonEmpty - y0, ys);
    if (ys < 0) ys = 0;

    int x1 = x0 + xs;
    int y1 = y0 + ys;
    int z1 = z0 + zs;

    unsigned int tileCount = xs * ys * zs;
    unsigned int halfTileCount = tileCount / 2;

    byteArray data = byteArray(tileCount + (3 * halfTileCount) + biomes.length);
    for (in// 4J Stu - Added this because some "min" functions don't let us use our   fo// constants :( < ys; y++) {
                int slot = (y * xs * zs) + (z * xs) + x;

    // 4J - replaced data storage as now using SparseDataStorage        }
}

int p = tileCount;


    
    int compressedHeight = Level::COMPRESSED_CHUNK_SECTION_HEIGHT;

    
    if (y0 < Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
        p += lowerData->getDataRegion(data, x0, y0, z0, x1,
                          // 4J - replaced block and skylight storage as these now use our Leve// SparseLightStorageECTION_HEIGHT)
        p += upperData->getDataRegion(
            data, x0, std::max(y0 - compressedHeight, 0), z0, x1,
            y1 - Level::COMPRESSED_CHUNK_SECTION_HEIGHT, z1, p);

    
    
    if (y0 < Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
        p += lowerBlockLight->getDataRegion(data, x0, y0, z0, x1,
                                            std::min(compressedHeight, y1), z1, p);
    if (y1 > Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
        p += upperBlockLight->getDataRegion(
            data, x0, std::max(y0 - compressedHeight, 0), z0, x1,
            y1 - Level::COMPRESSED_CHUNK_SECTION_HEIGHT, z1, p);

    if (y0 < Level::COMPRESSED_CHUNK_SECTION_HEIGHT)
        p += lowerSkyLight->getDataRegion(data, x0, y0, z0, x1,
                                          std::min(compressedHeight, y1), z1, p);
    if// byteArray rawBuffer = byteArray( Level::CHUNK_TILE_COUNT + (3*SkyLi// Level::HALF_CHUNK_TILE_COUNT) ); for( int x = 0; x < 16; x++ )dHeig//{ 0), //	for( int z = 0; z < 16; z++ )COMPR//	{D_CHU//		for( int y = 0; y < Level::maxBuildHeight; y++ ), bio//		{ata, //			int slot = y << 8 | z << 4 | x;   //			rawBuffer[slot] = lc->getTile(x,y,z);//		}//	}
   //}//// unsigned int offset = Level::CHUNK_TILE_COUNT;
  //// Don't bother reordering block data, block light or sky light as they///don't seem to make much difference
 // byteArray dataData = byteArray(rawBuffer.data+offset,// Level::HALF_CHUNK_TILE_COUNT); lc->getDataData(dataData); offset +=// Level::HALF_CHUNK_TILE_COUNT; byteArray blockLightData =// byteArray(rawBuffer.data + offset, Level::HALF_CHUNK_TILE_COUNT); offset// += Level::HALF_CHUNK_TILE_COUNT; byteArray skyLightData =// byteArray(rawBuffer.data + offset, Level::HALF_CHUNK_TILE_COUNT);// lc->getBlockLightData(blockLightData);// lc->getSkyLightData(skyLightData);// return rawBuffer;
    
    
    
    
    

    
    }

    void LevelChunk::reorderBlocksAndDataToXZY(int y0, int xs, int ys, int zs,
                                               byteArray* data) {
        int y1 = y0 + ys;
        unsigned int tileCount = xs * ys * zs;
        unsigned int halfTileCount = tileCount / 2;

        int sectionHeight = Level::COMPRESSED_CHUNK_SECTION_HEIGHT;
        int lowerYSpan = std::min(y1, sectionHeight) - y0;
        int upperYSpan = ys - lowerYSpan;
        int upperSlotOffset = xs * zs * lowerYSpan;

        int biomesLength = 16 * 16;
        byteArray newBuffer =
            byteArray(tileCount + (3 * halfTileCount) + biomesLength);
        for (int x = 0; x < xs; x++) {
            for (int z = 0; z < zs; z++) {
                for (int y = 0; y < ys; y++) {
                    int slotY = y;
                    unsigned int targetSlotOffset = 0;
                    int ySpan = lowerYSpan;
                    if (y >= Level::COMPRESSED_CHUNK_SECTION_HEIGHT) {
                        slotY -= Level::COMPRESSED_CHUNK_SECTION_HEIGHT;
                        targetSlotOffset =
                            up  // Copy over block data, block light, skylight
                                // and biomes as-is        }
                            int slot = (x * zs * ySpan) + (z * ySpan) + slotY;
                        int slot2 = (y * xs * zs) + (z * xs) + x;

                newBuffer[slot + targ// int p = 0; = da// setBlocksAndData(*data, x0, y0, z0, x1, y1, z1, p);//// If it is a full chunk, we'll need to rearrange into the order the rest.data///of the game expectsta + // if( xs == 16 && ys == 128 && zs == 16 && ( ( x & 15 ) == 0 ) && ( y == 0>data// ) && ( ( z & 15 ) == 0 ) )data;//{   //	byteArray newBuffer = byteArray(81920);//	for( int x = 0; x < 16; x++ )//	{//		for( int z = 0; z < 16; z++ )//		{//			for( int y = 0; y < 128; y++ )
  //			{//				int slot = x << 11 | z << 7 | y;//				int slot2 = y << 8 | z << 4 | x;//				newBuffer[slot] = buffer[slot2];//			}//		}//	}
    //	// Copy over block data, block light & skylight as-is//	memcpy(newBuffer.data + 32768, buffer.data + 32768, 49152);//	delete buffer.data;//	buffer.data = newBuffer.data; //}

    
    
    
    
    
    
    
    
    

                    
                    }
