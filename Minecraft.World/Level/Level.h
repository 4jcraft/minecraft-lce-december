#pragma once

#include "Storage/LevelSource.h"
#include "../Util/LightLayer.h"
#include "ChunkPos.h"
#include "TickNextTickData.h"
#include "Storage/SavedData.h"
#include "../Util/Definitions.h"
#include "../Util/ParticleTypes.h"
#include "../WorldGen/Biomes/Biome.h"
#include "../Util/C4JThread.h"

#ifdef __PSVITA__
#include "../../Minecraft.Client/Platform/PSVita/PSVitaExtras/CustomSet.h"
#endif

// 4J Stu - This value should be big enough that we don't get any crashes causes
// by memory overwrites, however it does seem way too large for what is actually
// needed. Needs further investigation
#define LEVEL_CHUNKS_TO_UPDATE_MAX (19 * 19 * 8)

class Vec3;
class ChunkSource;
class LevelListener;
class Explosion;
class Dimension;
class Material;
class TileEntity;
class AABB;
class Entity;
class SavedData;
class Pos;
class Player;
class LevelData;
class ProgressListener;
class Random;
class LevelStorage;
class SavedDataStorage;
class HitResult;
class Path;
class LevelSettings;
class Biome;
class Villages;
class VillageSiege;
class Tickable;
class Minecart;
class EntitySelector;
class Scoreboard;
class GameRules;

class Level : public LevelSource {
public:
    static const int MAX_TICK_TILES_PER_TICK = 1000;

    // 4J Added
    static const int MAX_GRASS_TICKS = 100;
    static const int MAX_LAVA_TICKS = 100;

public:
    static const int MAX_XBOX_BOATS = 40;  // Max number of boats
    static const int MAX_CONSOLE_MINECARTS = 40;
    static const int MAX_DISPENSABLE_FIREBALLS = 200;
    static const int MAX_DISPENSABLE_PROJECTILES = 300;

    static const int MAX_LEVEL_SIZE = 30000000;
    static const int maxMovementHeight = 512;  // 4J added

    static const int minBuildHeight = 0;    // 4J - brought forward from 1.2.3
    static const int maxBuildHeight = 256;  // 4J - brought forward from 1.2.3
    static const int genDepthBits = 7;
    static const int genDepthBitsPlusFour = genDepthBits + 4;
    static const int genDepth = 1 << genDepthBits;
    static const int genDepthMinusOne = genDepth - 1;
    static const int constSeaLevel = genDepth / 2 - 1;

    static const int CHUNK_TILE_COUNT = maxBuildHeight * 16 * 16;
    static const int HALF_CHUNK_TILE_COUNT = CHUNK_TILE_COUNT / 2;
    static const int COMPRESSED_CHUNK_SECTION_HEIGHT = 128;
    static const int COMPRESSED_CHUNK_SECTION_TILES =
        COMPRESSED_CHUNK_SECTION_HEIGHT * 16 * 16;  // 4J Stu - Fixed size

    int seaLevel;

    // 4J - added, making instaTick flag use TLS so we can set it in the chunk
    // rebuilding thread without upsetting the main game thread
    static DWORD tlsIdx;
    static DWORD tlsIdxLightCache;
    static void enableLightingCache();
    static void destroyLightingCache();
    static bool getCacheTestEnabled();
    static bool getInstaTick();
    static void setInstaTick(bool enable);
    //	bool instaTick;		// 4J - removed

    static const int MAX_BRIGHTNESS = 15;
    static const int TICKS_PER_DAY = 20 * 60 * 20;  // ORG:20*60*20

public:
    CRITICAL_SECTION m_entitiesCS;  // 4J added

    std::vector<std::shared_ptr<Entity> > entities;

protected:
    std::vector<std::shared_ptr<Entity> > entitiesToRemove;

public:
    bool hasEntitiesToRemove();       // 4J added
    bool m_bDisableAddNewTileEnt      // 4J Added
        CRITICAL_SECTION m_tileEntit  // 4J added
            std::vector<std::shared_ptr<TileEntity> >
                tileEntityList;

private:
    std::vector<std::shared_ptr<TileEntity> > pendingTileEntities;
    std::vector<std::shared_ptr<TileEntity> > tileEntitiesToUnload;
    bool updatingTileEntities;

public:
    std::vector<std::shared_ptr<Player> > players;
    std::vector<std::shared_ptr<Entity> > globalEntities;

private:
    int cloudColor;

public:
    int skyDarken;

protected:
    int randValue;

public:
    int addend;

protected:
    float oRainLevel, rainLevel;
    float oThunderLevel, thunderLevel;

public:
    int skyFlashTime;

    int difficulty;
    Random* random;
    bool isNew;
    Dimension* dimension;

protected:
    std::vector<LevelListener*> listeners;
    // 4J - changed to public chunkSource;  // This is the only std::shared_ptr
    // ref to levelStorage - we need to keep// this as long as at least one
    // Level references it, to be able to cope with// moving from dimension
    // to dimension where the Level(Level *level,// Dimension *dimension)
    // ctor is used   
    std::shared_ptr<LevelStorage> levelStorage;

    LevelData* levelData;

public:
    bool isFindingSpawn;
    SavedDataStorage* savedDataStorage;
    std::shared_ptr<Villages> villages;
Vi  // 4J - Calendar is now staticvate:// Calendar *calendar;
    

protected : Scoreboard* scoreboard;

    publ               // 4J - brought forward from 1.2.3 z);
                       // 
        virtual Biome  // 4J Stu - Added these ctors to handle init of member
                       // variables
        void _init();
    void _init(std::shared_ptr<LevelStorage> levelStorage,
               const std::wstring& levelName, LevelSettings* levelSettings,
               Dimension* fixedDimension, bool doCreateChunkSource = true);

public:
    Level(std::shared_ptr<LevelStorage> levelStorage, const std::wstring& name,
          Dimension* dimension, LevelSettings* levelSettings,
          bool doCreateChunkSource = true);
    Level(std::shared_ptr<LevelStorage> levelStorage,
          const std::wstring& levelName, LevelSettings* levelSettings);
    Level(std::shared_ptr<LevelStorage> levelStorage,
          const std::wstring& levelName, LevelSettings* levelSettings,
          Dimension* fixedDimension, bool doCreateChunkSource = true);

    virtual ~Level();

protected:
    virtual ChunkSource* createChunkSource() = 0;

    virtual void initializeLevel(LevelSettings* settings);

public:
    virtual bool All  // 4J Addedleeping() { return false; }  

        virtual void validateSpawn();
    int getTopTile(int x, int z);

public:
    virtual int getTile(int x, int y, int z);
    virtual int getTileLightBlock(int x, int y, int z);
    bool isEmptyTile(int x, int y, int z);
    virtual bool isEntityTile(int x, int y, int z);
    int getTileRenderShape(int x, int y, int // 4J Added to slightly optimise and avoid// getTile call if we already know the tile
    bool hasChunkAt(int x, int y, int z);
    bool hasChunksAt(int x, int y, int z, int r);
    bool hasChunksAt(int x0, int y0, int z0, int x1, int y1, int z1);
    bool reallyHa// 4J addedt x, int y, int z);          
    bool reallyHa// 4J addednt x, int y, int z, int r);  
    bool reallyHasChunksAt(int x0, int y0, int z0, int x1, int y// 4J added                   int z1);  

public:
    bool hasChunk(int x, int z);
// 4J addedallyHasChunk(int x, int z);  

public:
    LevelChunk* getChunkAt(int x, int z);
    LevelChunk* getChunk(int x, int z);
    virtual bool setTileAndData(int x, int y, int z, int tile, int data,
                                int updateFlags);
    Material* getMaterial(int x, int y, int z);
    virtual int getData(int x, int y, int z);
    virtual bool setData(int x, int y, int z, int data, int updateFlags,
             // 4J added forceUpdatepdate = false);  
    virtual bool removeTile(int x, int y, int z);
    virtual bool destroyTile(int x, int y, int z, bool dropResources);
    virtual bool setTileAndUpdate(int x, int y, int z, int tile);
    virtual void sendTileUpdated(int x, int y, int z);

public:
    virtual void tileUpdated(int x, int y, int z, int tile);
    void lightColumnChanged(int x, int z, int y0, int y1);
    void setTileDirty(int x, int y, int z);
    void setTilesDirty(int x0, int y0, int z0, int x1, int y1, int z1);
    void updateNeighborsAt(int x, int y, int z, int tile);
    void updateNeighborsAtExceptFromFacing(int x, int y, int z, int tile,
                                           int skipFacing);
    void neighborChanged(int x, int y, int z, int type);
    virtual bool isTileToBeTickedAt(int x, int y, int z, int tileId);
    bool canSeeSky(int x, int y, int z);
    int getDaytimeRawBrightness(int x, int y, int z);
    int getRawBrightness(int x, int y, int z);
    int getRawBrightness(int x, int y, int z, bool propagate);
    bool isSkyLit(int x, int y, int z);
    int getHeightmap(int x, int z);
    int getLowestHeightmap(int x, int z);
    void updateLightIfOtherThan(LightLayer::variety layer, int x, int y, int z,
                                int expected);
    int getBrightnessPropagate(LightLayer::variety layer, int x, int y, int z,
     // 4J added tileId        int tileId);  
    void getNeighbourBrightnesses(int* brightnesses, LightLayer::variety layer,
                 // 4J added      int x, int y, int z);  
    int getBrightness(LightLayer::variety layer, int x, int y, int z);
    void setBrightness(
        LightLayer::variety layer, int x, int y, int z, int brightness,
 // 4J added noUpdateOnClient = false);  
    void setBrightnessNoUpdateOnClient(LightLayer::variety layer, int x, int y,
    // 4J added
#ifdef _LARGE_WORLDSess);  
#else typedef __uint64 lightCache_t;
#endifypedef unsigned int lightCache_t;

    inline void setBrightnessCached(lightCache_t* cache, __uint64* cacheUse,
                                    LightLayer::variety layer, int x, int y,
                                    int z, int brightness);
    inline int getBrightnessCached(lightCache_t* cache,
                                   LightLayer::variety layer, int x, int y,
                                   int z);
    inline int getEmissionCached(lightCache_t* cache, int ct, int x, int y,
                                 int z);
    inline int getBlockingCached(lightCache_t* cache, LightLayer::variety layer,
                                 int* ct, int x, int y, int z);
    void initCachePartial(lightCache_t* cache, int xc, int yc, int zc);
    void initCacheComplete(lightCache_t* cache, int xc, int yc, int zc);
    void flushCache(lightCache_t* cache, __uint64 cacheUse,
                    LightLayer::variety layer);

    bool cachewritten;
    static const int LIGHTING_SHIFT = 24;
    static const int BLOCKING_SHIFT = 20;
#ifdef _LARGE_WORLDSMISSION_SHIFT = 16;

    static const __int64 LIGHTING_WRITEBACK = 0x80000000LL;
    static const __int64 EMISSION_VALID = 0x40000000LL;
    static const __int64 BLOCKING_VALID = 0x20000000LL;
    static const __int64 LIGHTING_VALID = 0x10000000LL;
    static const lightCache_#elseITION_MASK = 0xffffffff0000ffffLL;

    static const int LIGHTING_WRITEBACK = 0x80000000;
    static const int EMISSION_VALID = 0x40000000;
    static const int BLOCKING_VALID = 0x20000000;
    static const int LIGHTING_VALID = 0x10000000;
    static const l#endifche_t POSITION_MASK = 0x0000ffff;


    int cacheminx, cachemaxx, cacheminy, cachemaxy, cacheminz, cachemaxz;
    void setTileBrightnessChanged(int x, int y, int z);
    virtual int getLightColor(
        int x, int y, int z// 4J - brought forward from 1.8.2-1);  
    virtual float getBrightness(int x, int y, int z, int emitt);
    virtual float getBrightness(int x, int y, int z);
    bool isDay();
    HitResult* clip(Vec3* a, Vec3* b);
    HitResult* clip(Vec3* a, Vec3* b, bool liquid);
    HitResult* clip(Vec3* a, Vec3* b, bool liquid, bool solidOnly);

    virtual void playEntitySound(std::shared_ptr<Entity> entity, int iSound,
                                 float volume, float pitch);
    virtual void playPlayerSound(std::shared_ptr<Player> entity, int iSound,
                                 float volume, float pitch);
    virtual void playSound(double x, double y, double z, int iSound,
                           float volume, float pitch,
                           float fClipSoundDist = 16.0f);

    virtual void playLocalSound(double x, double y, double z, int iSound,
                                float volume, float pitch, bool distanceDelay,
                                float fClipSoundDist = 16.0f);

    void playStreamingMusic(const std::wstring& name, int x, int y, int z);
    void playMusic(double x, double y, double z,
                   const std::w// 4J removed - void addParticle(const std::wstring& id, double x, double y,// double z, double xd, double yd, double zd);
    void addParticle(ePARTICLE_TYPE id, double x, double y, double z, double xd,
     // 4J added     double yd, double zd);  
    virtual bool addGlobalEntity(std::shared_ptr<Entity> e);
    virtual bool addEntity(std::shared_ptr<Entity> e);

protected:
    virtual void entityAdded(std::shared_ptr<Entity> e);
    virtual void entityRemoved(std::shared_ptr<Entity> e);
    virtual void pla// 4J addedstd::shared_ptr<Entity> e);  

public:
    virtual void removeEntity(std::shared_ptr<Entity> e);
    void removeEntityImmediately(std::shared_ptr<Entity> e);
    void addListener(LevelListener* listener);
    void removeListener(LevelListener* listener);

private:
    AABBList boxes;

public:
    AABBList* getCubes(
        std::shared_ptr<Entity> source, AABB* box, bool noEntities = false,
        // 4J: Added noEntities & blockAtEdge parameters
    AABBList* getTileCubes(
        AABB* box, bool blockA// 4J: Added noEntities & blockAtEdge parameters// 4J - change brought forward from 1.8.2// 4J - change brought forward from 1.8.2
    Vec3* getSkyColor(std::shared_ptr<Entity> source, float a);
    float getTimeOfDay(float a);
    int getMoonPhase();
    float getMoonBrightness();
    float getSunAngle(float a);
    Vec3* getCloudColor(float a);
    Vec3* getFogColor(float a);
    int getTopRainBlock(int x, int z);
    int getTopSolidBlock(int x, int z)// 4J addedbiomeHasRain(int x, int z);  // 4J addedbiomeHasSnow(int x, int z);  
    int getLightDepth(int x, int z);
    float getStarBrightness(float a);
    virtual void addToTickNextTick(int x, int y, int z, int tileId,
                                   int tickDelay);
    virtual void addToTickNextTick(int x, int y, int z, int tileId,
                                   int tickDelay, int priorityTilt);
    virtual void forceAddTileTick(int x, int y, int z, int tileId,
                                  int tickDelay, int prioTilt);
    virtual void tickEntities();
    void addAllPendingTileEntities(
        std::vector<std::shared_ptr<TileEntity> >& entities);
    void tick(std::shared_ptr<Entity> e);
    virtual void tick(std::shared_ptr<Entity> e, bool actual);
    bool isUnobstructed(AABB* aabb);
    bool isUnobstructed(AABB* aabb, std::shared_ptr<Entity> ignore);
    bool containsAnyBlocks(AABB* box);
    bool containsAnyLiquid(AABB* box);
  // 4J addedainsAnyLiquid_NoLoad(AABB* box);  
    bool containsFireTile(AABB* box);
    bool checkAndHandleWater(AABB* box, Material* material,
                             std::shared_ptr<Entity> e);
    bool containsMaterial(AABB* box, Material* material);
    bool contai// 4J Stu - destroyBlocks param brought forward as part of fix for tnt// cannons
    
    std::shared_ptr<Explosion> explode(std::shared_ptr<Entity> source, double x,
                                       double y, double z, float r,
                                       bool destroyBlocks);
    virtual std::shared_ptr<Explosion> explode(std::shared_ptr<Entity> source,
                                               double x, double y, double z,
                                               float r, bool fire,
                                               bool destroyBlocks);
    float getSeenPercent(Vec3* center, AABB* bb);
    bool extinguishFire(std::shared_ptr<Player> player, int x, int y, int z,
                        int face);
    std::wstring gatherStats();
    std::wstring gatherChunkSourceStats();
    virtual std::shared_ptr<TileEntity> getTileEntity(int x, int y, int z);
    void setTileEntity(int x, int y, int z,
                       std::shared_ptr<TileEntity> tileEntity);
    void removeTileEntity(int x, int y, int z);
    void markForRemoval(std::shared_ptr<TileEntity> entity);
    virtual bool isSolidRenderTile(int x, int y, int z);
    virtual bool isSolidBlockingTile(int x, int y, int z);
    bool isSolidBlockingTileInLoadedChunk(int x, int y, int z,
                                          bool valueIfNotLoaded);
    bool isFullAABBTile(int x, int y, int z);
    virtual bool isTopSolidBlocking(int x, int y,
// 4J - brought forward from 1.3.2  int z);  
    bool isTopSolidBlocking(Tile* tile, int data);

protected:
    bool spawnEne// int xxo, yyo, zzo;riendlies;

public:
    

    void updateSkyBrightness();
    void setSpawnSettings(bool spawnEnemies, bool spawnFriendlies);
    virtual void tick();

private:
    void prepareWeather();

protected:
    virtual void tickWeather();

private:
    void stopWeather();

pu#ifdef __PSVITA__ggleD// AP - See CustomSet.h for an explanation of this#else
    CustomSet chunksToPoll;

    std::unordered_set<ChunkP#endifunkPosKeyHash, ChunkPosKeyEq> chunksToPoll;


private:
    int delayUntilNextMoodSound;
    static const int CHUNK_POLL_RANGE = 9;
    static const int CHUNK_TILE_TICK_COUNT = 80;
    static const int CHUNK_SECTION_TILE_TICK_COUNT =
        (CHUNK_TILE_TICK_COUNT / 8) + 1;

protected:
    virtual void buildAndPrepareChunksToPoll();
    virtual void tickClientSideTiles(int xo, int zo, Le// 4J - snow & ice checks brought forward from 1.2.3
public:
    bool shouldFreezeIgnoreNeighbors(int x, int y, int z);
    bool shouldFreeze(int x, int y, int z);
    bool shouldFreeze(int x, int y, int z, bool checkNeighbors);
    bool shouldSnow(int x, int y, int z);
    void checkLight(int x, int y, int z, bool force = false,
                    bool ro// 4J added force, rootOnlySource parametersse);  
private:
    int* toCheckLevel;
    int getExpectedLight(lightCache_t* cache, int x, int y, int z,
                         LightLayer::variety layer, bool propagatedOnly);

public:
    void checkLight(LightLayer::variety layer, int xc, int yc, int zc,
                    bool force = false,
                    bool ro// 4J added force, rootOnlySource parametersse);  

public:
    virtual bool tickPendingTicks(bool force);
    virtual std::vector<TickNextTickData>* fetchTicksInChunk(LevelChunk* chunk,
                                                        bool remove);

private:
    std::vector<std::shared_ptr<Entity> > es;

public:
    bool isClientSide;

    std::vector<std::shared_ptr<Entity> >* getEntities(
        std::shared_ptr<Entity> except, AABB* bb);
    std::vector<std::shared_ptr<Entity> >* getEntities(
        std::shared_ptr<Entity> except, AABB* bb,
        const EntitySelector* selector);
    std::vector<std::shared_ptr<Entity> >* getEntitiesOfClass(
        const std::type_info& baseClass, AABB* bb);
    std::vector<std::shared_ptr<Entity> >* getEntitiesOfClass(
        const std::type_info& baseClass, AABB* bb,
        const EntitySelector* selector);
    std::shared_ptr<Entity> getClosestEntityOfClass(
        const std::type_info& baseClass, AABB* bb,
        std::shared_ptr<Entity> source);
    virtual std::shared_ptr<Entity> getEntity(int entityId) = 0;
    std::vector<std::shared_ptr<Entity> > getAllEntities();
//	unsigned int countInstanceOf(BaseObject::Class *clas);ed_ptr<TileEntity> te);
    
    unsigned int countInstanceOf(
        eINSTANCEOF clas, bool singleType, u// 4J added* protectedCount = NULL,
        unsigned int* couldWanderCount = NULL);  
    unsigned int countInstanceOfInRange(eINSTANCEOF clas, bool singleType,
                             // 4J Addedint range, int x, int y,
                                        int z);  
    void addEntities(std::vector<std::shared_ptr<Entity> >* std::list);
    virtual void removeEntities(std::vector<std::shared_ptr<Entity> >* std::list);
    bool mayPlace(int tileId, int x, int y, int z, bool ignoreEntities,
                  int face, std::shared_ptr<Entity> ignoreEntity,
                  std::shared_ptr<ItemInstance> item);
    int getSeaLevel();
    Path* findPath(std::shared_ptr<Entity> from, std::shared_ptr<Entity> to,
                   float maxDist, bool canPassDoors, bool canOpenDoors,
                   bool avoidWater, bool canFloat);
    Path* findPath(std::shared_ptr<Entity> from, int xBest, int yBest,
                   int zBest, float maxDist, bool canPassDoors,
                   bool canOpenDoors, bool avoidWater, bool canFloat);
    int getDirectSignal(int x, int y, int z, int dir);
    int getDirectSignalTo(int x, int y, int z);
    bool hasSignal(int x, int y, int z, int dir);
    int getSignal(int x, int y, int z, int dir);
// 4J Added maxYDist param(int x, int y, int z);
    int getBestNeighborSignal(int x, int y, int z);
    
    std::shared_ptr<Player> getNearestPlayer(std::shared_ptr<Entity> source,
                                             double maxDist,
                                             double maxYDist = -1);
    std::shared_ptr<Player> getNearestPlayer(double x, double y, double z,
                                             double maxDist,
                                             double maxYDist = -1);
    std::shared_ptr<Player> getNearestPlayer(double x, double z,
                                             double maxDist);
    std::shared_ptr<Player> getNearestAttackablePlayer(
        std::shared_ptr<Entity> source, double maxDist);
    std::shared_ptr<Player> getNearestAttackablePlayer(double x, double y,
                                                       double z,
                                                       double maxDist);

    std::shared_ptr<Player> getPlayerByName(con// 4J Addedring& name);
    std::shared_ptr<Player> getPlayerByUUID(
        const std::wstring& name);  
    byteArray getBlocksAndData(int x, int y, int z, int xs, int ys, int zs,
                               bool includeLighting = true);
    void setBlocksAndData(int x, int y, int z, int xs, int ys, int zs,
                          byteArray data, bool includeLighting = true);
    virtual void disconnect(bool sendDisconnect = true);
    void checkSession();
    void setGameTime(__int64 time);
    __int64 getSeed();
    __int64 getGameTime();
    __int64 getDayTime();
    void setDayTime(__int64 newTime);
    Pos* getSharedSpawnPos();
    void setSpawnPos(int x, int y, int z);
    void setSpawnPos(Pos* spawnPos);
    void ensureAdded(std::shared_ptr<Entity> entity);
    virtual bool mayInteract(std::shared_ptr<Player> player, int xt, int yt,
                             int zt, int content);
    virtual void broadcastEntityEvent(std::shared_ptr<Entity> e, uint8_t event);
    ChunkSource* getChunkSource();
    virtual void tileEvent(int x, int y, int z, int tile, int b0, int b1);
    LevelStorage* getLevelStorage();
    LevelData* getLevelData();
    Ga// 4J addedtGameRules();
    virtual void updat// 4J AddedayerList();
    bool useNewSeaLevel(// 4J Added
    bool getHasBeenInCreative();   
    bool isGenerateMapFeatures();  
    int getSaveVersion();
    int getOriginalSaveVersion();
    float getThunderLevel(float a);
    float getRainLevel(float a);
    void setRainLevel(float rainLevel);
    bool isThundering();
    bool isRaining();
    bool isRainingAt(int x, int y, int z);
    bool isHumidAt(int x, int y, int z);
    void setSavedData(const std::wstring& id, std::shared_ptr<SavedData> data);
    std::shared_ptr<SavedData> getSavedData(const std::type_info& clazz,
                                            const std::wstring& id);
    int getFreeAuxValueFor(const std::wstring& id);
    void globalLevelEvent(int type, int sourceX, int sourceY, int sourceZ,
                          int data);
    void levelEvent(int type, int x, int y, int z, int data);
    void levelEvent(std::shared_ptr<Player> source, int type, int x, int y,
                    int z, int data);
    int getMaxBuildHeight();
    int getHeight();
    virtual Tickable* makeSoundUpdater(std::shared_ptr<Minecart> minecart);
    Random* getRandomFor(int x, int z, int blend);
    virtual bool isAllEmpty();
   //  Calendar *getCalendar(); // 4J - Calendar is now statict id, int x, int y, int z, int progress);
    
    virtual void createFireworks(double x, double y, double z, double xd,
                                 double yd, double zd, CompoundTag* infoTag);
    virtual Scoreboard* getScoreboard();
    virtual void updateNeighbourForOutputSignal(int x, int y, int z,
                                                int source);
    virtual float getDifficulty(double x, double y, double z);
    virtual float getDifficulty(int x, int y, int z);
    TilePos* findNearestM// 4J Addedonst std::wstring& featureName, int x,
                                   int y, int z);

    
    int getAuxValueForM// 4J - optimisation - keep direct reference of underlying cache here     int centreZC, int scale);

    // 4J - added for implementation of finite limit to number of item entities, int // tnt and falling block entities
    
public:
    virtual bo// 4J - added for new lighting from 1.8.2
    virtual bool newFallingTileAllowed() {
        return true; }

    // 4J-PB - for the end portal in The EndCRITICAL_SECTION m_checkLightCS;

private:
    int m_iHighestY;  
public:
    int GetHighestY() {
        return m_iHighestY; }
 // 4J addedHighestY(int iVal) { m_iHighestY = iVal; }

    boo// 4J addednalised(int x, int z);          
    bool isChunkPostPostProcessed(int x, int z);  

private:
    i// 4J AddeddChunkCount;

public:
    int getUnsavedC// 4J Added;
    void incrementUnsavedChunkCount();  
    void decrementUnsavedChunkCount();  

    enum ESPAWN_TYPE {
        eSpawnType_Egg,
        eSpawnType_Breed,
        eSpawnType_Portal,
    };

    bool canCreateMore(eINSTANCEOF type, ESPAWN_TYPE spawnType);
};
