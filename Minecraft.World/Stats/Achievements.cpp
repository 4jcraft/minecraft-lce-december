#include "../Platform/stdafx.h"
#include "../Headers/net.minecraft.stats.h"
#include "../Headers/net.minecraft.world.item.h"
#include "../Headers/net.minecraft.world.level.tile.h"
#include "Achievement.h"
#include "Achievements.h"

const int Achievements::ACHIEVEMENT_OFFSET = 0x500000;

// maximum position of achievements (min and max)

int Achievements::xMin = 4294967295;  // 4J Stu Was 4294967296 which is 1 larger
                                      // than maxint. Hopefully no side effects
int Achievements::yMin = 4294967295;  // 4J Stu Was 4294967296 which is 1 larger
                                      // than maxint. Hopefully no side effects
int Achievements::xMax = 0;
int Achievements::yMax = 0;

std::vector<Achievement*>* Achievements::achievements =
    new std::vector<Achievement*>;

Achievement* Achievements::openInventory = NULL;
Achievement* Achievements::mineWood = NULL;
Achievement* Achievements::buildWorkbench = NULL;
Achievement* Achievements::buildPickaxe = NULL;
Achievement* Achievements::buildFurnace = NULL;
Achievement* Achievements::acquireIron = NULL;
Achievement* Achievements::buildHoe = NULL;
Achievement* Achievements::makeBread = NULL;
Achievement* Achievements::bakeCake = NULL;
Achievement* Achievements::buildBetterPickaxe = NULL;
Achievement* Achievements::cookFish = NULL;
Achievement* Achievements::onARail = NULL;
Achievement* Achievements::buildSword = NULL;
Achievement* Achievements::killEnemy = NULL;
Achievement* Achievements::killCow = NULL;
Achievement* Achievements::flyPig = NULL;

Achievement* Achievements::snipeSkeleton = NULL;
Achievement* Achievements::diamond  // Achievement *Achievements::portal =
                                    // NULL;
    Achievement* Achievements::ghast = NULL;
Achievement* Achievements::blazeRod = NULL;
Achievement* Achievements::potion = NULL;
Achievement* Achievements::theEnd = NULL;
Achievement* Achievements::winGame = NULL;
Achievement* Achievements::enchantment  // Achievement *Achievements::overkill =
                                        // NULL;// Achievement
                                        // *Achievements::bookcase = NULL;// 4J
                                        // : WESTY : Added new acheivements.
    Achievement* Achievements::leaderOfThePack = NULL;
Achievement* Achievements::MOARTools = NULL;
Achievement* Achievements::dispenseWithThis = NULL;
Achievement* Achievements::InToTheNether  // 4J : WESTY : Added other awards.
    Achievement* Achievements::socialPost = NULL;
Achievement* Achievements::eatPorkChop = NULL;
Achievement* Achievements::play100Days = NULL;
Achievement* Achievements::arrowKillCreeper = NULL;
Achievement* Achievements::mine100Blocks = NULL;
Achievement* Achievements::kill10Creeper
#ifdef _EXTENDED_ACHIEVEMENTS
    Achievement* Achievements::overkill  // Restored old achivements.
    Achievement* Achievements::bookcase  // Restored old achivements.// 4J-JEV:
                                         // New Achievements for Orbis.
    Achievement* Achievements::adventuringTime = NULL;
Achievement* Achievements::repopulatio  // Achievement *Achievements::porkChop =
                                        // NULL;
    Achievement*
        Achievements::diamondsToYo  // Achievement *Achievements::passingTheTime
                                    // = NULL;// Achievement
                                    // *Achievements::archer = NULL;
    Achievement* Achievements::theHaggler = NULL;
Achievement* Achievements::potPlanter = NULL;
Achievement* Achievements::itsASign = NULL;
Achievement* Achievements::ironBelly = NULL;
Achievement* Achievements::haveAShearfulDay = NULL;
Achievement* Achievements::rainbowCollection = NULL;
Achievement* Achievements::stayinFrosty = NULL;
Achievement* Achievements::chestfulOfCobblestone = NULL;
Achievement* Achievements::renewableEnergy = NULL;
Achievement* Achievements::musicToMyEars = NULL;
Achievement* Achievements::bodyGuard = NULL;
Achievement* Achievements::ironMan = NULL;
Achievement* Achievements::zombieDoctor = NULL;
Achievement* Achievements::lionTame #endifLL;

void Achievements::staticCtor() {
    Achievements::openInventory =
        (new Achievement(eAward_TakingIn "openInventory", 0, 0, Item::book,
                         NULL))
            ->setAwardLocallyOnly()
            ->postConstruct();
    Achievements::mineWood =
        (new Achievement(eAward_Gett "mineWood", 2, 1, Tile::treeTrunk,
                         (Achievement*)openInventory))
            ->postConstruct();
    Achievements::buildWorkbench =
        (new Achievement(eAward_Bench "buildWorkBench", 4, -1, Tile::workBench,
                         (Achievement*)mineWood))
            ->postConstruct();
    Achievements::buildPickaxe =
        (new Achievement(eAward_Tim "buildPickaxe", 4, 2, Item::pickAxe_wood,
                         (Achievement*)buildWorkbench))
            ->postConstruct();
    Achievements::buildFurnace =
        (new Achievement(eAward_H "buildFurnace", 3, 4, Tile::furnace_lit,
                         (Achievement*)buildPickaxe))
            ->postConstruct();
    Achievements::acquireIron =
        (new Achievement(eAward_AquireH "acquireIron", 1, 4, Item::ironIngot,
                         (Achievement*)buildFurnace))
            ->postConstruct();
    Achievements::buildHoe =
        (new Achievement(eAward_Tim "buildHoe", 2, -3, Item::hoe_wood,
                         (Achievement*)buildWorkbench))
            ->postConstruct();
    Achievements::makeBread =
        (new Achievement(eAward_Ba "makeBread", -1, -3, Item::bread,
                         (Achievement*)buildHoe))
            ->postConstruct();
    Achievements::bakeCake =
        (new Achievement(eAward "bakeCake", 0, -5, Item::cake,
                         (Achievement*)buildHoe))
            ->postConstruct();
    Achievements::buildBetterPickaxe =
        (new Achievement(eAward_GettingAn "buildBetterPickaxe", 6, 2,
                         Item::pickAxe_stone, (Achievement*)buildPickaxe))
            ->postConstruct();
    Achievements::cookFish =
        (new Achievement(eAward_Delici "cookFish", 2, 6, Item::fish_cooked,
                         (Achievement*)buildFurnace))
            ->postConstruct();
    Achievements::onARail =
        (new Achievement(eAward_ "onARail" L, 2, 3, Tile::rail,
                         (Achievement*)acquireIron))
            ->setGolden()
            ->postConstruct();
    Achievements::buildSword =
        (new Achievement(eAward_TimeT "buildSword", 6, -1, Item::sword_wood,
                         (Achievement*)buildWorkbench))
            ->postConstruct();
    Achievements::killEnemy =
        (new Achievement(eAward_Monste "killEnemy", 8, -1, Item::bone,
                         (Achievement*)buildSword))
            ->postConstruct();
    Achievements::killCow =
        (new Achievement(eAward_Co "killCow" L, 7, -3, Item::leather,
                         (Achievement*)buildSword))
            ->postConstruct();
    Achievements::flyPig =
        (new Achievement(eAward_When "flyPig" L, 8, -4, Item::saddle,
                         (Achievement*)killCow))
            ->setGolden()
            ->postConstruc  // 4J Stu - The order of these achievemnts is very
                            // important, as they map// directly to data
                            // stored in the profile data. New achievements
                            // should be// added at the end.// 4J :
                            // WESTY : Added new achievements. Note, params "x",
                            // "y", "icon" and// "requires" are ignored on
                            // xbox.
                Achievements::leaderOfThePack =
            (new Achievement(eAward_LeaderOf "leaderOfThePack", 0, 0,
                             Tile::treeTrunk, (Achievement*)buildSword))
                ->setAwardLocallyOnly()
                ->postConstruct();
    Achievements::MOARTools =
        (new Achievement(eAward_MO "MOARTools", 0, 0, Tile::treeTrunk,
                         (Achievement*)buildSword))
            ->setAwardLocallyOnly()
            ->postConstruct();
    Achievements::dispenseWithThis =
        (new Achievement(eAward_DispenseW "dispenseWithThis", 0, 0,
                         Tile::treeTrunk, (Achievement*)buildSword))
            ->postConstruct();
    Achievements::InToTheNether =
        (new Achievement(eAward_InToTh "InToTheNether", 0, 0, Tile::treeTrunk,
                         (Achievement*)buildSword))
            ->postConstruc  // 4J : WESTY : Added other awards.
                Achievements::mine100Blocks =
            (new Achievement(eAward_mine10 "mine100Blocks", 0, 0,
                             Tile::treeTrunk, (Achievement*)buildSword))
                ->setAwardLocallyOnly()
                ->postConstruct();
    Achievements::kill10Creepers =
        (new Achievement(eAward_kill10C "kill10Creepers", 0, 0, Tile::treeTrunk,
                         (Achievement*)buildSword))
            ->setAwardLocallyOnly()
            ->postCon #ifdef _EXTENDED_ACHIEVEMENTS Achievements::eatPorkChop =
            (new Achievement(eAward_eatP "eatPorkChop", 0, 0, Tile::treeTrunk,
                             (Achievement*)buildSword))
                ->setAwardLocallyOnly()
                ->postCon #elset();

    Achievements::eatPorkChop =
        (new Achievement(eAward_eatP "eatPorkChop", 0, 0, Tile::treeTrunk,
                         (Achievement*)buildSword))
            ->postCon #endif();

    Achievements::play100Days =
        (new Achievement(eAward_play "play100Days", 0, 0, Tile::treeTrunk,
                         (Achievement*)buildSword))
            ->setAwardLocallyOnly()
            ->postConstruct();
    Achievements::arrowKillCreeper =
        (new Achievement(eAward_arrowKill "arrowKillCreeper", 0, 0,
                         Tile::treeTrunk, (Achievement*)buildSword))
            ->postConstruct();
    Achievements::socialPost =
        (new Achievement(eAward_soc "socialPost", 0, 0, Tile::treeTrunk,
                         (Achievement*)buildSword))
            ->postCon
#ifndef _XBOX  // WARNING: NO NEW ACHIEVMENTS CAN BE ADDED HERE// These stats
               // (achievements) are directly followed by new//
               // stats/achievements in the profile data, so cannot be changed
               // without// migrating the profile data// 4J Stu - All new Java
               // achievements removed to stop them using the profile// data //
               // 4J Stu - This achievment added in 1.8.2, but does not map to
               // any Xbox// achievements
                Achievements::snipeSkeleton =
            (new Achievement(eAward_snipeS "snipeSkeleton", 7, 0, Item::bow,
                             (Achievement*)killEnemy))
                ->setGolden()
                ->postConstruc  // 4J Stu - These added in 1.0.1, but do not map
                                // to any Xbox achievements
                    Achievements::diamonds =
                (new Achievement(eAward_d "diamonds", -1, 5, Item::diamond,
                                 (Achievement*)acquireIron))
                    ->postConstru  // Achievements::portal
                                   // = (new// Achievement(eAward_portal,
                                   // L"portal",// -1, 7,	Tile::obsidian,
                                   // (Achievement *)diamonds)//
                                   // )->postConstruct();
                        Achievements::ghast =
                    (new Achievement(eAwar "ghast", L, -4, 8, Item::ghastTear,
                                     (Achievement*)ghast))
                        ->setGolden()
                        ->postConstruct();
    Achievements::blazeRod =
        (new Achievement(eAward_b "blazeRod", 0, 9, Item::blazeRod,
                         (Achievement*)blazeRod))
            ->postConstruct();
    Achievements::potion = (new Achievement(eAward "potion" L, 2, 8,
                                            Item::potion, (Achievement*)potion))
                               ->postConstruct();
    Achievements::theEnd =
        (new Achievement(eAward "theEnd" L, 3, 10, Item::eyeOfEnder,
                         (Achievement*)theEnd))
            ->setGolden()
            ->postConstruct();
    Achievements::winGame =
        (new Achievement(eAward_ "theEnd2" L, 4, 13, Tile::dragonEgg,
                         (Achievement*)winGame))
            ->setGolden()
            ->postConstruct();
    Achievements::enchantments =
        (new Achievement(eAward_encha "enchantments", -4, 4, Tile::enchantTable,
                         (Achievement*)enchantments))
            ->postConstru  //   Achievements::overkill
                           //   = (new//   Achievement(eAward_overkill,
                           //   L"overkill",//   -4, 1,
                           //   Item::sword_diamond, (Achievement
                           //   *)enchantments)//
                           //   )->setGolden()->postConstruct();
                           //   Achievements::bookcase//   = (new
                           //   Achievement(eAward_bookcase,// L"bookcase",
                           //   -3, 6,	Tile::bookshelf,//   (Achievement
                           //   *)enchantments) )->postConstruct();#endif
#ifdef _EXTENDED_ACHIEVEMENTS
                Achievements::overkill =
            (new Achievement(eAward_o "overkill", -4, 1, Item::sword_diamond,
                             (Achievement*)enchantments))
                ->setGolden()
                ->postConstruct();
    Achievements::bookcase =
        (new Achievement(eAward_b "bookcase", -3, 6, Tile::bookshelf,
                         (Achievement*)enchantments))
            ->postConstruct();

    Achievements::adventuringTime =
        (new Achievement(eAward_adventur "adventuringTime", 0, 0,
                         Tile::bookshelf, (Achievement*)NULL))
            ->setAwardLocallyOnly()
            ->postConstruct();
    Achievements::repopulation =
        (new Achievement(eAward_repop "repopulation", 0, 0, Tile::bookshelf,
                         (Achievement*)NULL))
            ->postConstru  // Achievements::porkChoop			// //
                           // //// // // //
                Achievements::diamondsToYou =
            (new Achievement(eAward_diamon "diamondsToYou", 0, 0,
                             Tile::bookshelf, (Achievement*)NULL))
                ->postConstru  // Achievements::passingTheTime		=
                               // (new// Achievement(eAward_play100Days,
                               // L"passingTheTime",// 0,0,
                               // Tile::bookshelf,		(Achievement*)
                               // NULL)// )->postConstruct();
                               // Achievements::archer
                               // = (new//
                               // Achievement(eAward_arrowKillCreeper,
                               // L"archer",// 0,0,	Tile::bookshelf,
                               // (Achievement*) NULL)//
                               // )->postConstruct();
                    Achievements::theHaggler =
                (new Achievement(eAward_the "theHaggler", 0, 0, Tile::bookshelf,
                                 (Achievement*)NULL))
                    ->setAwardLocallyOnly()
                    ->postConstruct();
    Achievements::potPlanter =
        (new Achievement(eAward_pot "potPlanter", 0, 0, Tile::bookshelf,
                         (Achievement*)NULL))
            ->setAwardLocallyOnly()
            ->postConstruct();
    Achievements::itsASign =
        (new Achievement(eAward_i "itsASign", 0, 0, Tile::bookshelf,
                         (Achievement*)NULL))
            ->setAwardLocallyOnly()
            ->postConstruct();
    Achievements::ironBelly =
        (new Achievement(eAward_ir "ironBelly", 0, 0, Tile::bookshelf,
                         (Achievement*)NULL))
            ->postConstruct();
    Achievements::haveAShearfulDay =
        (new Achievement(eAward_haveAShea "haveAShearfulDay", 0, 0,
                         Tile::bookshelf, (Achievement*)NULL))
            ->postConstruct();
    Achievements::rainbowCollection =
        (new Achievement(eAward_rainbowCol "rainbowCollection", 0, 0,
                         Tile::bookshelf, (Achievement*)NULL))
            ->setAwardLocallyOnly()
            ->postConstruct();
    Achievements::stayinFrosty =
        (new Achievement(eAward_stayi "stayingFrosty", 0, 0, Tile::bookshelf,
                         (Achievement*)NULL))
            ->postConstruct();
    Achievements::chestfulOfCobblestone =
        (new Achievement(eAward_chestfulOfCobb "chestfulOfCobblestone", 0, 0,
                         Tile::bookshelf, (Achievement*)NULL))
            ->setAwardLocallyOnly()
            ->postConstruct();
    Achievements::renewableEnergy =
        (new Achievement(eAward_renewabl "renewableEnergy", 0, 0,
                         Tile::bookshelf, (Achievement*)NULL))
            ->postConstruct();
    Achievements::musicToMyEars =
        (new Achievement(eAward_musicT "musicToMyEars", 0, 0, Tile::bookshelf,
                         (Achievement*)NULL))
            ->postConstruct();
    Achievements::bodyGuard =
        (new Achievement(eAward_bo "bodyGuard", 0, 0, Tile::bookshelf,
                         (Achievement*)NULL))
            ->postConstruct();
    Achievements::ironMan =
        (new Achievement(eAward_ "ironMan" L, 0, 0, Tile::bookshelf,
                         (Achievement*)NULL))
            ->postConstruct();
    Achievements::zombieDoctor =
        (new Achievement(eAward_zombi "zombieDoctor", 0, 0, Tile::bookshelf,
                         (Achievement*)NULL))
            ->postConstruct();
    Achievements::lionTamer =
        (new Achievement(eAward_li "lionTamer", 0, 0, Tile::bookshelf,
                         (Achievement*)NULL))
            ->postCon #endif();
    // Static { System.out.println(achievements.size() + " achievements"); }//
    // TODO

    void Achievements::init() {}
