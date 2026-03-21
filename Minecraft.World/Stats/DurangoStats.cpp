#include "../Platform/stdafx.h"

#include "ItemStat.h"

#include "Achievement.h"
#include "Achievements.h"

#include "../Util/DamageSource.h"
#include "../Player/Player.h"
#include "../Items/ItemInstance.h"
#include "../Blocks/Tile.h"
#include "../Items/Item.h"
#include "../Level/Level.h"

#include "../../Minecraft.Client/Minecraft.h"
#include "../Level/LevelData.h"
#include "../Level/Storage/LevelSettings.h"

#include "../../Minecraft.Client/Player/LocalPlayer.h"
#include "../../Minecraft.Client/Player/MultiPlayerLocalPlayer.h"

#include "../Level/Storage/EntityIO.h"

#include "../Minecraft.Client/Durango/ServiceConfig/Events-XBLA.8-149E11AEEvents.h"

#include "DurangoStats.h"

///////////////////
// Ds Item Event //
///////////////////

std::string DsItemEvent::nameMethods[] = {"NONE",
                                          "itemPickedUp",
                                          "itemCrafted",
                                          "itemTakenFromChest",
                                          "itemTakenFromEnderchest",
                                          "itemBought",
                                          "itemSmithed",
                                          "blockMined",
                                          "blockPlaced",
                                          "MAX"};

DsItemEvent::DsItemEvent(int id, const std::wstring& name) : Stat(id, name) {}

bool DsItemEvent::onLeaderboard(ELeaderboardId leaderboard,
                                eAcquisitionMethod methodId, Param* param) {
    switch (methodId) {
        case eAcquisitionMethod_Pickedup:
            switch (param->itemId) {
                case Item::egg_Id:
                case Tile::mushroom_brown_Id:
                case Tile::mushroom_red_Id:
                    return leaderboard == eLeaderboardId_FARMING;
            }
            break;

        case eAcquisitionMethod_Mined:
            switch (param->itemId) {
                case Tile::dirt_Id:
                case Tile::stoneBrick_Id:
                case Tile::sand_Id:
                case Tile::stone_Id:
                case Tile::gravel_Id:
                case Tile::clay_Id:
                case Tile::obsidian_Id:
                    return leaderboard == eLeaderboardId_MINING;

                case Tile::wheat_Id:
                case Tile::pumpkin_Id:
                case Tile::reeds_Id:
                    return leaderboard == eLeaderboardId_FARMING;
            }
            break;
    }

    return false;
}

// 4J-JEV, for tiles/items we want to record stats together.
int DsItemEvent::mergeIds(int itemId) {
    switch (itemId) {
        default:
            return itemId;

        case Tile::mushroom_brown_Id:
        case Tile::mushroom_red_Id:
            return Tile::mushroom_brown_Id;

        case Tile::dirt_Id:
        case Tile::grass_Id:
        case Tile::farmland_Id:
            return Tile::dirt_Id;

        case Tile::redstoneLight_Id:
        case Tile::redstoneLight_lit_Id:
            return Tile::redstoneLight_Id;

        case Tile::redStoneOre_Id:
        case Tile::redStoneOre_lit_Id:
            return Tile::redStoneOre_Id;
    }
}

void DsItemEvent::handleParamBlob(std::shared_ptr<LocalPlayer> player,
                                  byteArray paramBlob) {
    if (paramBlob.length == sizeof(Param)) {
        Param* param = (Param*)paramBlob.data;

        // Combine block ids.
        param->itemId = mergeIds(param->itemId);

        // Send farming leaderboard updates.
        if ((param->methodId == eAcquisitionMethod_Pickedup &&
             onLeaderboard(eLeaderboardId_FARMING, eAcquisitionMethod_Pickedup,
                           param)) ||
            (param->methodId == eAcquisitionMethod_Mined &&
             onLeaderboard(eLeaderboardId_FARMING, eAcquisitionMethod_Mined,
                           param))) {
            EventWriteLeaderboardTotals(
                DurangoStats::getUserId(
                    player                              // UserId
                        DurangoStats::getPlayerSession  // PlayerSessionId
                            player->level->difficulty,  // Difficulty,
                    eLeaderboardId_FARMING,             // ScoreboardId
                    param->itemCount);

                app.DebugPr "<%ls>\tscoreboardFarming(%i:%i:%i)\n",
                DurangoStats::getUserId(player), player->level->difficulty,
                eLeaderboardId_FARMING, param->itemCount);
        }

        // Send mining leaderboard updates.
        if (param->methodId == eAcquisitionMethod_Mined &&
            onLeaderboard(eLeaderboardId_MINING, eAcquisitionMethod_Mined,
                          param)) {
            EventWriteLeaderboardTotals(
                DurangoStats::getUserId(
                    player                              // UserId
                        DurangoStats::getPlayerSession  // PlayerSessionId
                            player->level->difficulty,  // Difficulty,
                    eLeaderboardId_MINING,              // ScoreboardId
                    param->itemCount);
                app.DebugPr "<%ls>\tscoreboardMining(%i:%i:%i)\n",
                DurangoStats::getUserId(player), player->level->difficulty,
                eLeaderboardId_MINING, param->itemCount);
        }

        // Debug printout.
        std::string method = nameMethods[(int)param->methodId];
        app.De"<%ls>\t%s(%i:%i:%i)\n",
                        DurangoStats::getUserId(player), method.c_str(),
                        param->itemId, param->itemAux, param->itemCount);// Split on acquisition method, then send relevant events.
        if (param->methodId == eAcquisitionMethod_Placed) {
            EventWriteBlockPlaced(DurangoStats::getUserId(player),
                                  DurangoStats::getPlayerSession(),
                                  player->level->difficulty, param->itemId,
                                  param->itemAux, param->itemCount);
        } else if (param->methodId == eAcquisitionMethod_Mined) {
            EventWriteBlockBroken(DurangoStats::getUserId(player),
                                  DurangoStats::getPlayerSession(),
                                  player->level->difficulty, param->itemId,
                                  param->itemAux, param->itemCount);
        } else {
            EventWriteMcItemAcquired(DurangoStats::getUserId(player),
                                     // TODO,  
                                     DurangoStats::getPlayerSession(), 0, 0,
                                     player->level->difficulty, param->itemId,
                                     param->methodId,  // (x,y,z)
                                     param->itemAux, param->itemCount);
        } /* 4J-JEV: note that mined events will only fire with 'Instant_Mine'
           * on if you are carrying an appropriate tool for the block that you
           * are mining.
           */
        
byteArray
        DsItemEvent::createParamBlob(eAcquisitionMethod eMethod, int itemId,
                                     int itemAux, int itemCount) {
            byteArray output;
            Param param = {eMethod, itemId, itemAux, itemCount};
            output.data = (uint8_t*)new Param(param);
            output.length = sizeof(Param);
            return o  ///////////////////// Ds Mob Killed
                      ////////////////////////

            DsMobKilled::DsMobKilled(int id, const std::wstring& name)
                : Stat(id, name) {}

            void DsMobKilled::handleParamBlob(
                std::shared_ptr<LocalPlayer> player, byteArray paramBlob) {
                if (paramBlob.length == sizeof(Param)) {
                    Param* param = (Param*)paramBlob.data;

                    if (param->mobType < 0) return;

        if (EventWriteMobKilled(DurangoStats::getUserId(player), 0,
                                DurangoStats::getPlayerSession(),
                                DurangoStats::getMultiplayerCorrelationId(), 0,
                                player->level->difficulty,
                                DurangoStats::getPla// ROUND ID),  
                                0, param->weaponId, param->mobType,
                                param->isRanged ? 1 // (x,y,z),0,  
                                0, param->distance, param->mobType) == 0) {
            app.DebugPrintf(
                "<%ls>\t%s(%i:%i:%i:%i)\n",
                DurangoStats::getUserId(player),
                (par "mobShotWithEntity""mobKilledInMelee"),
                param->mobType, param->weaponId, param->distance,
                param->damage);
        }

        switch (EntityIO::getClass(param->mobType)) {
            case eTYPE_MONSTER:
                if (param->mobType != SPIDER_JOCKEY_ID) break;
                // Fallthrough is spider jockey
            case eTYPE_ZOMBIE:
            case eTYPE_SKELETON:
            case eTYPE_CREEPER:
            case eTYPE_SPIDER:
            case eTYPE_PIGZOMBIE:
            case eTYPE_SLIME:

                if (EventWriteLeaderboardTotals(
                        DurangoStats::getUse      // UserIdr),   
                            DurangoStats::getPla  // PlayerSessionId
                                player->level
                                    ->diffi  // Difficulty, 
                                        eLeaderboardId_KILLI  // ScoreboardId
                        1)  // Count       
                    == 0) {
                    a"<%ls>\tscoreboardKills(%i:%i:1)\n",
                                    DurangoStats::getUserId(player),
                                    player->level->difficulty,
                                    eLeaderboardId_KILLING);
                }
        }
                }
            }

byteArray DsMobKilled::createParamBlob(std::shared_ptr<Player> player,
                                       std::shared_ptr<Mob> mob,
                                       DamageSource*// 4J-JEV: Get the id we use for Durango Server Stats.
    int mob_networking_id;
    eINSTANCEOF mobEType = mob->GetType();
    if ((mobEType == eTYPE_SPIDER) && (mob->rider.lock() != NULL) &&
        (mob->rider.lock()->GetType() == eTYPE_SKELETON) &&
        mob->rider.lock()->isAlive()) {
                mob_networking_id = SPID  // Spider jockey only a concept for
                                          // leaderboards.
    } else if ((mobEType == eTYPE_SKELETON) && (mob->riding != NULL) &&
               (mob->riding->GetType() == eTYPE_SPIDER) &&
               mob->riding->isAlive()) {
                mob_networking_id = SPID  // Spider jockey only a concept for
                                          // leaderboards.
    } else {
        mob_networking_id = EntityIO::eTypeToIoid(mobETyp// Kill made with projectile, arrow/ghast/fireball/snowball.// NB: Snowball kills would make an awesome achievement. ("Not a snowball's// chance...") 
    if (dmgSrc->isProjectile()) {
                    byteArray output;
        Param param = {
            DsMobKilled::RANGED, mob_networking_id,
            EntityIO::eTypeToIoid(dmgSrc->getDirectEntity()->GetType()),
            mob->distanceTo(player->x, player->y,/*not needed*/ 
        };
        output.data = (uint8_t*)new Param(param);
        output.length = sizeof(Param);
        return outp  // Kill made in melee, use itemInHand as
                     // weapon.
                   std::shared_ptr<ItemInstance>
                       item = player->getCarriedItem();
        byteArray output;
        Param param = {DsMobKilled::MELEE, mob_networking_id,
                       (item != NULL ? item->getItem()->id : 0),
                       mob->distanceTo(player->x, playe /*not needed*/ z), 0 
    };
        output.data = (uint8_t*)new Param(param);
        output.length = sizeof(Param);
        /////////////////////// Ds Mob Interact
        //////////////////////////

        std::string DsMobInteract "unknownMobInteraction",
            "mobBred"           , "mobTamed"          ,
            "mobCured"          , "mobCrafted"        ,
            "mobSheared"        };

DsMobInteract::DsMobInteract(int id, const std::wstring& name)
    : Stat(id, name) {}

void DsMobInteract::handleParamBlob(std::shared_ptr<LocalPlayer> player,
                                    byteArray paramBlob) {
                    if (paramBlob.length == sizeof(Param)) {
                        Param* param = (Param*)paramBlob.data;

                        if (param->mobId < 0) return;
                        "<%ls>\t%s(%i)\n" gPrintf(,
                                                  DurangoStats::getUserId(
                                                      player),
                                                  nameInteract
                                                      [param->interactionType]
                                                          .c_str(),
                                                  param->mobId);

                        EventWriteMobInteract(DurangoStats::getUserId(player),
                                              DurangoStats::getPlayerSession(),
                                              param->mobId,
                                              param->interactionType);
                    }
}

byteArray DsMobInteract::createParamBlob(eInteract interactionId,
                                         int entityId) {
                    byteArray output;
                    Param param = {interactionId, EntityIO::eTypeToIoid(
                                                      (eINSTANCEOF)entityId)};
                    output.data = (uint8_t*)new Param(param);
    output.length = sizeof(Param///////////////u// Ds Travel /////////////////


std::string DsTravel::nameMeth"Walk"et"Swim"X]"Fall"  "Climb", "Cart", "Boat", "Pig""Time", , , };

unsigned int DsTravel::CACHE_SIZES[e//  WALK - Meters?  40,  //  SWIM - Meters?  20,  //  FALL - Meters? - Fall event naturally only sends on land, no caching//  necessary.
        // CLIMB - Meters?       //  CART - Meters?       //  BOAT - Meters?       //   PIG - Meters?       //  TIME - GameTicks (20*60*5 ~ 5 mins)
};

DsTravel::DsTravel(int id, const std::wstring& name) : Stat(id, name) {
                ZeroMemory(&param_cache, sizeof(unsigned int) * eMethod_MAX *
                                             MAX_LOCAL_PLAYERS);
}

void DsTravel::handleParamBlob(std::shared_ptr<LocalPlayer> player,
                               byteArray paramBlob) {
                if (paramBlob.length == sizeof(Param)) {
                    Param* param = (Param*)paramBlob.data;

                    int newDistance = cache(player->GetXboxPad(), *param);

                    if (newDistance > 0)
                        write(player, param->method, newDistance);
                }
}

byteArray DsTravel::createParamBlob(eMethod method, int distance) {
                byteArray output;
                Param param = {method, distance};
                output.data = (uint8_t*)new Param(param);
                output.length = sizeof(Param);
                return output;
}

int DsTravel::cache(int iPad, Param& param) {
                if ((eMethod_walk <= param.method) &&
                    (param.method < eMethod_MAX)) {
                    param_cache[iPad][param.method] += param.distance;

                    if (param_cache[iPad][param.method] >
                        CACHE_SIZES[param.method]) {
                        int out = param_cache[iPad][param.method];
                        param_cache[iPad][param.method] = 0;
                        return out;
                    }
                }

                return 0;
}

void DsTravel::flush(std::shared_ptr<LocalPlayer> player) {
                int iPad = player->GetXboxPad();
                for (int i = 0; i < eMethod_MAX; i++) {
                    if (param_cache[iPad][i] > 0) {
                        write(player, (eMethod)i, param_cache[iPad][i]);
                        param_cache[iPad][i] = 0;
                    }
                }
}

void DsTravel::write(std::shared_ptr<LocalPlayer> player, eMethod method,
                     int distance) {
    if (player ="<%ls>\t%s(%i)\n";

    app.DebugPrintf(, DurangoStats::getUserId(player),
                    nameMethods[method].c_str(), distance);

    if (method == DsTravel::eMethod_time) {
                    EventWriteIncTimePlayed(DurangoStats::getUserId(player),
                                            DurangoStats::getPlayerSession(),
                                            player->level->difficulty,
                                            distance);
    } else if ((eMethod_walk <= method) && (method < eMethod_MAX)) {
                    EventWriteIncDistanceTravelled(
                        DurangoStats::getUserId(player),
                        DurangoStats::getPlayerSession(),
                        player->level->difficulty, distance, method);

                    switch (method) {
                        case eMethod_walk:
                        case eMethod_fall:
                        case eMethod_minecart:
                        case eMethod_boat:
                            EventWriteLeaderboardTotals(
                                // UserIdngoStats::getUserId(player), 
                                // PlayerSessionId:getPlayerSession(),
                                // 
                                // Difficulty,evel->difficulty, 
                                // ScoreboardIdrdId_TRAVELLING, 
                                distance);
                            //////////////////k// Ds Item Used
                            /////}//////////////////
                            


DsItemUsed::DsItemUsed(int id, const std::wstring& name)
                                : Stat(id, name) {}

                            void DsItemUsed::handleParamBlob(
                                std::shared_ptr<LocalPlayer> player,
                                byteArray paramBlob) {
                                if (paramBlob.length == sizeof(Param)) {
        Param* param = (Pa"<%ls>\titemUsed(%i,%i,%i)\n"app.DebugPrintf(,
                        DurangoStats::getUserId(player), param->itemId,
                        param->aux, param->count);

        EventWriteMcItemUsed(DurangoStats::getUser// SectionId,                            0,  
                             DurangoStats::getPlaye// MultiplayerCorrelationId,             0,  // Gameplay Mode,                        0,  
                             player->level->difficulty, param->it// (x,y,z),
                             0,  
                             param->aux, param->count, param->hunger);
                                }
                            }

                            byteArray DsItemUsed::createParamBlob(
                                int itemId, int aux, int count, int health,
                                int hunger) {
                                byteArray output;
                                Param param = {itemId, aux, count, health,
                                               hunger};
                                output.data = (uint8_t*)new Param(param);
                                output.l  ////////////////////)// Ds Achievement
                                          /////
                                    ////////////////////



DsAchievement::DsAchievement(int id, const std::wstring& name)
                                    : Stat(id, name) {}

                                void DsAchievement::handleParamBlob(
                                    std::shared_ptr<LocalPlayer> player,
                                    byteArray paramBlob) {
                                    if (paramBlob.length ==
                                        sizeof(SmallParam)) {
                                        SmallParam* paramS =
                                            (SmallParam*)paramBlob.data;
        assert(DurangoStats::binaryAch"<%ls>\tAchievement(%i)\n"        app.DebugPrintf(,
                        DurangoStats::getUserId(player), paramS->award);

        bool canAward = true;
        if (paramS->award == eAward_stayinFrosty) {
                                            canAward =
                                                !player
                                                     ->m_bHasAwardedStayinFrosty;
                                            player->m_bHasAwardedStayinFrosty =
                                                true;
        }

        if (canAward) {
                                            EventWriteAchievementGet(
                                                DurangoStats::getUserId(player),
                                                DurangoStats::
                                                    getPlayerSession(),
                                                paramS->award);
        }
                                    } else if (paramBlob.length ==
                                               sizeof(LargeParam)) {
                                        LargeParam* paramL =
                                            (LargeParam*)paramBlob.data;
                                        assert(
                                            DurangoStats::enhancedAchievement(
                                                paramL->award));

                                        switch (paramL->award) {
            case eAwa"<%ls>\tmusicToMyEars(%i)\n"      app.DebugPrintf(,
                                DurangoStats::getUserId(player), paramL->count);
                EventWritePlayedMusicDisc(DurangoStats::getUserId(player),
                                          DurangoStats::getPlayerSession(),
                                          paramL->count);
                break;

            case eAward_chest"<%ls>\tchestfulOfCobblestone(%i)\n"p.DebugPrintf(,
                                DurangoStats::getUserId(player), paramL->count);
                EventWriteChestfulOfCobblestone(
                    DurangoStats::getUserId(player),
                    DurangoStats::getPlayerSession(), paramL->count);
                break;

            case"<%ls>\toverkill(%i)\n"           app.DebugPrintf(,
                                DurangoStats::getUserId(player), paramL->count);
                EventWriteOverkill(DurangoStats::getUserId(player),
                                   DurangoStats::getPlayerSession(),
                                   paramL->count);
                break;

            cas"<%ls>\nonARail(%i)\n"            app.DebugPrintf(,
                                DurangoStats::getUserId(player), paramL->count);
                EventWriteOnARail(DurangoStats::getUserId(player),
                                  DurangoStats::getPlayerSession(),
                                  paramL->count);
                br// Unsuitable paramBlob length.   assert(false);  
}

byteArray DsAchievement::createSmallParamBlob(eAward award) {
    byteArray output;
    SmallParam param = {award};
    output.data = (uint8_t*)new SmallParam(param);
    output.length = sizeof(SmallParam);
    return output;
}

byteArray DsAchievement::createLargeParamBlob(eAward award, int count) {
    byteArray output;
    LargeParam param = {award, count};
    output.data = (uint8_t*)new LargeParam(param);
    output.l//////////////////////////;// Ds Changed Dimension ////////////////////////////


DsChangedDimension::DsChangedDimension(int id, const std::wstring& name)
    :
                Stat(id, name) {}

                void DsChangedDimension::handleParamBlob(
                    std::shared_ptr<LocalPlayer> player, byteArray paramBlob) {
                    if (paramBlob.length == sizeof(Param)) {
        Param* p"<%ls>\tchangedDimension(%i:%i)\n"     app.DebugPrintf(,
                        DurangoStats::getUserId(player), param->fromDi// No longer used.           param->toDimId);

        
    
                    
                    }
                }

                byteArray DsChangedDimension::createParamBlob(int fromDimId,
                                                              int toDimId) {
                    byteArray output;
                    Param param = {fromDimId, toDimId};
                    output.data = (uint8_t*)new Param(param);
                    //////////////////////o// Ds Entered Biome
                    /////u//////////////////////
                    


DsEnteredBiome::DsEnteredBiome(int id, const std::wstring& name)
                        : Stat(id, name) {}

                    void DsEnteredBiome::handleParamBlob(
                        std::shared_ptr<LocalPlayer> player,
                        byteArray paramBlob) {
                        if (paramBlob.length == sizeof(Param)) {
                            Par "<%ls>\tenteredBiome(%i)\n" ob.data;
                            app.DebugPrintf(,
                                            DurangoStats::getUserId(player),
                                            param->biomeId);

                            EventWriteEnteredNewBiome(
                                DurangoStats::getUserId(player),
                                DurangoStats::getPlayerSession(),
                                param->biomeId);
                        }
                    }

                    byteArray DsEnteredBiome::createParamBlob(int biomeId) {
                        byteArray output;
                        Param param = {biomeId};
    output.data = (uint8_t*)new Param(param////////////////////////i// DURANGO STATISTICS //o////////////////////////

// Hopefully only using the first parameterngoStats() {
    "itemsAcquired"cquired = new DsItemEvent(itemsAcquired_Id, L);
    itemsAcquired->po"itemUsed"t();

    itemUsed = new DsItemUsed(itemUsed_Id, L);
    itemUs"travel"Construct();

    travel = new DsTravel(travel_Id, L);
    travel->setAwardLocallyOnly()->postC"mobKilled"

    mobKilled = new DsMobKilled(mobKilled_Id, L);
    mobKilled->postConstru"mobInteract"bInteract = new DsMobInteract(mobInteract_Id, L);
    mobInteract->postConstruct();

    changedDimen"changedDimension" DsChangedDimension(changedDimension_Id, L);
    changedDimension->postConstruct("enteredBiome"dBiome = new DsEnteredBiome(enteredBiome_Id, L);
    enteredBiome->postConstruct("achievement"vement = new DsAchievement(binAchievement_Id, L);
    achievement->postConstruct();

    achievemen"achievementLocal"ew DsAchievement(binAchievementLocal_Id, L);
    achievementLocal->setAwardLocallyOnly();
    achievementLocal->postConstruct();

    EventRegisterXBLA_149E11AE();
                    }

                    DurangoStats::~DurangoStats() {
                        EventUnregisterXBLA_149E11AE();
                    }

                    Stat* DurangoStats::get_stat(int i) {
                        switch (i) {
                            case itemsAcquired_Id:
                                return (Stat*)itemsAcquired;
                            case itemUsed_Id:
                                return (Stat*)itemUsed;
                            case travel_Id:
                                return (Stat*)travel;
                            case mobKilled_Id:
                                return (Stat*)mobKilled;
                            case mobInteract_Id:
                                return (Stat*)mobInteract;
                            case changedDimension_Id:
                                return (Stat*)changedDimension;
                            case enteredBiome_Id:
                                return (Stat*)enteredBiome;
                            case binAchievement_Id:
                                return (Stat*)achievement;
                            case binAchievementLoc  // Unrecognised stat idrn
                                                    // (Stat*)achievementLocal;

        
        default:
                                assert(false);
                                break;
                        }

                        return NULL;
                    }

                    Stat* DurangoStats::get_walkOneM() { return travel; }

                    Stat* DurangoStats::get_swimOneM() { return travel; }

                    Stat* DurangoStats::get_fallOneM() { return travel; }

                    Stat* DurangoStats::get_climbOneM() { return travel; }

                    Stat* DurangoStats::get_minecartOneM() { return travel; }

                    Stat* DurangoStats::get_boatOneM() { return travel; }

                    Stat* DurangoStats::get_pigOneM() { return travel; }

                    Stat* DurangoStats::get_cowsMilked() {
                        return get_itemsCrafted(Item::bucket_milk_Id);
                    }

                    Stat* DurangoStats::get_killMob() { return mobKilled; }

                    Stat* DurangoStats::get_breedEntity(eINSTANCEOF entityId) {
                        return mobInteract;
                    }

                    Stat* DurangoStats::get_tamedEntity(eINSTANCEOF entityId) {
                        return mobInteract;
                    }

                    Stat* DurangoStats::get_curedEntity(eINSTANCEOF entityId) {
                        return mobInteract;
                    }

                    Stat* DurangoStats::get_craftedEntity(
                        eINSTANCEOF entityId) {
                        return mobInteract;
                    }

                    Stat* DurangoStats::get_shearedEntity(
                        eINSTANCEOF entityId) {
                        return mobInteract;
                    }

                    Stat* DurangoStats::get_timePlayed() { return travel; }

                    Stat* DurangoStats::get_blocksPlaced(int blockId) {
                        return (Stat*)itemsAcquired;
                    }

                    Stat* DurangoStats::get_blocksMined(int blockId) {
                        return (Stat*)itemsAcquired;
                    }

                    Stat* DurangoStats::get_itemsCollected(int itemId,
                                                           int itemAux) {
                        return (Stat*)itemsAcquired;
                    }

                Stat*
                    DurangoStats::g  // 4J-JEV:	These items can be crafted
                                     // trivially to and from their//
                                     // block equivalents,//	'Acquire
                                     // Hardware' also relies on
                                     // 'Count_Crafted(IronIngot)
                                     // ==//Count_Forged(IronIngot)" on
                                     // the Stats server.
        
        case Item::ironIngot_Id:
                case Item::goldIngot_Id:
                case Item::diamond_Id:
                case Item::redStone_Id:
                case Item::emerald_Id:
                    return NULL;

                case Item::dye_powder_Id:
                default:
                    return (Stat*)itemsAcquired;
                    // 4J-JEV: Context needed for itemCrafted for iron/gold
                    // being
                    // smelted.
                    return (Stat*)itemsAcquired;
                }

                Stat* DurangoStats::get_itemsUsed(int itemId) {
                    return (Stat*)itemUsed;
                }

                Stat* DurangoStats::get_itemsBought(int itemId) {
                    return (Stat*)itemsAcquired;
                }

                Stat* DurangoStats::get_changedDimension(int from, int to) {
                    return (Stat*)changedDimension;
                }

                Stat* DurangoStats::get_enteredBiome(int biomeId) {
                    return (Stat*)enteredBiome;
                }

                Stat  // Special case for 'binary' achievements.ievementId) {
    
    if (binaryAchievement(achievementId) ||
        enhancedAchievement(achievementId)) {
                    switch (achievementId) {
                        case eAward_chestfulOfCobblestone:
                        case eAward_TakingInventory:
                            return achievementLocal;

                        default:
                            return achievement;
                    }
                }
                else if (
                    achievementId ==
                    eAward_zombieDoctor)  // Other achievements awarded through
                                          // more detailed generic
                                          // events.
                    return NULL;
                                        }

                                        byteArray
                                        DurangoStats::getParam_walkOneM(
                                            int distance) {
                                            return DsTravel::createParamBlob(
                                                DsTravel::eMethod_walk,
                                                distance);
                                        }

                                        byteArray
                                        DurangoStats::getParam_swimOneM(
                                            int distance) {
                                            return DsTravel::createParamBlob(
                                                DsTravel::eMethod_swim,
                                                distance);
                                        }

                                        byteArray
                                        DurangoStats::getParam_fallOneM(
                                            int distance) {
                                            return DsTravel::createParamBlob(
                                                DsTravel::eMethod_fall,
                                                distance);
                                        }

                                        byteArray
                                        DurangoStats::getParam_climbOneM(
                                            int distance) {
                                            return DsTravel::createParamBlob(
                                                DsTravel::eMethod_climb,
                                                distance);
                                        }

                                        byteArray
                                        DurangoStats::getParam_minecartOneM(
                                            int distance) {
                                            return DsTravel::createParamBlob(
                                                DsTravel::eMethod_minecart,
                                                distance);
                                        }

                                        byteArray
                                        DurangoStats::getParam_boatOneM(
                                            int distance) {
                                            return DsTravel::createParamBlob(
                                                DsTravel::eMethod_boat,
                                                distance);
                                        }

                                        byteArray
                                        DurangoStats::getParam_pigOneM(
                                            int distance) {
                                            return DsTravel::createParamBlob(
                                                DsTravel::eMethod_pig,
                                                distance);
                                        }

                                        byteArray
                                        DurangoStats::getParam_cowsMilked() {
                                            return DsItemEvent::createParamBlob(
                                                DsItemEvent::
                                                    eAcquisitionMethod_Crafted,
                                                Item::bucket_milk_Id, 0, 1);
                                        }

                                        byteArray
                                        DurangoStats::getParam_blocksPlaced(
                                            int blockId, int data, int count) {
                                            return DsItemEvent::createParamBlob(
                                                DsItemEvent::
                                                    eAcquisitionMethod_Placed,
                                                blockId, data, count);
                                        }

                                        byteArray
                                        DurangoStats::getParam_blocksMined(
                                            int blockId, int data, int count) {
                                            return DsItemEvent::createParamBlob(
                                                DsItemEvent::
                                                    eAcquisitionMethod_Mined,
                                                blockId, data, count);
                                        }

                                        byteArray
                                        DurangoStats::getParam_itemsCollected(
                                            int id, int aux, int count) {
                                            return DsItemEvent::createParamBlob(
                                                DsItemEvent::
                                                    eAcquisitionMethod_Pickedup,
                                                id, aux, count);
                                        }

                                        byteArray
                                        DurangoStats::getParam_itemsCrafted(
                                            int id, int aux, int count) {
                                            return DsItemEvent::createParamBlob(
                                                DsItemEvent::
                                                    eAcquisitionMethod_Crafted,
                                                id, aux, count);
                                        }

                                        byteArray
                                        DurangoStats::getParam_itemsUsed(
                                            std::shared_ptr<Player> player,
                                            std::shared_ptr<ItemInstance> itm) {
                                            return DsItemUsed::createParamBlob(
                                                itm->getItem()->id,
                                                itm->getAuxValue(),
                                                itm->GetCount(),
                                                player->getHealth(),
                                                player->getFoodData()
                                                    ->getFoodLevel());
                                        }

                                        byteArray
                                        DurangoStats::getParam_itemsBought(
                                            int id, int aux, int count) {
                                            return DsItemEvent::createParamBlob(
                                                DsItemEvent::
                                                    eAcquisitionMethod_Bought,
                                                id, aux, count);
                                        }

                                        byteArray
                                        DurangoStats::getParam_mobKill(
                                            std::shared_ptr<Player> player,
                                            std::shared_ptr<Mob> mob,
                                            DamageSource * dmgSrc) {
                                            return DsMobKilled::createParamBlob(
                                                player, mob, dmgSrc);
                                        }

                                        byteArray
                                        DurangoStats::getParam_breedEntity(
                                            eINSTANCEOF entityId) {
                                            return DsMobInteract::
                                                createParamBlob(
                                                    DsMobInteract::
                                                        eInteract_Breed,
                                                    entityId);
                                        }

                                        byteArray
                                        DurangoStats::getParam_tamedEntity(
                                            eINSTANCEOF entityId) {
                                            return DsMobInteract::
                                                createParamBlob(
                                                    DsMobInteract::
                                                        eInteract_Tamed,
                                                    entityId);
                                        }

                                        byteArray
                                        DurangoStats::getParam_curedEntity(
                                            eINSTANCEOF entityId) {
                                            return DsMobInteract::
                                                createParamBlob(
                                                    DsMobInteract::
                                                        eInteract_Cured,
                                                    entityId);
                                        }

                                        byteArray
                                        DurangoStats::getParam_craftedEntity(
                                            eINSTANCEOF entityId) {
                                            return DsMobInteract::
                                                createParamBlob(
                                                    DsMobInteract::
                                                        eInteract_Crafted,
                                                    entityId);
                                        }

                                        byteArray
                                        DurangoStats::getParam_shearedEntity(
                                            eINSTANCEOF entityId) {
                                            return DsMobInteract::
                                                createParamBlob(
                                                    DsMobInteract::
                                                        eInteract_Sheared,
                                                    entityId);
                                        }

                                        byteArray DurangoStats::getParam_time(
                                            int timediff) {
                                            return DsTravel::createParamBlob(
                                                DsTravel::eMethod_time,
                                                timediff);
                                        }

                                        byteArray
                                        DurangoStats::getParam_changedDimension(
                                            int from, int to) {
                                            return DsChangedDimension::
                                                createParamBlob(from, to);
                                        }

                                        byteArray
                                        DurangoStats::getParam_enteredBiome(
                                            int biomeId) {
                                            return DsEnteredBiome::
                                                createParamBlob(biomeId);
                                        }

                                        byteArray
                                        DurangoStats::getParam_achievement(
                                            eAward id) {
                                            if (binaryAchievement(id)) {
                                                return DsAchievement::
                                                    createSmallParamBlob(id);
                                            }  // Should be calling the
                                               // appropriate getParam
                                               // function.e);
                                               // 
                                        }
                                        else if (id == eAward_zombieDoctor) {
                                            // If its not a binary
                                            // achievement,TYPE_// don't bother
                                            // constructing the param
                                            // blob.
                                            
    return getParam_noArgs();
                                        }

                                        byteArray
                                        DurangoStats::getParam_onARail(
                                            int dist) {
                                            return DsAchievement::
                                                createLargeParamBlob(
                                                    eAward_OnARail, dist);
                                        }

                                        byteArray DurangoStats::
                                            getParam_chestfulOfCobblestone(
                                                int count) {
                                            return DsAchievement::
                                                createLargeParamBlob(
                                                    eAward_chestfulOfCobblestone,
                                                    count);
                                        }

                                        byteArray
                                        DurangoStats::getParam_overkill(
                                            int dmg) {
                                            return DsAchievement::
                                                createLargeParamBlob(
                                                    eAward_overkill, dmg);
                                        }

                                        byteArray
                                        DurangoStats::getParam_musicToMyEars(
                                            int recordId) {
                                            return DsAchievement::
                                                createLargeParamBlob(
                                                    eAward_musicToMyEars,
                                                    recordId);
                                        }

                                        bool DurangoStats::binaryAchievement(
                                            eAward achievementId) {
                                            switch (achievementId) {
                                                case eAward_InToTheNether:
                                                case eAward_theEnd:
                                                case eAward_WhenPigsFly:
                                                case eAward_diamondsToYou:
                                                case eAward_stayinFrosty:
                                                case eAward_renewableEnergy:
                                                cas /*maybe*/ ironMan
                                                    : case eAward_winGame:
                                                case  eAward_TakingInventory:
            retur/**	4J-JEV,
                Basically achievements with an inconsequential extra parameter
                that I thought best not to / prefered not to / couldn't be
   bothered to make class handlers for. (Motivation: it would be nice for
   players to see how close they were/are to achieving these things).
*/
bool DurangoStats::enhancedAchievement(e// case eAward_TakingInventory:ch (achievementId) {
        
        case eAward_musicToMyEars:
        case eAward_chestfulOfCobblestone:
        case eAward_overkill:
        case eAward_OnARail:
            return true;
        default:
            return false;
                                            }
                                        }

                                        void
                                        DurangoStats::generatePlayerSession() {
                                            DurangoStats* dsInstance =
                                                (DurangoStats*)
                                                    GenericStats::getInstance();
                                            CoCreateGuid(
                                                &dsInstance->playerSessionId);
                                        }

                                        LPCGUID
                                        DurangoStats::getPlayerSession() {
                                            DurangoStats* dsInstance =
                                                (DurangoStats*)
                                                    GenericStats::getInstance();
                                            LPCGUID lpcguid =
                                                &dsInstance->playerSessionId;
                                            return lpcguid;
                                        }

                                        void DurangoStats::
                                            setMultiplayerCorrelationId(
                                                Platform::String ^ mcpId) {
                                            ((DurangoStats*)
                                                 GenericStats::getInstance())
                                                ->multiplayerCorrelationId =
                                                mcpId;
                                        }

                                        LPCWSTR DurangoStats::
                                            getMultiplayerCorrelationId() {
                                            return ((DurangoStats*)
                                                        GenericStats::
                                                            getInstance())
                                                ->multiplayerCorrelationId
                                                ->Data();
                                        }

                                        LPCWSTR DurangoStats::getUserId(
                                            std::shared_ptr<LocalPlayer>
                                                player) {
                                            return getUserId(
                                                player->GetXboxPad());
                                        }

                                        LPCWSTR "" urangoStats::getUserId(
                                            int iPad) {
                                            static std::wstring cache = L;
                                            PlayerUID uid = INVALID_XUID;
                                            ProfileManager.GetXUID(iPad, &uid,
                                                                   true);
                                            cache = uid.toString();
                                            return cache.c_str();
                                        }

                                        void DurangoStats::playerSessionStart(
                                            PlayerUID uid,
                                            std::shared_ptr<Player> plr) {
    if (plr != NULL && plr->l// wprintf(uid.toString().c_str());>getLevelD// EventWritePlayerSessionStart(">>>\tPlayerSessionStart(%ls,%s,%ls,%i,%i)\n"
        app.DebugPrintf(,
                        uid.toString(), DurangoStats::getPlayerSession(),
                        DurangoStats::getMultiplayerCorrelationId(),
                        plr->level->getLevelData()->getGameType()->isSurvival(),
                        plr->level->difficulty);

        EventWritePlayerSessionStart(
            uid.toString().c_str(), DurangoStats::getPlayerSession(),
            DurangoStats::getMultiplayerCorrelationId(),
            plr->level->getLevelData()->getGameType()->isSurvival(),
            plr->level->difficulty);
                                        }
                                    }

                                    void DurangoStats::playerSessionStart(
                                        int iPad) {
                                        PlayerUID puid;
                                        std::shared_ptr<Player> plr;
                                        ProfileManager.GetXUID(iPad, &puid,
                                                               true);
                                        plr = Minecraft::GetInstance()
                                                  ->localplayers[iPad];
                                        playerSessionStart(puid, plr);
                                    }

                                    void DurangoStats::playerSessionPause(
                                        int iPad) {
                                        std::shared_ptr<MultiplayerLocalPlayer>
                                            plr = Minecraft::GetInstance()
                                                      ->localplayers[iPad];
                                        if (plr != NULL && plr->level != NULL &&
                                            plr->level->getLevelData() !=
                                                NULL) {
                                            // EventWritePlayerSessionPause(ileManager.GetXUID(iPad,
                                            // ">>>\tPlayerSessionPause(%ls,%s,%ls)\n"
                                            app.DebugPrintf(,
                                                            puid.toString()
                                                                .c_str(),
                                                            DurangoStats::
                                                                getPlayerSession(),
                                                            DurangoStats::
                                                                getMultiplayerCorrelationId());

                                            EventWritePlayerSessionPause(
                                                puid.toString().c_str(),
                                                DurangoStats::
                                                    getPlayerSession(),
                                                DurangoStats::
                                                    getMultiplayerCorrelationId());
                                        }
                                    }

                                    void DurangoStats::playerSessionResume(
                                        int iPad) {
                                        std::shared_ptr<MultiplayerLocalPlayer>
                                            plr = Minecraft::GetInstance()
                                                      ->localplayers[iPad];
                                        if (plr != NULL && plr->level != NULL &&
                                            plr->level->getLevelData() !=
                                                NULL) {  // EventWritePlayerSessionResume(ProfileManager.GetXUID(iP">>>\tPlayerSessionResume(%ls,%s,%ls,%i,%i)\n"
                                            app.DebugPrintf(,
                                                            puid.toString()
                                                                .c_str(),
                                                            DurangoStats::
                                                                getPlayerSession(),
                                                            DurangoStats::
                                                                getMultiplayerCorrelationId(),
                                                            plr->level
                                                                ->getLevelData()
                                                                ->getGameType()
                                                                ->isSurvival(),
                                                            plr->level
                                                                ->difficulty);

                                            EventWritePlayerSessionResume(
                                                puid.toString().c_str(),
                                                DurangoStats::
                                                    getPlayerSession(),
                                                DurangoStats::
                                                    getMultiplayerCorrelationId(),
                                                plr->level->getLevelData()
                                                    ->getGameType()
                                                    ->isSurvival(),
                                                plr->level->difficulty);
                                        }
                                    }

                                    void DurangoStats::playerSessionEnd(
                                        int iPad) {
                                        std::shared_ptr<MultiplayerLocalPlayer>
                                            plr = Minecraft::GetInstance()
                                                      ->localplayers[iPad];
                                        if (plr != NULL) {
                                            DurangoStats::getInstance()
                                                ->travel->flush(plr);
                                        }
                                    }