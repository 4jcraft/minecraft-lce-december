#include "../Platform/stdafx.h"
#include "../Headers/net.minecraft.locale.h"
#include "../Headers/net.minecraft.world.level.tile.h"
#include "../Headers/net.minecraft.world.item.h"
#include "../Headers/net.minecraft.world.entity.item.h"
#include "../Headers/net.minecraft.world.item.crafting.h"
#include "Achievements.h"
#include "ItemStat.h"
#include "GeneralStat.h"
#include "Stats.h"
#include "../../Minecraft.Client/GameState/StatsCounter.h"

const int Stats::BLOCKS_MINED_OFFSET = 0x1000000;
const int Stats::ITEMS_COLLECTED_OFFSET = 0x1010000;
const int Stats::ITEMS_CRAFTED_OFFSET = 0x1020000;
const int Stats::ADDITIONAL_STATS_OFFSET =
    0x5010000;  // Needs to be higher than Achievements::ACHIEVEMENT_OFFSET =
                // 0x500000;

std::unordered_map<int, Stat*>* Stats::statsById =
    new std::unordered_map<int, Stat*>;

std::vector<Stat*>* Stats::all = new std::vector<Stat*>;
std::vector<Stat*>* Stats::generalStats = new std::vector<Stat*>;
std::vector<ItemStat*>* Stats::blocksMinedStats = new std::vector<ItemStat*>;
std::vector<ItemStat*>* Stats::itemsCollectedStats = new std::vector<ItemStat*>;
std::vector<ItemSta
#if (defined _EXTENDED_ACHIEVEMENTS) && (!defined _DURANGO)
                std::vect #endifmStat*>* Stats::blocksPlacedStats =
    new std::vector<ItemStat*>;

Stat* Stats::walkOneM = NULL;
Stat* Stats::swimOneM = NULL;
Stat* Stats::fallOneM = NULL;
Stat* Stats::climbOneM = NULL;
Stat* Stats::minecartOneM = NULL;
Stat* Stats::boatOneM = NULL;
Stat* Stats::pigOneM = NULL;
Stat* Stats::portalsCreated = NULL;
Stat* Stats::cowsMilked = NULL;
Stat* Stats::netherLavaCollected = NULL;
Stat* Stats::killsZombie = NULL;
Stat* Stats::killsSkeleton = NULL;
Stat* Stats::killsCreeper = NULL;
Stat* Stats::killsSpider = NULL;
Stat* Stats::killsSpiderJockey = NULL;
Stat* Stats::killsZombiePigman = NULL;
Stat* Stats::killsSlime = NULL;
Stat* St  // 4J : WESTY : Added for new achievements.etherZombiePigman = NULL;

    Stat* Stats::befriendsWolf = NULL;
Stat* Stats::totalBlocksMined = NULL;
Stat* Stats::timePlayed = NULL;

StatArray Stats::blocksM
#if (defined _EXTENDED_ACHIEVEMENTS) && (!defined _DURANGO)sCrafted;

    StatArray Stats::blocksPlaced;
#endifrray Stats::rainbowCollection;
StatArray
    Stat  // The number of times this player has dealt the killing blow to the
          // // Enderdragon//
          // The number of times this player has beens::completeTheEnd = NULL;
          // // present when the Enderdragon has died
          //

    voi "stat.walkOneM" Ctor() {
    Stats::walkOneM =
        (new GeneralStat(2000, L, (StatFormatter*)Stat::distanceFormatter))
            ->setAwardLocallyOnly() "stat.swimOneM" truct();
    Stats::swimOneM =
        (new GeneralStat(2001, L, (StatFormatter*)Stat::distanceFormatter))
            ->setAwardLocallyOnly() "stat.fallOneM" truct();
    Stats::fallOneM = (new GeneralStat(2002, L,
                                       (StatFormatter*)Stat::distanceFormatter))
                          ->setAwardLocallyOnly()
                          ->po "stat.climbOneM" Stats::climbOneM =
        (new GeneralStat(2003, L, (StatFormatter*)Stat::distanceFormatter))
            ->setAwardLocallyOnly()
            ->postC "stat.minecartOneM" ts::minecartOneM =
            (new GeneralStat(2004, L, (StatFormatter*)Stat::distanceFormatter))
                ->setAwardLocallyOnly() "stat.boatOneM" truct();
    Stats::boatOneM =
        (new GeneralStat(2005, L, (StatFormatter*)Stat::distanceFormatter))
            ->setAwardLocallyOnly() "stat.pigOneM" nstruct();
    Stats::pigOneM = (new GeneralStat(2006, L,
                                      (StatFormatter*)Stat::distanceFormatter))
                         ->setAwardLocallyOnly()
                         ->postCon "stat.portalsUsed"s ::portalsCreated =
        (new GeneralStat(2007, L))->pos "stat.cowsMilked" Stats::cowsMilked =
            (new GeneralStat(2008, L))
                ->postConstruc "stat.netherLavaCollected" Collected =
                (new GeneralStat(2009, L))
                    ->post "stat.killsZombie" tats::killsZombie =
                    (new GeneralStat(2010, L))
                        ->postCo "stat.killsSkeleton" ::killsSkeleton =
                        (new GeneralStat(2011, L))
                            ->postC "stat.killsCreeper" ts::killsCreeper =
                            (new GeneralStat(2012, L))
                                ->post "stat.killsSpider" tats::killsSpider =
                                (new GeneralStat(2013, L))
                                    ->postConstr
                                "stat.killsSpiderJockey" piderJockey =
                                    (new GeneralStat(2014, L))
                                        ->postConstr
                                    "stat.killsZombiePigman" ombiePigman =
                                        (new GeneralStat(2015, L))
                                            ->pos
                                        "stat.killsSlime" Stats::killsSlime =
                                            (new GeneralStat(2016, L))
                                                ->pos "stat.killsGhast" Stats::
                                                    killsGhast =
                                                (new GeneralStat(2017, L))
                                                    ->postConstruct();"stat.killsNetherZombiePigman"igman =
        (new GeneralStat(2018, // 4J : WESTY : Added for new achievements.   ->postConstruct();

    "stat.befriendsWolf"::befriendsWolf =
        (new GeneralStat(2019, L))->postConst"stat.totalBlocksMined"lBlocksMined =
        (ne// 4J-PB - don't want the time played going to the serverruct();

    "stat.timePlayed"
    Stats::timePlayed = (new GeneralStat(2021, L))
                            ->setA// WARNING: NO NEW STATS CAN BE ADDED HERE    -// These stats are directly followed by the achievemnts in the profile data,// so cannot be changed without migrating the profile data
    

    buildBlockSta// 4J Stu - Added this function to allow us to add news stats from TU9// onwards
    
    buildAdditionalStat// WARNING: NO NEW STATS CAN BE ADDED HERE:// These stats are directly followed by the achievemnts in the profile data, so// cannot be changed without migrating the profile data

void Stats::buildBlockStats() {
    blocksMined = StatArray(32"mineBlock.dirt"tat* newStat = new ItemStat(BLOCKS_MINED_OFFSET + 0, L,
                                     Tile::dirt->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::dirt->id] = newStat;
    blocksMined[Tile::grass->id] = newStat;
    blocksMined[Tile::farmland->id] = newStat;
    newStat->p"mineBlock.stone"    newStat = new ItemStat(BLOCKS_MINED_OFFSET + 1, L,
                           Tile::cobblestone->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::cobblestone->id] = newStat;
    newStat->p"mineBlock.sand"
    newStat = new ItemStat(BLOCKS_MINED_OFFSET + 2, L,
                           Tile::sand->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::sand->id] = newStat;
    newStat->p"mineBlock.cobblestone"wStat = new ItemStat(BLOCKS_MINED_OFFSET + 3, L,
                           Tile::stone->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::stone->id] = newStat;
    newStat->p"mineBlock.gravel"   newStat = new ItemStat(BLOCKS_MINED_OFFSET + 4, L,
                           Tile::gravel->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::gravel->id] = newStat;
    newStat->p"mineBlock.clay"
    newStat = new ItemStat(BLOCKS_MINED_OFFSET + 5, L,
                           Tile::clay->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::clay->id] = newStat;
    newStat->p"mineBlock.obsidian" newStat = new ItemStat(BLOCKS_MINED_OFFSET + 6, L,
                           Tile::obsidian->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::obsidian->id] = newStat;
    newStat->p"mineBlock.coal"
    newStat = new ItemStat(BLOCKS_MINED_OFFSET + 7, L,
                           Tile::coalOre->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::coalOre->id] = newStat;
    newStat->p"mineBlock.iron"
    newStat = new ItemStat(BLOCKS_MINED_OFFSET + 8, L,
                           Tile::ironOre->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::ironOre->id] = newStat;
    newStat->p"mineBlock.gold"
    newStat = new ItemStat(BLOCKS_MINED_OFFSET + 9, L,
                           Tile::goldOre->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::goldOre->id] = newStat;
    newStat->po"mineBlock.diamond" newStat = new ItemStat(BLOCKS_MINED_OFFSET + 10, L,
                           Tile::diamondOre->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::diamondOre->id] = newStat;
    newStat->po"mineBlock.redstone"newStat = new ItemStat(BLOCKS_MINED_OFFSET + 11, L,
                           Tile::redStoneOre->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::redStoneOre->id] = newStat;
    blocksMined[Tile::redStoneOre_lit->id] = newStat;
    newStat->po"mineBlock.lapisLazuli"Stat = new ItemStat(BLOCKS_MINED_OFFSET + 12, L,
                           Tile::lapisOre->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::lapisOre->id] = newStat;
    newStat->po"mineBlock.netherrack"wStat = new ItemStat(BLOCKS_MINED_OFFSET + 13, L,
                           Tile::netherRack->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::netherRack->id] = newStat;
    newStat->po"mineBlock.soulSand"newStat = new ItemStat(BLOCKS_MINED_OFFSET + 14, L,
                           Tile::soulsand->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::soulsand->id] = newStat;
    newStat->po"mineBlock.glowstone"ewStat = new ItemStat(BLOCKS_MINED_OFFSET + 15, L,
                           Tile::glowstone->id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::glowstone->id] = newStat;
    newStat->po"mineBlock.wood"    newStat = new ItemStat(BLOCKS_MINED_OFFSET + 16, L,
                           Tile::treeTrunk->id);
    blocksMinedStats->push_back(newStat);
    blocksMin// WARNING: NO NEW STATS CAN BE ADDED HEREStat-// These stats are directly followed by the achievemnts in the profile data,// so cannot be changed without migrating the profile data
    

    blockStatsLoaded = true;
    buildCraftableStats();
}

bool Stats::itemStatsLoaded = false;

void Stats::buildItemStats() {
        itemStatsLoaded = true;
        // WARNING: NO NEW STATS CAN BE ADDED HEREf// These stats are directly
        // followed by the achievemnts in the profile data, so// cannot be
        // changed without migrating the profile data

        void Stats::buildCraftableStats() {
    if (!bl// still waiting for the JVM to load stufftableStat// Or stats already loaded
    // Collected statseturn;
        }

        craftableStatsLoaded = true;

        itemsCollected = StatArray(32000);

    ItemStat* newStat = new ItemS"collectItem.egg"ED_OFFSET + 0,
                                     L, Item::egg->id);
    itemsCollectedStats->push_back(newStat);
    itemsC  // 4J Stu - The following stats were added as it was too easy to
            // cheat the// leaderboards by dropping and picking up these
            // items They are now changed// to mining the block which
            // involves a tiny bit more effort
        "mineBlock.wheat" newStat =
            new ItemStat(BLOCKS_MINED_OFFSET + 18, L, Tile::wheat_Id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::wheat_Id] = newStat;
    newStat->po "mineBlock.mushroom1" ewStat =
        new ItemStat(BLOCKS_MINED_OFFSET + 19, L, Tile::mushroom_brown_Id);
    blocksMinedStats->push_back(newStat);
    blocksMined[Tile::mushroom_brown_Id] = newStat;
    newStat->po "mineBlock.sugar" newStat =
        new ItemStat(BLOCKS_MINED_OFFSET + 17, L, Tile::reeds_Id);
    blocksMinedStats->push_back(newStat);
#if 0ksMined [Tile::reeds_Id] = newStat;
    newStat->postCo "collectItem.wheat" ewStat =
        new ItemStat(ITEMS_COLLECTED_OFFSET + 1, L, Item::wheat->id);
    itemsCollectedStats->push_back(newStat);
    itemsCollected[Item::wheat->id] = newStat;
    newStat->p "collectItem.mushroom" tat =
        new ItemStat(ITEMS_COLLECTED_OFFSET + 2, L, Tile::mushroom1->id);
    itemsCollectedStats->push_back(newStat);
    itemsCollected[Tile::mushroom1->id] = newStat;
    itemsCollected[Tile::mushroom2->id] = newStat;
    newStat->p "collectItem.sugarCane" at =
        new ItemStat(ITEMS_COLLECTED_OFFSET + 3, L, Item::reeds->id);
    itemsCollectedStats->push_back(newStat);
#endifollected[Item::reeds->id] = newStat;
    newStat->postConstru "collectItem.pumpkin" Stat =
        new ItemStat(ITEMS_COLLECTED_OFFSET + 4, L, Tile::pumpkin->id);
    itemsCollectedStats->push_back(newStat);
    itemsCollected[Tile::pumpkin->id] = newStat;
    itemsCollecte  // Crafted statsin->id] = newStat;
        newStat->postConstruct();

    itemsCrafted = Sta "craftItem.plank" newStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 0, L, Tile::wood->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Tile::wood->id] = newStat;
    newStat->po "craftItem.workbench" ewStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 1, L, Tile::workBench->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Tile::workBench->id] = newStat;
    newStat->po "craftItem.stick" newStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 2, L, Item::stick->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::stick->id] = newStat;
    newStat->po "craftItem.woodenShovel" tat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 3, L, Item::shovel_wood->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted  // 4J : WESTY : Added for new
                  // achievements.tat->postConstruct();

        "craftItem.woodenPickAxe" at =
            new ItemStat(ITEMS_CRAFTED_OFFSET + 4, L, Item::pickAxe_wood->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::pickAxe_wood->id] = newStat;
    newStat->po "craftItem.stonePickAxe" tat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 5, L, Item::pickAxe_stone->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::pickAxe_stone->id] = newStat;
    newStat->po "craftItem.ironPickAxe" Stat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 6, L, Item::pickAxe_iron->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::pickAxe_iron->id] = newStat;
    newStat->postConstr "craftItem.diamondPickAxe" new ItemStat(
        ITEMS_CRAFTED_OFFSET + 7, L, Item::pickAxe_diamond->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::pickAxe_diamond->id] = newStat;
    newStat->po "craftItem.goldPickAxe" Stat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 8, L, Item::pickAxe_gold->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::pickAxe_gold->id] = newStat;
    newStat->po "craftItem.stoneShovel" Stat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 9, L, Item::shovel_stone->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::shovel_stone->id] = newStat;
    newStat->pos "craftItem.ironShovel" Stat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 10, L, Item::shovel_iron->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::shovel_iron->id] = newStat;
    newStat->postConstru "craftItem.diamondShovel" new ItemStat(
        ITEMS_CRAFTED_OFFSET + 11, L, Item::shovel_diamond->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::shovel_diamond->id] = newStat;
    newStat->pos "craftItem.goldShovel" Stat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 12, L, Item::shovel_gold->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::shovel_gold->id] = newStat;
    newStat->pos "craftItem.woodenAxe" wStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 13, L, Item::hatchet_wood->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::hatchet_wood->id] = newStat;
    newStat->pos "craftItem.stoneAxe" ewStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 14, L, Item::hatchet_stone->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::hatchet_stone->id] = newStat;
    newStat->pos "craftItem.ironAxe" newStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 15, L, Item::hatchet_iron->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::hatchet_iron->id] = newStat;
    newStat->pos "craftItem.diamondAxe" Stat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 16, L, Item::hatchet_diamond->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::hatchet_diamond->id] = newStat;
    newStat->pos "craftItem.goldAxe" newStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 17, L, Item::hatchet_gold->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::hatchet_gold->id] = newStat;
    newStat->pos "craftItem.woodenHoe" wStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 18, L, Item::hoe_wood->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::hoe_wood->id] = newStat;
    newStat->pos "craftItem.stoneHoe" ewStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 19, L, Item::hoe_stone->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::hoe_stone->id] = newStat;
    newStat->pos "craftItem.ironHoe" newStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 20, L, Item::hoe_iron->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::hoe_iron->id] = newStat;
    newStat->pos "craftItem.diamondHoe" Stat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 21, L, Item::hoe_diamond->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::hoe_diamond->id] = newStat;
    newStat->pos "craftItem.goldHoe" newStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 22, L, Item::hoe_gold->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::hoe_gold->id] = newStat;
    newStat->pos "craftItem.glowstone" wStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 23, L, Tile::glowstone_Id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Tile::glowstone_Id] = newStat;
    newStat->postConstru "craftItem.tnt" tat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 24, L, Tile::tnt_Id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Tile::tnt_Id] = newStat;
    newStat->pos "craftItem.bowl" newStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 25, L, Item::bowl->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::bowl->id] = newStat;
    newStat->pos "craftItem.bucket" newStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 26, L, Item::bucket_empty->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::bucket_empty->id] = newStat;
    newStat->postConstruct();

    newStat "craftItem.flintAndSteel"TED_OFFSET + 27,
                           L, Item::flintAndSteel->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::flintAndSteel->id] = newStat;
    newStat->pos "craftItem.fishingRod" Stat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 28, L, Item::fishingRod->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::fishingRod->id] = newStat;
    newStat->pos "craftItem.clock" newStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 29, L, Item::clock->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::clock->id] = newStat;
    newStat->pos "craftItem.compass" newStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 30, L, Item::compass->id);
    itemsCraftedStats->push_back(newStat);
    itemsCrafted[Item::compass->id] = newStat;
    newStat->pos "craftItem.map" newStat =
        new ItemStat(ITEMS_CRAFTED_OFFSET + 31, L, Item::map->id);
    itemsCraftedStats->push_back(newStat);
    item// WARNING: NO NEW STATS CAN BE ADDED HEREStat-// These stats are directly followed by the achievemnts in the profile data,// so cannot be changed without migrating the profile data// This sets up a static list of stat/leaderboard pairings, used to tell// which leaderboards need an update
    // 4J Stu - Added this function to allow us to add news stats from TU9 onwards
void Stats::bu// The order of these stats should not be changed, as the map directly to// bits in the profile data// The number of times this player has dealt the killing blow to the  // Enderdragon
    "stat.killsEnderdragon"erdragon =
        (new Ge// The number of times this player has been present when the Enderdragon has// died
    "stat.completeTheEnd"leteTheEnd =

#if (defined _EXTENDED_ACHIEVEMENTS) && (!defined _DURANGO) struct();

 "craftItem.flowerPot"* itemStat =
            new ItemStat(offset++, L, Item::flowerPot_Id);
        itemsCraftedStats->push_back(itemStat);
        itemsCrafted[itemStat->getItemId()] = itemStat;
        i"craftItem.sign"struct();

        itemStat = new ItemStat(offset++, L, Item::sign_Id);
        itemsCraftedStats->push_back(itemStat);
        itemsCrafted[itemStat->getItemId()] = itemStat;
        itemStat->pos"mineBlock.emerald"    itemStat =
            new ItemStat(offset++, L, Tile::emeraldOre_Id);
        blocksMinedStats->push_back(itemStat);
        blocksMined[itemStat-// 4J-JEV: We don't need itemsCollected(emerald) so I'm using it to   // stor itemsBought(emerald) so I don't have to make yet another massive// StatArray for Items Bought.
        "itemsBought.emerald"  itemStat =
            new ItemStat(offset++, L, Item::emerald_Id);
        itemsCollectedStats->push_back(itemStat);
        itemsCollected[itemStat-// 4J-JEV:	WHY ON EARTH DO THESE ARRAYS HAVE TO BE SO PAINFULLY       // LARGE WHEN THEY ARE GOING TO BE MOSTLY EMPTY!!!//			Either way, I'm making this one smaller because//we don't need those record items (and we only need 2).
        
        blocks"blockPlaced.flowerPot");

    itemStat = new ItemStat(offset++, L, Tile::flowerPot_Id);
    blocksPlacedStats->push_back(itemStat);
    blocksPlaced[itemStat->getItemId()] = itemStat;
    i "blockPlaced.sign" ruct();

    itemStat = new ItemStat(offset++, L, Tile::sign_Id);
    blocksPlacedStats->push_back(itemStat);
    blocksPlaced[itemStat->getItemId()] = itemStat;
    itemStat->pos "blockPlaced.wallsign" itemStat =
        new ItemStat(offset++, L, Tile::wallSign_Id);
    blocksPlacedStats->push_back(itemStat);
    blocksPlaced[itemStat->getItemId()] = itemStat;
    itemStat->postConstruct();

    GeneralStat* generalStat = NULL;

    rainbowCollection = StatArray(16);
    for (unsigned int i = 0; i < 16; i++) {
        "rainbowCollection." tat =
            new GeneralStat(offset++, L + _toString<unsigned int>(i));
        generalStats->push_back(generalStat);
        rainbowCollection[i] = generalStat;
        generalStat->postConstruct();
    }

    biomesVisisted = StatArray(23);
    for (unsigned int i = 0; i < 23; i++) {
        "biomesVisited." ralStat =
            new GeneralStat(offset++, L + _toString<unsigned int>(i));
        generalStats->push_back(generalStat);
        biomesVisisted[i] = generalStat;
        generalStat->p "itemCrafted.porkchop"
    }

    itemStat = new ItemStat(offset++, L, Item::porkChop_cooked_Id);
    itemsCraftedStats->push_back(itemStat);
    itemsCrafted[itemStat->getItemId()] = itemStat;
    i "itemEaten.porkchop" ct();

    itemStat = new ItemStat(offset++, L, Item::porkChop_cooked_Id);
    blocksPlacedStats->push_back(itemStat);
    blocksPlaced[itemSt #endiftItemId()] = itemStat;
    itemStat->postConstruct();
    }
}

Stat* Stats::get(int key) { return statsById->at(key); }
