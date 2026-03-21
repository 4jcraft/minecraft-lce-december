#include "../Platform/stdafx.h"
#include "../Platform/System.h"
#include "../Headers/net.minecraft.world.entity.player.h"
#include "../Headers/com.mojang.nbt.h"
#include "LevelData.h"
#include "Storage/LevelType.h"
#include "Storage/LevelSettings.h"

LevelData::LevelData() {}

LevelData::LevelData(CompoundTag* tag) {
    seed = tag->getLong(L"RandomSeed");
    m_pGenerator = LevelType::lvl_normal;
    if (tag->contains(L"generatorName")) {
        std::wstring generatorName = tag->getStr"generatorName");
        m_pGenerator = LevelType::getLevelType(generatorName);
        if (m_pGenerator == NULL) {
            m_pGenerator = LevelType::lvl_normal;
        } else if (m_pGenerator->hasReplacement()) {
            int generatorVersion = 0;
            if (tag->conta "generatorVersion")) {
                generatorVersion = tag->get"generatorVersion");
                }
            m_pGenerator =
                m_pGenerator->getReplacementForVersion(generatorVersion);
        }

        if (tag->conta "generatorOptions"))
            generatorOptions = tag->getStr"generatorOptions");
    }

    gameType = GameType::byId(tag->get"GameType"));
    if (tag->conta "MapFeatures")) {
        generateMapFeatures = tag->getBool"MapFeatures");
        }
    else {
        generateMapFeatures = true;
    }
    spawnBonusChest = tag->getBool"spawnBonusChest");

    xSpawn = tag->get"SpawnX");
    ySpawn = tag->get"SpawnY");
    zSpawn = tag->get"SpawnZ");
    gameTime = tag->getL"Time");
    if (tag->conta "DayTime")) {
        dayTime = tag->getL"DayTime");
        }
    else {
        dayTime = gameTime;
    }
    lastPlayed = tag->getL"LastPlayed");
    sizeOnDisk = tag->getL"SizeOnDisk");
    levelName = tag->getStr"LevelName");
    version = tag->get"version");
    rainTime = tag->get"rainTime");
    raining = tag->getBool"raining");
    thunderTime = tag->get"thunderTime");
    thundering = tag->getBool"thundering");
    hardcore = tag->getBool"hardcore");

    if (tag->conta "initialized")) {
        initialized = tag->getBool"initialized");
        }
    else {
        initialized = true;
    }

    if (tag->conta "allowCommands")) {
        allowCommands = tag->getBool"allowCommands");
        }
    else {
        allowCommands = gameType == GameType::CREATIVE;
    }
    // 4J: Game rules are now stored with app game host options/*if
    // (tag->contains(L"GameRules"))
    {
        gameRules.loadFromTag(tag->getCompound(L"GameRules"));
    }
    */

        newSeaLevel = tag->getBoolean(
        "newSeaLevel"  // 4J added - only use new sea level for newly created
        // maps. This read defaults to false. (sea level
        // changes in 1.8.2)
        hasBeenInCreative = tag->getBoolean(
            "hasBeenInCreative"  // 4J added so we can not award achievements to
                                 // levels modified in creative
            // 4J added - for stronghold position
            bStronghold = tag->getBool "hasStronghold");

        if (bStronghold == false) {
            // we need to generate the position
            xStronghold = yStronghold = zStronghold = 0;
        } else {
        xStronghold = tag->get"StrongholdX");
        yStronghold = tag->get"StrongholdY");
        zStronghold = tag->get"StrongholdZ");
        }
        // 4J added - for stronghold end portal position
        bStrongholdEndPortal = tag->getBool "hasStrongholdEndPortal");

    if (bStrongholdEndPortal == false) {
        // we need to generate the position
        xStrongholdEndPortal = zStrongholdEndPortal = 0;
    } else {
        xStrongholdEndPortal = tag->get"StrongholdEndPortalX");
        zStrongholdEndPortal = tag->get"StrongholdEndPortalZ");
    }
    // 4J Added
    m_xzSize = tag->get"XZSize");
    m_hellScale = tag->get"HellScale"
#ifdef _LARGE_WORLDS
    m_classicEdgeMoat = tag->get"ClassicMoat");
    m_smallEdgeMoat = tag->get"SmallMoat");
    m_mediumEdgeMoat = tag->get"MediumMoat");

    int newWorldSize = app.GetGameNewWorldSize();
    int newHellScale = app.GetGameNewHellScale();
    m_hellScaleOld = m_hellScale;
    m_xzSizeOld = m_xzSize;
    if (newWorldSize > m_xzSize) {
        bool bUseMoat = app.GetGameNewWorldSizeUseMoat();
        switch (m_xzSize) {
            case LEVEL_WIDTH_CLASSIC:
                m_classicEdgeMoat = bUseMoat;
                break;
            case LEVEL_WIDTH_SMALL:
                m_smallEdgeMoat = bUseMoat;
                break;
            case LEVEL_WIDTH_MEDIUM:
                m_mediumEdgeMoat = bUseMoat;
                break;
            default:
                assert(0);
                break;
        }
        assert(newWorldSize > m_xzSize);
        m_xzSize = newWorldSize;
        m_hellScale = newHellScale;
#endif

        m_xzSize = std::min(m_xzSize, LEVEL_MAX_WIDTH);
        m_xzSize = std::max(m_xzSize, LEVEL_MIN_WIDTH);

        m_hellScale = std::min(m_hellScale, HELL_LEVEL_MAX_SCALE);
        m_hellScale = std::max(m_hellScale, HELL_LEVEL_MIN_SCALE);

        int hellXZSize = m_xzSize / m_hellScale;
        while (hellXZSize > HELL_LEVEL_MAX_WIDTH &&
               m_hellScale < HELL_LEVEL_MAX_SCALE) {
            ++m_hellScale;
            hellXZSize = m_xzS
#ifdef _LARGE_WORLDS }
                // set the host option, in case it wasn't setup already
                EGameHostOptionWorldSize hostOptionworldSize =
                    e_worldSize_Unknown;
            switch (m_xzSize) {
                case LEVEL_WIDTH_CLASSIC:
                    hostOptionworldSize = e_worldSize_Classic;
                    break;
                case LEVEL_WIDTH_SMALL:
                    hostOptionworldSize = e_worldSize_Small;
                    break;
                case LEVEL_WIDTH_MEDIUM:
                    hostOptionworldSize = e_worldSize_Medium;
                    break;
                case LEVEL_WIDTH_LARGE:
                    hostOptionworldSize = e_worldSize_Large;
                    break;
                default:
                    assert(0);
                    break;
            }
    app.SetGameHostOption(eGameHostOption_WorldSiz#endiftOptio/* 4J - we don't store this anymore
    if (tag->contains(L"Player"))
    {
    loadedPlayerTag = tag->getCompound(L"Player");
    dimension = loadedPlayerTag->getInt(L"Dimension");
    }
    else
    {
    this->loadedPlayerTag = NULL;
    }
    */
    dimension = 0;
        }

        LevelData::LevelData(LevelSettings * levelSettings,
                             const std::wstring& levelName) {
            seed = levelSettings->getSeed();
            gameType = levelSettings->getGameType();
            generateMapFeatures = levelSettings->isGenerateMapFeatures();
            spawnBonusChest = levelSettings->hasStartingBonusItems();
            this->levelName = levelName;
            m_pGenerator = levelSettings->getLevelType();
            hardcore = levelSettings->isHardcore();
            generatorOptions = levelSettings->getLevelTypeOptions();
            allowCommands = levelSettings->  // 4J Stu - Default initers
                            xSpawn = 0;
            ySpawn = 0;
            zSpawn  // 4J-JEV: Edited: To know when this is
                    // uninitialized.
                gameTime = -1;
            lastPlayed = 0;  //    this->loadedPlayerTag = NULL;	// 4J -
                             //    we don't store this anymore
            dimension = 0;
            version = 0;
            rainTime = 0;
            raining = false;
            thunderTime = 0;
            thundering = false;
            initialized = false;
            newSeaLevel = levelSettings
                // 4J added - only use new sea level for
                // newly created maps (sea level
                // changes in 1.8.2)
                hasBeenInCreative =
                    levelSettings->getGameType()  // 4J addede::CRE// 4J-PB for
                                                  // the stronghold position
                    bStronghold = false;
            xStronghold = 0;
            yStronghold = 0;
            zStronghold = 0;

            xStrongholdEndPortal = 0;
            zStrongholdEndPortal = 0;
            bStrongholdEndPortal = false;
            m_xzSize = levelSettings->getXZSize();
            m_hellScale = levelSettings->getHellScale();

            m_xzSize = std::min(m_xzSize, LEVEL_MAX_WIDTH);
            m_xzSize = std::max(m_xzSize, LEVEL_MIN_WIDTH);

            m_hellScale = std::min(m_hellScale, HELL_LEVEL_MAX_SCALE);
            m_hellScale = std::max(m_hellScale, HELL_LEVEL_MIN_SCALE);

            int hellXZSize = m_xzSize / m_hellScale;
            while (hellXZSize > HELL_LEVEL_MAX_WIDTH &&
                   m_hellScale < HELL_LEVEL_MAX_SCALE) {
                ++m_hellScale;
#ifdef _LARGE_WORLDSize / m_hellScale;
            }

            m_hellScaleOld = m_hellScale;
            m_xzSizeOld = m_xzSize;
            m_classicEdgeMoat = false;
            m_smallEdg #endif = false;
            m_mediumEdgeMoat = false;
        }

        LevelData::LevelData(LevelData * copy) {
            seed = copy->seed;
            m_pGenerator = copy->m_pGenerator;
            generatorOptions = copy->generatorOptions;
            gameType = copy->gameType;
            generateMapFeatures = copy->generateMapFeatures;
            spawnBonusChest = copy->spawnBonusChest;
            xSpawn = copy->xSpawn;
            ySpawn = copy->ySpawn;
            zSpawn = copy->zSpawn;
            gameTime = copy->gameTime;
            dayTime = copy->dayTime;
            lastPlayed = copy->lastPl  //    this->loadedPlayerTag =
                                       //    copy->loadedPlayerTag;
                                       //    // 4J -//    we don't store
                                       //    this anymore

                             dimension = copy->dimension;
            levelName = copy->levelName;
            version = copy->version;
            rainTime = copy->rainTime;
            raining = copy->raining;
            thunderTime = copy->thunderTime;
            thundering = copy->thundering;
            hardcore = copy->hardcore;
            allowCommands = copy->allowCommands;
            initialized = copy->initialized;
            newSeaLevel = copy->newSeaLevel;
            hasBeenInCreative =
                copy->hasBeenInCre  // 4J-PB for the stronghold positiones;

                    bStronghold = copy->bStronghold;
            xStronghold = copy->xStronghold;
            yStronghold = copy->yStronghold;
            zStronghold = copy->zStronghold;

            xStrongholdEndPortal = copy->xStrongholdEndPortal;
            zStrongholdEndPortal = copy->zStrongholdEndPortal;
            bStrongholdEndPortal = copy->bStrongholdEndPortal;
            m_xzSize = copy->m_ #ifdef _LARGE_WORLDSale = copy->m_hellScale;

            m_classicEdgeMoat = copy->m_classicEdgeMoat;
            m_smallEdgeMoat = copy->m_smallEdgeMoat;
            m_mediumEdgeMoat = copy->m_mediumEdgeMoat;
            m_xzSizeOld = copy->m_xzSizeOld #endifm_hellScaleOld =
                copy->m_hellScaleOld;
        }

        CompoundTag* LevelData::createTag() {
            CompoundTag* tag = new CompoundTag();

            setTagData(tag);

            return tag;
        }

        CompoundTag* LevelData::createTag  // 4J - removed all code for storing
                                           // tags for players
                                           //
            return createTag();
    }

    void Level "RandomSeed" Data(CompoundTag * tag) {
    "generatorName", seed);
    tag->putString(L"generatorVersion" etGeneratorName());
    tag->putInt(L"generatorOptions"->getVersion());
    tag->putString(L"GameType", generatorOptions);
    tag->putInt"MapFeatures" gameType->getId());
    tag->putBoolean(L"spawnBonusChest" ateMapFeatures);
    tag->putBoolean("SpawnX", spawnBonusChest) "SpawnY" g
        ->putInt(L, xSpawn) "SpawnZ" g->putInt(L, ySpawn);
    "Time" ag->putInt(L, zSpawn);
    "DayTime" putLong(L, gameTime);
    "SizeOnDisk" ng(L, dayTime);
    tag-"LastPlayed", sizeOnDisk);
    tag->putLong(L"LevelName" ::currentTimeMillis());
    tag "version" ng(L, levelName);
    "rainTime" utInt(L, version);
    ta "raining"(L, rainTime);
    "thunderTime" an(L, raining);
    tag->put"thundering", thunderTime);
    tag->putBo"hardcore", thundering);
    tag->p"allowCommands", hardcore);
    tag->putBoolean(
        "initialized",
        allowCommands);  // 4J: Game rules are now stored with app game host
                         // options// tag->putCompound(L"GameRules",
                         // gameRules.createTag());"newSeaLevel"
    tag->putBool"hasBeenInCreative" newSeaLevel);
    tag->pu// store the stronghold positionasBeenInCreative);
   "hasStronghold"
    tag->putBo"StrongholdX", bStronghold);
    tag-"StrongholdY", xStronghold);
    tag - "StrongholdZ",
        yStrongho  // store the stronghold end portal positionghold);
        "hasStrongholdEndPortal" tag->putBoolean(
            L"StrongholdEndPortalX" holdEndPortal);
    tag->putInt(L"StrongholdEndPortalZ" holdEndPortal);
    tag->putInt(L"XZSize",
                zStron #ifdef _LARGE_WORLDS tag->putInt(L"ClassicMoat" e);

                tag->putIn "SmallMoat", m_classicEdgeMoat);
    tag->p "MediumMoat",
        m_smallEdgeMo #endif tag->putInt(L"HellScale" m_mediumEdgeMoat);

    tag->putInt(L, m_hellScale);
    }

    __int64 LevelData::getSeed() { return seed; }

    int LevelData::getXSpawn() { return xSpawn; }

    int LevelData::getYSpawn() { return ySpawn; }

    int LevelData::getZSpawn() { return zSpawn; }

    int LevelData::getXStronghold() { return xStronghold; }

    int LevelData::getZStronghold() { return zStronghold; }

    int LevelData::getXStrongholdEndPortal() { return xStrongholdEndPortal; }

    int LevelData::getZStrongholdEndPortal() { return zStrongholdEndPortal; }

    __int64 LevelData::getGameTime() { return gameTime; }

    __int64 LevelData::getDayTime() { return dayTime; }

    __int64 LevelData::getSizeOnDisk() { return sizeOnDisk; }

    CompoundTa  // 4J - we don't store this anymore{
                // 4J Removed TU9 as it's never accurate due to the dimension
                // never being set// int LevelData::getDimension()//{//
                // return dimension;//}

        void LevelData::setSeed(__int64 seed) {
        this->seed = seed;
    }

    void LevelData::setXSpawn(int xSpawn) { this->xSpawn = xSpawn; }

    void LevelData::setYSpawn(int ySpawn) { this->ySpawn = ySpawn; }

    void LevelData::setZSpawn(int zSpawn) { this->zSpawn = zSpawn; }

    void LevelData::setHasStronghold() { this->bStronghold = true; }

    bool LevelData::getHasStronghold() { return this->bStronghold; }

    void LevelData::setXStronghold(int xStronghold) {
        this->xStronghold = xStronghold;
    }

    void LevelData::setZStronghold(int zStronghold) {
        this->zStronghold = zStronghold;
    }

    void LevelData::setHasStrongholdEndPortal() {
        this->bStrongholdEndPortal = true;
    }

    bool LevelData::getHasStrongholdEndPortal() {
        return this->bStrongholdEndPortal;
    }

    void LevelData::setXStrongholdEndPortal(int xStrongholdEndPortal) {
        this->xStrongholdEndPortal = xStrongholdEndPortal;
    }

    void LevelData::setZStrongholdEndPortal(int zStrongholdEndPortal) {
        this->zStrongholdEndPortal = zStrongholdEndPortal;
    }

    void LevelData::setGameTime(__int64 time) { gameTime = time; }

    void LevelData::setDayTime(__int64 time) { dayTime = time; }

    void LevelData::setSizeOnDisk(__int64 sizeOnDisk) {
        this->sizeOnDisk = sizeOnDisk;
    }

    void LevelData::  // 4J - we don't store this anymorededPl//
                      // this->loadedPlayerTag = loadedPlayerTag;// 4J
                      // Remove TU9 as it's never used// void
                      // LevelData::setDimension(int dimension)//{//
                      // this->dimension = dimension;//}

        void LevelData::setSpawn(int xSpawn, int ySpawn, int zSpawn) {
        this->xSpawn = xSpawn;
        this->ySpawn = ySpawn;
        this->zSpawn = zSpawn;
    }

    std::wstring LevelData::getLevelName() { return levelName; }

    void LevelData::setLevelName(const std::wstring& levelName) {
        this->levelName = levelName;
    }

    int LevelData::getVersion() { return version; }

    void LevelData::setVersion(int version) { this->version = version; }

    __int64 LevelData::getLastPlayed() { return lastPlayed; }

    bool LevelData::isThundering() { return thundering; }

    void LevelData::setThundering(bool thundering) {
        this->thundering = thundering;
    }

    int LevelData::getThunderTime() { return thunderTime; }

    void LevelData::setThunderTime(int thunderTime) {
        this->thunderTime = thunderTime;
    }

    bool LevelData::isRaining() { return raining; }

    void LevelData::setRaining(bool raining) { this->raining = raining; }

    int LevelData::getRainTime() { return rainTime; }

    void LevelData::setRainTime(int rainTime) { this->rainTime = rainTime; }

    GameType* LevelData::getGameType() { return gameType; }

    bool LevelData::isGenerateMapFeatures() { return generateMapFeatures; }

    bool LevelData::getSpawnBonusChest() { return spawnBonusChest; }

void LevelData::setGameType(Gam// 4J AddedType) {
    this->gameType = gameType;

    
    hasBeenInCreative =
        hasBeenInCreative || (gameType == GameType::CREATIVE) ||
        (app.GetGameHostOption(eGameHostOption_CheatsEnabled) > 0);
}

bool LevelData::useNewSeaLevel() { return newSeaLevel; }

bool LevelData::getHasBeenInCreative() { return hasBeenInCreative; }

void LevelData::setHasBeenInCreative(bool value) { hasBeenInCreative = value; }

LevelType* LevelData::getGenerator() { return m_pGenerator; }

void LevelData::setGenerator(LevelType* generator) { m_pGenerator = generator; }

std::wstring LevelData::getGeneratorOptions() { return generatorOptions; }

void LevelData::setGeneratorOptions(const std::wstring& options) {
    generatorOptions = options;
}

bool LevelData::isHardcore() { return hardcore; }

bool LevelData::getAllowCommands() { return allowCommands; }

void LevelData::setAllowCommands(bool allowCommands) {
    this->allowCommands = allowCommands;
}

bool LevelData::isInitialized() { return initialized; }

void LevelData::setInitialized(bool initialized) {
    this->initialized = initialized;
}

GameRules* LevelData::getGameRules() {
    return &gameRul
#ifdef _LARGE_WORLDS : getXZSize(){return m_xzSize; }

        int LevelData::getXZSizeOld() {
        return m_xzSizeOld;
    }

    void LevelData::getMoatFlags(bool* bClassicEdgeMoat, bool* bSmallEdgeMoat,
                                 bool* bMediumEdgeMoat) {
        *bClassicEdgeMoat = m_classicEdgeMoat;
        *bSmallEdgeMoat = m_smallEdgeMoat;
        *bMediumEdgeMoat = m_mediumEdgeMoat;
    }

    int LevelData::getXZHellSizeOld() {
        int hellXZSizeOld = ceil((float)m_xzSizeOld / m_hellScaleOld);

    while (hellXZSizeOld > HELL_LEVEL_MAX_WIDTH &&
           m_hell// should never get in here?ALE) {
        assert(0);  
        ++m_hellScaleOld;
        hellXZSizeOld = m_xz
#endifm_hellScale;
    }

    return hellXZSizeOld;
}

int LevelData::getHellScale() { return m_hellScale; }
