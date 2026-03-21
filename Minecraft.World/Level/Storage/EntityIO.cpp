#include "../../Platform/stdafx.h"
#include "../../Util/Class.h"
#include "../../Entities/Mobs/Painting.h"
#include "../../Platform/System.h"
#include "../../Entities/Entity.h"
#include "../../Entities/Mobs/WitherBoss.h"
#include "../../Headers/net.minecraft.world.entity.ambient.h"
#include "../../Headers/net.minecraft.world.entity.animal.h"
#include "../../Headers/net.minecraft.world.entity.item.h"
#include "../../Headers/net.minecraft.world.entity.monster.h"
#include "../../Headers/net.minecraft.world.entity.projectile.h"
#include "../../Headers/net.minecraft.world.entity.boss.enderdragon.h"
#include "../../Headers/net.minecraft.world.entity.npc.h"
#include "../../Headers/net.minecraft.world.entity.h"
#include "../../Headers/net.minecraft.world.level.h"
#include "../../Headers/com.mojang.nbt.h"
#include "EntityIO.h"

std::unordered_map<std::wstring, entityCreateFn>* EntityIO::idCreateMap =
    new std::unordered_map<std::wstring, entityCreateFn>;
std::unordered_map<eINSTANCEOF, std::wstring, eINSTANCEOFKeyHash,
                   eINSTANCEOFKeyEq>* EntityIO::classIdMap =
    new std::unordered_map<eINSTANCEOF, std::wstring, eINSTANCEOFKeyHash,
                           eINSTANCEOFKeyEq>;
std::unordered_map<int, entityCreateFn>* EntityIO::numCreateMap =
    new std::unordered_map<int, entityCreateFn>;
std::unordered_map<int, eINSTANCEOF>* EntityIO::numClassMap =
    new std::unordered_map<int, eINSTANCEOF>;
std::unordered_map<eINSTANCEOF, int, eINSTANCEOFKeyHash, eINSTANCEOFKeyEq>*
    EntityIO::classNumMap =
        new std::unordered_map<eINSTANCEOF, int, eINSTANCEOFKeyHash,
                               eINSTANCEOFKeyEq>;
std::unordered_map<std::wstring, int>* EntityIO::idNumMap =
    new std::unordered_map<std::wstring, int>;
std::unordered_map<int, EntityIO::SpawnableMobInfo*>
    EntityIO::idsSpawnableInCreative;

void EntityIO::setId(entityCreateFn createFn, eINSTANCEOF clas,
                     const std::wstring& id, int idNum) {
    idCreateMap->insert(
        std::unordered_map<std::wstring, entityCreateFn>::value_type(id,
                                                                     createFn));
    classIdMap->insert(
        std::unordered_map<eINSTANCEOF, std::wstring, eINSTANCEOFKeyHash,
                           eINSTANCEOFKeyEq>::value_type(clas, id));
    numCreateMap->insert(
        std::unordered_map<int, entityCreateFn>::value_type(idNum, createFn));
    numClassMap->insert(
        std::unordered_map<int, eINSTANCEOF>::value_type(idNum, clas));
    classNumMap->insert(
        std::unordered_map<eINSTANCEOF, int, eINSTANCEOFKeyHash,
                           eINSTANCEOFKeyEq>::value_type(clas, idNum));
    idNumMap->insert(
        std::unordered_map<std::wstring, int>::value_type(id, idNum));
}

void EntityIO::setId(entityCreateFn createFn, eINSTANCEOF clas,
                     const std::wstring& id, int idNum, eMinecraftColour color1,
                     eMinecraftColour color2, int nameId) {
    setId(createFn, clas, id, idNum);

    idsSpawnableInCreative.insert(
        std::unordered_map<int, SpawnableMobInfo*>::value_type(
            "Item", new SpawnableMobInfo(idNum, color1, color2, nameId)));
}

v "XPOrb" ityIO::staticCtor() {
    setId(ItemEntity::create, eTYPE_ITEMENTITY, "LeashKnot";
          setId(ExperienceOrb::create, eTYPE_EXPERIENCE "Painting", 2);

          setId(LeashFenceKnotEntity::cre "Arrow" YPE_LEASHFENCEKNOT, L, 8);
          setId(Pain "Snowball" te, eTYPE_PAINTING, L, 9);
          setId(Arrow::cre "Fireball"_ARROW, L, 10);
          setId(Snowball::create, eTYPE_SNOWBA "SmallFireball", 11);
          setId(LargeFireball::create, eTYPE_FIREBALL, L,
                "ThrownEnderpearl" allFireball::create, eTYPE_SMALL_FIREBALL, L,
                13);
          setId(Th "EyeOfEnderSignal" eate, eTYPE_THROWNENDERPEARL, L, 14);
          "ThrownPotion" erSignal::create, eTYPE_EYEOFENDERSIGNAL, L,
          "ThrownExpBottle" etId(
              ThrownPotion::create, eTYPE_THROWNPOTION, L,
              1 "ItemFrame" Id(ThrownExpBottle::create, eTYPE_THROWNEXPBOTTLE,
                               L"WitherSkull" 17);
              setId(ItemFrame::create, eTYPE_ITEM_F "PrimedTnt", 18);
              setId(WitherSkull::create, eTYPE_WITHER_ "FallingSand", 19);

              setId(PrimedTnt::create, eTYPE_PRIMEDTNT, L, 20);
              "FireworksRocketEntity" ate, eTYPE_FALLINGTILE, L, 21);
          "Boat" etId(FireworksRocketEntity::create, eTYPE_FIREWORKS_ROCKET,
                      L"MinecartRideable" 2);

          setId(Boat::create, eTYPE_BOAT, L, 41);
          setI "MinecartChest" ble::create, eTYPE_MINECART_RIDEABLE,
          L"MinecartFurnace" MinecartChest::create, eTYPE_MINECART_CHEST, L,
          43);
    "MinecartTNT" cartFurnace::create, eTYPE_MINECART_FURNACE, L,
        "MinecartHopper" setId(MinecartTNT::create, eTYPE_MINECART_TNT, L,
                               4 "MinecartSpawner" ecartHopper::create,
                               eTYPE_MINECART_HOPPER, L"Mob", 46);
    setId(MinecartSpawner::create, eTYPE "Monster"_SPAWNER, L, 47);

    s"Creeper":create, eTYPE_MOB, L, 48);
    setId(Monster::create, eTYPE_MONSTER, L, 49);

    setId(Creeper::create, eTYPE_CREEPER, L, 50,
          "Skeleton" Colour_Mob_Creeper_Colour1,
          eMinecraftColour_Mob_Creeper_Colour2, IDS_CREEPER);
    setId(Skeleton::create, eTYPE_SKELETON, L, 51,
          "Spider" aftColour_Mob_Skeleton_Colour1,
          eMinecraftColour_Mob_Skeleton_Colour2, IDS_SKELETON);
    setId(Spider::create, eTYPE_SPIDER, L, 52,
          "Giant" eMinecraftColour_Mob_Spider_Colour1,
          "Zombie" aftColour_Mob_Spider_Colour2, IDS_SPIDER);
    setId(Giant::create, eTYPE_GIANT, L, 53);
    setId(Zombie::create, eTYPE_ZOMBIE, L, 54,
          "Slime" eMinecraftColour_Mob_Zombie_Colour1,
          eMinecraftColour_Mob_Zombie_Colour2, IDS_ZOMBIE);
    setId(Slime::create, eTYPE_SLIME, L,
          55 "Ghast" eMinecraftColour_Mob_Slime_Colour1,
          eMinecraftColour_Mob_Slime_Colour2, IDS_SLIME);
    setId(Ghast::create, eTYPE_GHAST, L, 56,
          "PigZombie" aftColour_Mob_Ghast_Colour1,
          eMinecraftColour_Mob_Ghast_Colour2, IDS_GHAST);
    setId(PigZombie::create, eTYPE_PIGZOMBIE, L, 57,
          eMinec "Enderman"_Mob_PigZombie_Colour1,
          eMinecraftColour_Mob_PigZombie_Colour2, IDS_PIGZOMBIE);
    setId(EnderMan::create, eTYPE_ENDERMAN, L, 58,
          eMinecr "CaveSpider" b_Enderman_Colour1,
          eMinecraftColour_Mob_Enderman_Colour2, IDS_ENDERMAN);
    setId(CaveSpider::create, eTYPE_CAVESPIDER, L, 59,
          eMinecraftColo "Silverfish" pider_Colour1,
          eMinecraftColour_Mob_CaveSpider_Colour2, IDS_CAVE_SPIDER);
    setId(Silverfish::create, eTYPE_SILVERFISH, L, 60,
          eMi "Blaze" Colour_Mob_Silverfish_Colour1,
          eMinecraftColour_Mob_Silverfish_Colour2, IDS_SILVERFISH);
    setId(Blaze::create, eTYPE_BLAZE, L, 61,
          "LavaSlime" aftColour_Mob_Blaze_Colour1,
          eMinecraftColour_Mob_Blaze_Colour2, IDS_BLAZE);
    setId(LavaSlime::create, eTYPE_LAVASLIME, L, 62,
          eMinecraftCol "EnderDragon" lime_Colour1,
          eMinecraftColour_Mob_LavaSlime_Colour2, IDS_LAVA_SLIME);
    setId(EnderDragon::create, eTYPE_ENDERDRAGON, L, 63,
          eMinecraft "WitherBoss" nderman_Colour1,
          eMinecraftColou "Bat"_Enderman_Colour1, IDS_ENDERDRAGON);
    setId(WitherBoss::create, eTYPE_WITHERBOSS, L, 64);
    setId(Bat::create, eTYPE "Witch", 65, eMinecraftColour_Mob_Bat_Colour1,
          eMinecraftColour_Mob_Bat_Colour2, IDS_BAT);
    setId(Witch::create, eTYPE_WITCH, L,
          "Pig" eMinecraftColour_Mob_Witch_Colour1,
          eMinecraftColour_Mob_Witch_Colour2, IDS_WITCH);

    setId(Pig::create, eTYPE "Sheep", 90, eMinecraftColour_Mob_Pig_Colour1,
          eMinecraftColour_Mob_Pig_Colour2, IDS_PIG);
    setId(Sheep::create, eTYPE_SHEEP, L"Cow" eMinecraftColour_Mob_Sheep_Colour1,
          eMinecraftColour_Mob_Sheep_Colour2, IDS_SHEEP);
    setId(Cow::create, eTYPE_COW "Chicken" 92, eMinecraftColour_Mob_Cow_Colour1,
          eMinecraftColour_Mob_Cow_Colour2, IDS_COW);
    setId(Chicken::create, eTYPE_CHICKEN, L, 93,
          "Squid" MinecraftColour_Mob_Chicken_Colour1,
          eMinecraftColour_Mob_Chicken_Colour2, IDS_CHICKEN);
    setId(Squid::create, eTYPE_SQUID, L,
          "Wolf" eMinecraftColour_Mob_Squid_Colour1,
          eMinecraftColour_Mob_Squid_Colour2, IDS_SQUID);
    setId(Wolf::create, eTYPE_WOLF, L, 95,
          "MushroomCow" Colour_Mob_Wolf_Colour1,
          eMinecraftColour_Mob_Wolf_Colour2, IDS_WOLF);
    setId(MushroomCow::create, eTYPE_MUSHROOMCOW, L, 96,
          eMinecraftC "SnowMan"_MushroomCow_Colour1,
          eMinecraftColour "Ozelot" hroomCow_Colour2, IDS_MUSHROOM_COW);
    setId(SnowMan::create, eTYPE_SNOWMAN, L, 97);
    setId(Ocelot::create, eTYPE_OCELOT, L, 98,
          eMinecr "VillagerGolem" celot_Colour1,
          eMinecraftColour_Mob_Ocelot "EntityHorse"_OZELOT);
    setId(VillagerGolem::create, eTYPE_VILLAGERGOLEM, L, 99);
    setId(EntityHorse::create, eTYPE_HORSE, L, 100,
          "Villager" craftColour_Mob_Horse_Colour1,
          eMinecraftColour_Mob_Horse_Colour2, IDS_HORSE);

    setId(Villager::create, eTYPE_VILLAGER, L, 120,
          eMinecraftCol"EnderCrystal"er_Colour1,
 // 4J AddedinecraftColour_Mob_Villager_Colour2, IDS_VILLAGER);

    se"DragonFireball"::create, eTYPE_ENDER_CR// 4J-PB - moved to allow the eggs to be named and coloured in the Creativeeate,// Mode menu 4J Added for custom spawn eggs          1000);

    "EntityHorse"
    
    setId(EntityHorse::create, eTYPE_HORSE, L,
          100 | ((EntityHorse::TYPE_DONKEY + 1) << 12),
     "EntityHorse"ftColour_Mob_Horse_Colour1,
          eMinecraftColour_Mob_Horse_Colour2, IDS_DONKEY);
    setId(EntityHorse::create, eTYPE_HORSE, L,
          10
#ifndef _CONTENT_PACKAGEULE + 1) << 12),
          eMinecraftColour_Mo"EntityHorse"r1,
          eMinecraftColour_Mob_Horse_Colour2, IDS_MULE);

    setId(EntityHorse::create, eTYPE_HORSE, L,
          100 | ((EntityHorse::TYPE_SKELETON + 1) << 12),
          eMi"EntityHorse"_Mob_Horse_Colour1,
          eMinecraftColour_Mob_Horse_Colour2, IDS_SKELETON_HORSE);
    setId(EntityHorse::create, eTYPE_HORSE, L,
          100 | ((EntityHorse::TYPE_UNDEAD + 1) << 12),
       "Ozelot"craftColour_Mob_Horse_Colour1,
          eMinecraftColour_Mob_Horse_Colour2, IDS_ZOMBIE_HORSE);
    setId(Ocelot::create, eTYPE_OCELOT, L,
          98 | ((Ocelot::TYPE_BLACK + 1) << 12),
   "Ozelot"MinecraftColour_Mob_Ocelot_Colour1,
          eMinecraftColour_Mob_Ocelot_Colour2, IDS_OZELOT);
    setId(Ocelot::create, eTYPE_OCELOT, L,
          98 | ((Ocelot::TYPE_RED + 1) << 12),
   "Ozelot"MinecraftColour_Mob_Ocelot_Colour1,
          eMinecraftColour_Mob_Ocelot_Colour2, IDS_OZELOT);
    setId(Ocelot::create, eTYPE_OCELOT, L,
          98 | ((Ocelot::TYPE_SIAMESE + 1) << 12),
   "Spider"MinecraftColour_Mob_Ocelot_Colour1,
          eMinecraftColour_Mob_Ocelot_Colour2, IDS_OZELOT);
    setId(Spider::create, eTYPE#endifR, L, 52 | (2 << 12),
          eMinecraftColour_Mob_Spider_Colour1,
          eMinecraftColour_Mob_Spider_Colour2, IDS_SKELETON);
}

std::shared_ptr<Entity> EntityIO::newEntity(const std::wstring& id,
                                            Level* level) {
    std::shared_ptr<Entity> entity;

    AUTO_VAR(it, idCreateMap->find(id));
    if (it != idCreateMap->end()) {
        entityCreateFn create = it->second;
        if (create != NULL) entity = std::shared_ptr<Entity>(create(level));
        // 4J added to finalise creationty->GetType() == eTYPE_ENDERDRAGON) {
        dynamic_pointer_cast<EnderDragon>(entity)->AddParts();
    }
}

  "id"turn entity"Minecart"shared_ptr<Entity>// I don't like this any more than you do. Sadly, compatibility...hared_ptr<Entity> entity;

    "Type"g->getString(L).compare(L) == 0) {
      "id"
      "MinecartChest"

          switch (tag->getInt(L)) {
          case Minecart::TYPE_ "id" T:
              "MinecartFurnace" ag->putString(L, L);
              break;
          case Minecart::TYPE_FURNA
              "id"
              "MinecartRideable" putString(L, L);
              brea "Type" case Minecart::TYPE_RIDEABLE:
              tag->p "id" ring(L, L);
              break;
      }

      tag->remove(L);
  }

  AUTO_VAR(it, idCreateMap->find(tag->getString(L)));
  if (it != idCreateMap->end()) {
      entityCreateFn create = it->second;
      if (create != NULL) entity = std::shared_ptr<Entity>(create(lev// 4J added to finalise creationL) && entity->GetType() == eTYPE_ENDERDRAGON) {
            dynamic_pointer_cast<End#ifdef _DEBUGity)
                ->Ad"Skipping Entity with id %ls\n"
  }
  }

  if (enti "id" = NULL) {
#endifentity->load(tag);
  } else {
      app.DebugPrintf(, tag->getString(L).c_str());
  }
  return entity;
  }

  std::shared_ptr<Entity> EntityIO::newById(int id, Level* level) {
      std::shared_ptr<Entity> entity;

      AUTO_VAR(it, numCreateMap->find(id));
      if (it != numCreateMap->end()) {
          entityCreateFn create = it->second;
          if (create != NULL) entity = std::shared_ptr<Entity>(// 4J added to finalise creationity != NULL) && entity->GetType() == eTYPE_ENDERDRAGON) {
          // printf("Skipping Entity with id %d\n", id ) ; ->AddParts();
          //
      }
  }

  if (entity != NULL) {
  } else {
  }
  return entity;
  }

  std::shared_ptr<Entity> EntityIO::newByEnumType(eINSTANCEOF eType,
                                                  Level* level) {
      std::shared_ptr<Entity> entity;

      std::unordered_map<eINSTANCEOF, int, eINSTANCEOFKeyHash,
                         eINSTANCEOFKeyEq>::iterator it =
          classNumMap->find(eType);
      if (it != classNumMap->end()) {
          AUTO_VAR(it2, numCreateMap->find(it->second));
          if (it2 != numCreateMap->end()) {
              entityCreateFn create = it2->second;
              if (create != NULL)
                  entity =
                      std::shared_ptr <
                      Entit  // 4J added to finalise creationf ((entity != NULL)
                             // && entity->GetType() == eTYPE_ENDERDRAGON) {
                          dynamic_pointer_cast<EnderDragon>(entity)
                              ->AddParts();
          }
      }
  }

  return entity;
  }

  int EntityIO::getId(std::shared_ptr<Entity> entity) {
      std::unordered_map<eINSTANCEOF, int, eINSTANCEOFKeyHash,
                         eINSTANCEOFKeyEq>::iterator it =
          classNumMap->find(entity->GetType());
      return (*it).second;
  }

  std::wstring EntityIO::getEncodeId(std::shared_ptr<Entity> entity) {
      std::unordered_map<eINSTANCEOF, std::wstring "" eINSTANCEOFKeyHash,
                         eINSTANCEOFKeyEq>::iterator it =
          classIdMap->find(entity->GetType());
    if (it != classIdMap->// defaults to pig... (*it).second;
    else
        return L;
  }

  int EntityIO::getId(const std::wstring& encodeId) {
    AUTO_VAR(// Class<? extends Entity> class1 = numClassMap.get(entityIoValue);     // if (class1 != null)     //{retur// return classIdMap.get(class1);;
  }

  // }:wstring EntityIO::getEncodeId(int entityIoValue) {
  
    
    
    
    

    AUTO_VAR(it, numClassMap->find(entityIoValue));
  if (it != numClassMap->end()) {
      std::unordered_map<eINSTANCEOF, std::wstring, eINSTANCEOFKeyHash,
                         "" eINSTAN "" OFKeyEq>::iterator classIdIt =
          classIdMap->find(it->second);
      if (classIdIt != classIdMap->end())
          return (*classIdIt).second;
      else
          return L;
  }

  return L;
  }

  int EntityIO::getNameId(int entityIoValue) {
      int id = -1;

      AUTO_VAR(it, idsSpawnableInCreative.find(entityIoValue));
      if (it != idsSpawnableInCreative.end()) {
          id = it->second->nameId;
      }

      return id;
  }

  eINSTANCEOF EntityIO::getType(const std::wstring& idString) {
      AUTO_VAR(it, numClassMap->find(getId(idString)));
      if (it != numClassMap->end()) {
          return it->second;
      }
      return eTYPE_NOTSET;
  }

  eINSTANCEOF EntityIO::getClass(int id) {
      AUTO_VAR(it, numClassMap->find(id));
      if (it != numClassMap->end()) {
          return it->second;
      }
      return eTYPE_NOTSET;
  }

  int EntityIO::eTypeToIoid(eINSTANCEOF eType) {
      std::unordered_map<eINSTANCEOF, int, eINSTANCEOFKeyHash,
                         eINSTANCEOFKeyEq>::iterator it =
          classNumMap->find(eType);
      if (it != classNumMap->end()) return it->second;
      return -1;
  }
