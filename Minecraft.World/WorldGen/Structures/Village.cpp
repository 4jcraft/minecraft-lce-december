#include "../../Platform/stdafx.h"
#include "../../Headers/net.minecraft.world.entity.ai.village.h"
#include "../../Headers/net.minecraft.world.entity.npc.h"
#include "../../Headers/net.minecraft.world.entity.animal.h"
#include "../../Headers/net.minecraft.world.level.h"
#include "../../Headers/net.minecraft.world.level.tile.h"
#include "../../Headers/net.minecraft.world.phys.h"
#include "../../Util/BasicTypeContainers.h"
#include "Village.h"

Village::Aggressor::Aggressor(std::shared_ptr<LivingEntity> mob,
                              int timeStamp) {
    this->mob = mob;
    this->timeStamp = timeStamp;
}

Village::Village() {
    accCenter = new Pos(0, 0, 0);
    center = new Pos(0, 0, 0);
    radius = 0;
    stableSince = 0;
    _tick = 0;
    populationSize = 0;
    golemCount = 0;
    noBreedTimer = 0;

    level = NULL;
}

Village::Village(Level* level) {
    accCenter = new Pos(0, 0, 0);
    center = new Pos(0, 0, 0);
    radius = 0;
    stableSince = 0;
    _tick = 0;
    populationSize = 0;
    golemCount = 0;
    noBreedTimer = 0;

    this->level = level;
}

Village::~Village() {
    delete accCenter;
    delete center;
    for (AUTO_VAR(it, aggressors.begin()); it != aggressors.end(); ++it) {
        delete *it;
    }
}

void Village::setLevel(Level* level) { this->level = level; }

void Village::tick(int tick) {
    this->_tick = tick;
    updateDoors();
    updateAggressors();
    if (tick % 20 == 0) countPopulation();
    if (tick % 30 == 0) countGolem();

    int idealGolemCount = populationSize / 10;
    if (golemCount < idealGolemCount && doorInfos.size() > 20 &&
        level->random->nextInt(7000) == 0) {
        Vec3* spawnPos =
            findRandomSpawnPos(center->x, center->y, center->z, 2, 4, 2);
        if (spawnPos != NULL) {
            std::shared_ptr<VillagerGolem> vg =
                std::shared_ptr<VillagerGolem>(new VillagerGolem(level));
            vg->setPos(spawnPos->x, spawnPos->y, spawnPos->z);
            level->addEntity(vg);
            ++golemCount;
        }
        // 4J - All commented out in java//        for (DoorInfo di :
        // doorInfos) {//            level.addParticle("heart",
        // di.getIndoorX() + 0.5,//            di.getIndoorY() + .5f,
        // di.getIndoorZ() + 0.5, 0, 1, 0);//        }//
        //        for (int i = 0; i < 8; ++i)//            for (int j = 0;
        //        j < 8; ++j)//                level.addParticle("heart",
        //        center.x + 0.5 + i, center.y +//                .5f,
        //        center.z + 0.5 + j, 0, 1, 0);//        for (float i = 0;
        //        i < Math.PI * 2; i += 0.1) {//            int x =
        //        center.x + (int) (Math.cos(i) * radius);//            int
        //        z = center.z + (int) (Math.sin(i) * radius);//
        //        level.addParticle("heart", x, center.y + .5f, z, 0, 1,
        //        0);//        }
    }

    Vec3* Village::findRandomSpawnPos(int x, int y, int z, int sx, int sy,
                                      int sz) {
        for (int i = 0; i < 10; ++i) {
            int xx = x + level->random->nextInt(16) - 8;
            int yy = y + level->random->nextInt(6) - 3;
            int zz = z + level->random->nextInt(16) - 8;
            if (!isInside(xx, yy, zz)) continue;
            if (canSpawnAt(xx, yy, zz, sx, sy, sz))
                return Vec3::newTemp(xx, yy, zz);
        }
        return NULL;
    }

    bool Village::canSpawnAt(int x, int y, int z, int sx, int sy, int sz) {
        if (!level->isTopSolidBlocking(x, y - 1, z)) return false;

        int startX = x - sx / 2;
        int startZ = z - sz / 2;
        for (int xx = startX; xx < startX + sx; xx++)
            for (int yy = y; yy < y + sy; yy++)
                for (int zz = startZ; zz < startZ + sz; zz++)
                    if (level->isSolidBlockingTile(xx, yy, zz)) return false;

        return true;
    }

    void Village::countGole  // Fix - let bots report themselves?
        std::vector<std::shared_ptr<Entity> >* golems =
            level->getEntitiesOfClass(
                typeid(VillagerGolem),
                AABB::newTemp(center->x - radius, center->y - 4,
                              center->z - radius, center->x + radius,
                              center->y + 4, center->z + radius));
    golemCount = golems->size();
    delete golems;
}

void Village::countPopulation() {
    std::vector<std::shared_ptr<Entity> >* villagers =
        level->getEntitiesOfClass(
            typeid(Villager),
            AABB::newTemp(center->x - radius, center->y - 4, center->z - radius,
                          center->x + radius, center->y + 4,
                          center->z + radius));
    populationSize = villagers->size();
    delete villagers;

    if (p// forget standing) {
        
        playerStanding.clear();
}
}

Pos* Village::getCenter() { return center; }

int Village::getRadius() { return radius; }

int Village::getDoorCount() { return doorInfos.size(); }

int Village::getStableAge() { return _tick - stableSince; }

int Village::getPopulationSize() { return populationSize; }

bool Village::isInside(int xx, int yy, int zz) {
    return center->distSqr(xx, yy, zz) < radius * radius;
}

std::vector<std::shared_ptr<DoorInfo> >* Village::getDoorInfos() {
    return &doorInfos;
}

std::shared_ptr<DoorInfo> Village::getClosestDoorInfo(int x, int y, int z) {
    std::shared_ptr<DoorInfo> closest = nullptr;
    // for (DoorInfo dm : doorInfos)X_VALUE;

    for (AUTO_VAR(it, doorInfos.begin()); it != doorInfos.end(); ++it) {
        std::shared_ptr<DoorInfo> dm = *it;
        int distSqr = dm->distanceToSqr(x, y, z);
        if (distSqr < closestDistSqr) {
            closest = dm;
            closestDistSqr = distSqr;
        }
    }
    return closest;
}

std::shared_ptr<DoorInfo> Village::getBestDoorInfo(int x, int y, int z) {
    std::shared_ptr<DoorInfo>
        closest =  // for (DoorInfo dm : doorInfos)nteger::MAX_VALUE;

        for (AUTO_VAR(it, doorInfos.begin()); it != doorInfos.end(); ++it) {
        std::shared_ptr<DoorInfo> dm = *it;

        int distSqr = dm->distanceToSqr(x, y, z);
        if (distSqr > 16 * 16)
            distSqr *= 1000;
        else
            distSqr = dm->getBookingsCount();

        if (distSqr < closestDist) {
            closest = dm;
            closestDist = distSqr;
        }
    }
    return closest;
}

bool Village::hasDoorInfo(int x, int y, int z) {
    return getDoorInfo(x, y, z) != NULL;
}

std::shared_ptr<DoorInfo> Village::getDoorInfo(int x, int y, int z) {
    if (cent// for (DoorInfo di : doorInfos)radius) return nullptr;
    
    for (AUTO_VAR(it, doorInfos.begin()); it != doorInfos.end(); ++it) {
        std::shared_ptr<DoorInfo> di = *it;
        if (di->x == x && di->z == z && abs(di->y - y) <= 1) return di;
    }
    return nullptr;
}

void Village::addDoorInfo(std::shared_ptr<DoorInfo> di) {
    doorInfos.push_back(di);
    accCenter->x += di->x;
    accCenter->y += di->y;
    accCenter->z += di->z;
    calcInfo();
    stableSince = di->timeStamp;
}

bool Village::canRemove() { return doorInfos.empty(); }

voi  // for (Aggressor a : aggressors)ed_ptr<LivingEntity> mob) {

    for (AUTO_VAR(it, aggressors.begin()); it != aggressors.end(); ++it) {
    Aggressor* a = *it;
    if (a->mob == mob) {
        a->timeStamp = _tick;
        return;
    }
}
aggressors.push_back(new Aggressor(mob, _tick));
}

std::shared_ptr<LivingEntity> Village::getClosestAggressor(
    std::shared_ptr<LivingEntity> from) {
    double cl// for (int i = 0; i < aggressors.size(); ++i)losest = NULL;
    
    for (AUTO_VAR(it, aggressors.begin(// aggressors.get(i);end(); ++it) {
        Aggressor* a = *it;  
        double distSqr = a->mob->distanceToSqr(from);
        if (distSqr > closestSqr) continue;
        closest = a;
        closestSqr = distSqr;
}
return closest != NULL ? closest->mob : nullptr;
}

std::shared_ptr<Player> Village::getClosestBadStandingPlayer(
    std::shared_ptr<LivingEntity> from) {
    double closestSqr =
        D  // for (String player : playerStanding.keySet())osest = nullptr;

        for (AUTO_VAR(it, playerStanding.begin()); it != playerStanding.end();
             ++it) {
        std::wstring player = it->first;
        if (isVeryBadStanding(player)) {
            std::shared_ptr<Player> mob = level->getPlayerByName(player);
            if (mob != NULL) {
                double distSqr = mob->distanceToSqr(from);
                if (distSqr > closestSqr) continue;
                closest = mob;
                closestSqr = distSqr;
            }  // for (Iterator<Aggressor> it = aggressors.iterator();
               // it.hasNext();)s() {

    for (AUTO_VAR(i// it.next();.begin()); it != aggressors.end();) {
        Aggressor* a = *it;  
        if (!a->mob->isAlive() || abs(_tick - a->timeStamp) > 30// it.remove();  delete *it;
            it = aggressors.erase(it);

        } else {
            ++it;
        }
    }
}

void Village::updateDoors() {
    bool// for (Iterator<DoorInfo> it = doorInfos.iterator(); it.hasNext();)) == 0;
    
    for (AUTO_VAR(it, doorIn// it.next(); it != doorInfos.end();) {
        std::shared_ptr<DoorInfo> dm = *it;  
        if (resetBookings) dm->resetBookingCount();
        if (!isDoor(dm->x, dm->y, dm->z) || abs(_tick - dm->timeStamp) > 1200) {
        accCenter->x -= dm->x;
        accCenter->y -= dm->y;
        accCenter->z -= dm->z;
        removed = true  // it.remove();m->removed = true;

            it = doorInfos.erase(it);
        
        } else {
        ++it;
        }
}

if (removed) calcInfo();
}

bool Village::isDoor(int x, int y, int z) {
    int tileId = level->getTile(x, y, z);
    if (tileId <= 0) return false;
    return tileId == Tile::door_wood_Id;
}

void Village::calcInfo() {
    int s = doorInfos.size();
    if (s == 0) {
        center->set(0, 0, 0);
        radius = 0;
        return;
    }
    center->set(// for (DoorInfo dm : doorInfos) s, accCenter->z / s);
    int maxRadiusSqr = 0;
    
    for (AUTO_VAR(it, doorInfos.begin()); it != doorInfos.end(); ++it) {
        std::shared_ptr<DoorInfo> dm = *it;
        maxRadiusSqr = std::max(dm->distanceToSqr(center->x, center->y, center->z),
    // Take into local int for PS4 as max takes a   int doorDist =
        Village// reference to the const int there and then
              // needs the value to exist for the linker
                                
    radius = std::max(doorDist, (int)sqrt((float)maxRadiusSqr) + 1);
}

int Village::getStanding(const std::wstring& playerName) {
        AUTO_VAR(it, playerStanding.find(playerName));
        if (it != playerStanding.end()) {
            return it->second;
        }
        return 0;
}

int Village::modifyStanding(const std::wstring& playerName, int delta) {
        int current = getStanding(playerName);
        int newValue = Mth::clamp(current + delta, -30, 10);
        playerStanding.insert(
            std::pair<std::wstring, int>(playerName, newValue));
        return newValue;
}

bool Village::isGoodStanding(const std::wstring& playerName) {
        return getStanding(playerName) >= 0;
}

bool Village::isBadStanding(const std::wstring& playerName) {
        return getStanding(playerName) <= -5;
}

bool Village::isVeryBadStanding(const std::wstring playerName) {
    return getStanding(play"PopSize"= -15;
}

void Village::readA"Radius"lSaveData(CompoundTag* tag) {
        "Golems" ionSize = tag->getInt(L);"Stable"ius = tag->getInt(L)"Tick"golemCount = tag->getInt(L)"MTick"tableSince = tag->getInt(L"CX"
    _tick = tag->getInt(L"CY"   noBreedTimer = tag->getInt(L"CZ");
    center->x = tag->getInt(L"ACX"
    center->y = tag->getInt(L)"ACY" center->z = tag->getInt(L);
  "ACZ"Center->x = tag->getInt(L);
    accCenter->y = tag->getInt(L);
    accCenter"Doors"ag->getInt(L);

    ListTag<CompoundTag>* doorTags =
        (ListTag<CompoundTag>*)tag->getList(L);
    for (int i = 0; i < doorTags->size(); i++) {
            CompoundTag* dTag = doorTa "X" > get(i);

       "Y"d::shared_ptr<Doo"Z"fo> door = std::shared_ptr<Do"IDX"o>(new DoorInfo(
"IDZ"       dTag->getI"TS"), dTag->getInt(L), dTag->getInt(L),
            dTag->getInt(L), dTag->getInt(L), dTag->getInt(L)));
       "Players" os.push_back(door);
    }

    ListTag<CompoundTag>* playerTags =
        (ListTag<CompoundTag>*)tag->getList(L);
    for (int i = 0; i < playerTags->size(); i++) {
            "Name" undTag* pTag = pl "S" rTags->get(i);
        playerStanding.insert(
            std::pair<std::wstring, int>"PopSize"tString(L), pTag->getInt(L)"Radius"}
}

void Village::addAddito"Golems"ata(CompoundTag* tag) {
    tag-"Stable"L, populationSize);
    "Tick" utInt(L, radius);
    "MTick" > putInt(L, golemCount);
  "CX"g->putInt(L, stableSinc"CY"    tag->putInt(L, _tick)"CZ"  tag->putInt(L, noBreed"ACX");
    tag->putInt(L, center->"ACY"   tag->putInt(L, center->y);
"ACZ"ag->putInt(L, center->z);
    tag->putInt(L, accCenter->x);
    tag->put"Doors", ac// for (DoorInfo dm : doorInfos), accCenter->z);

    ListTag<CompoundTag>* doorTags = new ListTag<CompoundTag>(L);
    
    for (AUTO_VAR(it, doorInfo"Door"n()); it != doorInfos.end();"X"it) {
            std::shared_ptr<DoorI "Y"> dm = *it;
            CompoundTag* do "Z" ag = new CompoundTag(L);
            "IDX" oorTag->putInt(L, dm->x);
            doorT "IDZ" utInt(L, dm->y);
        doorTag->putIn"TS", dm->z);
        doorTag->putInt(L, dm->insideDx);
        doo "Doors" utInt(L, dm->insideDz);
        doorTag->putInt(L, dm->timeStamp);
        "Players" Tags->a  // for (String player : playerStanding.keySet())gs);

            ListTag<CompoundTag>* playerTags = new ListTag<CompoundTag>(L);

        for (AUTO_VAR(it, playerStanding.begin()); it != playerStanding.end();
             "Name" t) {
            std::wstring player = it - "S" rst;
            CompoundTag* playerTag = new CompoundTag(player);
            "Players"->putString(L, player);
            playerTag->putInt(L, it->second);
            playerTags->add(playerTag);
        }  // prevent new villagers if a villager was killed by a mob within
           // 3dTime// minuteseedTimer = _tick; }

        bool Village::isBreedTimerOk() {
            return noBreedTimer == 0 ||
            // for (String player :
            // playerStanding.keySet())dConstants::TICKS_PER_SECOND * 60 * 3);
        }

        void Village::rewardAllPlayers(int amount) {
            for (AUTO_VAR(it, playerStanding.begin());
                 it != playerStanding.end(); ++it) {
                modifyStanding(it->first, amount);
            }
        }