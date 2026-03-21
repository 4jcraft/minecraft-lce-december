#pragma once

class DataLayer;
class TileEntity;
class Random;
class ChunkSource;
class EntitySelector;

#include "Storage/SparseLightStorage.h"
#include "Storage/CompressedTileStorage.h"
#include "Storage/SparseDataStorage.h"

#include "../Util/LightLayer.h"
#include "../Entities/Entity.h"
#include "Level.h"

#define SHARING_ENABLED
class TileCompressData_SPU;

#if 0  //__PSVITA__
#define _ENTITIES_RW_SECTION
#endif

class LevelChunk {
    friend class TileCompressData_SPU;
    friend class LevelRenderer;

public:
    byteArray biomes;  // 4J Stu - Made public

    // 4J Stu - No longer static in 1.8.2
    const int ENTITY_BLOCKS_LENGTH;
    static const int BLOCKS_LENGTH = Level::CHUNK_TILE_COUNT;  // 4J added

    static bool touchedSky;

    enum EColumnFlag {
        eColumnFlag_recheck = 1,
        eColumnFlag_biomeOk = 2,
        eColumnFlag_biomeHasSnow = 4,
        eColumnFlag_biomeHasRain = 8,
    };

    //    byteArray blocks;
    // 4J - actual storage for blocks is now private with public methods to
    // access it
private:
    CompressedTileStorage* lowerBlocks;  // 0 - 127
    CompressedTileStorage* upperBlocks;  // 128 - 255
public:
    bool isRenderChunkEmpty(int y);
    void setBlockData(byteArray data);  // Set block data to that passed in in
                                        // the input array of size 32768
    void getBlockData(
        byteArray data);  // Sets data in passed in array of size 32768, from
                          // the block data in this chunk
    int getBlocksAllocatedSize(int* count0, int* count1, int* count2,
                               int* count4, int* count8);

    bool loaded;
    unsigned char
        rainHeights[16 * 16];  // 4J - optimisation brought forward from 1.8.2
                               // (was int arrayb in java though)
    unsigned char columnFlags[16 * 8];  // 4J - lighting update brought forward
                                        // from 1.8.2, was a bool array but now
                                        // mixed with other flags in our
                                        // version, and stored in nybbles
    Level* level;

    // 4J - actual storage for data is now private with public methods to access
    // it
private:
    SparseDataStorage* lowerData;  // 0 - 127
    SparseDataStorage* upperData;  // 128 - 255
public:
    void setDataData(byteArray data);  // Set data to that passed in in the
                                       // input array of size 32768
    void getDataData(byteArray data);  // Sets data in passed in array of size
                                       // 16384, from the data in this chunk

    //    DataLayer *data;
private:
    // 4J - actual storage for sky & block lights is now private with new
    // methods to be able to access it.

    SparseLightStorage* lowerSkyLight;    // 0 - 127
    SparseLightStorage* upperSkyLight;    // 128 - 255
    SparseLightStorage* lowerBlockLight;  // 0 - 127
    SparseLightStorage* upperBlockLight;  // 128 - 255
public:
    void getSkyLightData(
        byteArray data);  // Get a uint8_t array of length 16384 ( 128 x 16 x 16
                          // x 0.5 ), containing sky light data. Ordering same
                          // as java version.
    void getBlockLightData(
        byteArray data);  // Get a uint8_t array of length 16384 ( 128 x 16 x 16
                          // x 0.5 ), containing block light data. Ordering same
                          // as java version.
    void setSkyLightData(
        byteArray data);  // Set sky light data to data passed in input uint8_t
                          // array of length 16384. This data must be in
                          // original (java version) order
    void setBlockLightData(
        byteArray data);  // Set block light data to data passed in input
                          // uint8_t array of length 16384. This data must be in
                          // original (java version) order
    void setSkyLightDataAllBright();  // Set sky light data to be all fully lit
    bool isLowerBlockStorageCompressed();
    int isLowerBlockLightStorageCompressed();
    int isLowerDataStorageCompressed();

    void writeCompressedBlockData(DataOutputStream* dos);
    void writeCompressedDataData(DataOutputStream* dos);
    void writeCompressedSkyLightData(DataOutputStream* dos);
    void writeCompressedBlockLightData(DataOutputStream* dos);

    void readCompressedBlockData(DataInputStream* dis);
    void readCompressedDataData(DataInputStream* dis);
    void readCompressedSkyLightData(DataInputStream* dis);
    void readCompressedBlockLightData(DataInputStream* dis);

    byteArray heightmap;
    int minHeight;
    int x, z;

private:
    bool hasGapsToCheck;

public:
    std::unordered_map<TilePos, std::shared_ptr<TileEntity>, TilePosKeyHash,
                       TilePosKeyEq>
        tileEntities;
    std::vector<std::shared_ptr<Entity> >** entityBlocks;

    static const int sTerrainPopulatedFromHere = 2;
    static const int sTerrainPopulatedFromW = 4;
    static const int sTerrainPopulatedFromS = 8;
    static const int sTerrainPopulatedFromSW = 16;
    static const int sTerrainPopulatedAllAffec  // All the post-processing that
                                                // can actually place tiles in
                                                // this// chunk are complete
        static const int sTerrainPopulatedFromNW = 32;
    static const int sTerrainPopulatedFromN = 64;
    static const int sTerrainPopulatedFromNE = 128;
    static const int sTerrainPopulatedFromE = 256;
    static const int sTerrainPopulatedFromSE = 512;
    static const int sTerrainPopulatedAllNeighbou  // The post-processing passes
                                                   // of all neighbours to this
                                                   // chunk are// complete
                                                   //
        static const int
            sTerrainPostPostProcess  // This chunk has been post-post-processed,
                                     // which is only done//
                                     // when all neighbours have been
                                     // post-processed

        short         // 4J - changed from bool to bitfield within
                      // short
        short* serve  // 4J addedulated;

        void setUnsa  // 4J addedsaved);  // 4J Stu - Stopped this being
                      // private so we can add some more logic to
                      // it
        bool m_unsaved;

public:
    bool dontSave;
    bool l #ifdef SHARING_ENABLED bool sh  // 4J addedn#endif

        bool emissiveAdded;      // 4J added
    void stopSharingTilesAndData(// 4J added         
    virtual void reSyncLighting()// 4J added         
    void startSharingTilesAndData// 4J addeds = 0);  
    __int64 lastUnsharedTime;    // 4J added         
    __int64 lastSaveTime;
    bool seenByPlayer;
    int lowestHeightmap;
    __i
#ifdef _LARGE_WORLDS
    bool m_bUnloaded;
    CompoundTag* m_u#endifdEntit// static const int LIGHT_CHECK_MAX_POS = NUM_SECTIONS * 16 * 16;
private:
    int checkLightPosition;

public:
    virtual void init(Level* level, int x, int z);
    LevelChunk(Level* level, int x, int z);
    LevelChunk(Level* level, byteArray blocks, int x, int z);
    LevelChunk(Level* level, int x, int z, LevelChunk* lc);
    ~LevelChunk();

    virtual bool isAt(int x, int z);

    virtual int getHeightmap(int x, int z);
    int getHighestSectionPosition();
    virtual void recalcBlockLights();

    virtual void recalcHeightmapOnly();

    virtual void recalcHeightmap();

    virtual void lightLava();

private:
    void lightGaps// 4J - changes for lighting brought forward from 1.8.2
public:
    void recheckGaps(boo// 4J - added parameter, made public
private:
    void lightGap(int x, int z, int source);
    void lightGap(int x, int z, int y1, int y2);

    void recalcHeight(int x, int yStart, int z);

public:
    virtual int getTileLightBlock(int x, int y, int z);
    virtual int getTile(int x, int y, int z);
    virtual bool setTileAndData(int x, int y, int z, int _tile, int _data);
    virtual bool setTile(int x, int y, int z, int _tile);
    virtual int getData(int x, int y, int z);
    virtual bool setData(int x, int y, int z, int val, int mask,
                         bool* m// 4J added mask);  
    virtual int getBrightness(LightLayer::variety layer, int x, int y, int z);
    virtual void getNeighbourBrightnesses(int* brightnesses,
                                          LightLayer::variety layer, int x,
                                      // 4J addedint z);  
    virtual void setBrightness(LightLayer::variety layer, int x, int y, int z,
                               int brightness);
    virtual int getRawBrightness(int x, int y, int z, int skyDampen);
    virtual void addEntity(std::shared_ptr<Entity> e);
    virtual void removeEntity(std::shared_ptr<Entity> e);
    virtual void removeEntity(std::shared_ptr<Entity> e, int yc);
    virtual bool isSkyLit(int x, int y, int z);
    virtual void skyBrightnessChanged();
    virtual std::shared_ptr<TileEntity> getTileEntity(int x, int y, int z);
    virtual void addTileEntity(std::shared_ptr<TileEntity> te);
    virtual void setTileEntity(int x, int y, int z,
                               std::shared_ptr<TileEntity> tileEntity);
    virtual void removeTileEntity(int x, int y, int z);
    virtual void load();
    // 4J - added parameterbool unloadTileEntities);
    // 4J - addedo#ifdef _LARGE_WORLDS
#endif
    virtual bool isUnloaded();

    virtual void markUnsaved();
    virtual void getEntities(std::shared_ptr<Entity> except, AABB* bb,
                             std::vector<std::shared_ptr<Entity> >& es,
                             const EntitySelector* selector);
    virtual void getEntitiesOfClass(const std::type_info& ec, AABB* bb,
                                    std::vector<std::shared_ptr<Entity> >& es,
                                    const EntitySelector* selector);
    virtual int countEntities();
    virtual bool shouldSave(bool force);
    virtual int getBlocksAndData(
        byteArray* data, int x0, int y0, int// 4J - added includeLighting parameter     bool includeLighting = true);  
    static void tileUpdatedCallback(int x, in// 4J added void* param,
                                    int yparam);  
    virtual int setBlocksAndData(
        byteArray data, int x0, int y0, int// 4J - added includeLighting parameter     bool includeLighting = true);  
    virtual bool testSetBlocksAndData(byteArray data, int x0, int y0, int z0,
                                  // 4J added int y1, int z1,
                                      int p);  
    virtual void setCheckAllLight();

    virtual Random* getRandom(__in
#ifdef SHARING_ENABLEDl isEmpty();
    virtual void attemptCompre// 4J added#endif// 4J  added #ifdef _ENTITIES_RW_SECTIONcsSharing;

    // AP - we're using a RW critical so we can do multiple     m_csEntities; //
    // reads without contention#else
#endif

    static CRITICAL_S// 4J  addedntities;

    static CRITICAL_SECTION m_csTileEntities;  
    static void staticCtor();
    void checkPostProcess(ChunkSource* source, ChunkSource* parent, int x,
                 // 4J addedt z);
    void checkChests(ChunkSource* source, int x, int z);  // 4J - optimisation brought forward from 1.8.2                   // 4J - lighting change brought forward from 1.8.2
    void tick();  
    ChunkPos* g// 4J added  bool isYSpaceEmpty(int y1, int y2);
    void reloadBiomes();  
    virtual Biome* getBiome(int x, int z, BiomeSource* biomeSource);
    byteArray getBiomes();
 // 4J addedBiomes(byteArray biomes);
    bool biom// 4J addedt x, int z);  
    bool biomeHasSnow(int // 4J added 
private:
    void updateBi// 4J addedt x, int z);  
publi// 4J added compressLighting();  // 4J added compressBlocks();    
    void compressData();      
    int getHighestNonEmptyY();
    byteArray getReorderedBlocksAndData(int x, int y, int z, int xs, int& ys,
                                        int zs);
    static void reorderBlocksAndDataToXZY(int y0, int xs, i#ifdef LIGHT_COMPRESSION_STATS                           byteArray* data);

    int getBlockLightPlanesLower() {
        return lowerBlockLight->count; }
    int getSkyLightPlanesLower() {
        return lowerSkyLight->count; }
    int getBlockLightPlanesUpper() {
        return upperBlockLight->#endif #ifdef DATA_COMPRESSION_STATSper() {
            return upperSkyLight->count;
        }

#endif
        int getDataPlanes() { return data->count; }
        

}
;
