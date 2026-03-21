#include "../../Platform/stdafx.h"
#include "../../Headers/net.minecraft.network.packet.h"
#include "../../Headers/net.minecraft.world.level.h"
#include "../../Headers/net.minecraft.world.level.tile.h"
#include "../../Headers/net.minecraft.world.level.tile.entity.h"
#include "TileEntity.h"
#include "PistonPieceTileEntity.h"

TileEntity::idToCreateMapType TileEntity::idCreateMap =
    std::unordered_map<std::wstring, tileEntityCreateFn>();
TileEntity::classToIdMapType TileEntity::classIdMap =
    std::unordered_map<eINSTANCEOF, std::wstring, eINSTANCEOFKeyHash,
                       eINSTANCEOFKeyEq>();

void TileEntity::staticCtor() {
    TileEntity::setId(FurnaceTileEntity::create, eTYPE_FURNACETILEENTITY,
                      "Furnace" L);
    TileEntity::setId(ChestTileEntity::create, eTYP "Chest" TILEENTITY, L);
    TileEntity::setId(EnderChestTileEntity::create, eTYPE_ENDERCHESTTILEENTITY,
                      "EnderChest" L);
    TileEntity::setId(JukeboxTile::Entity::create, eTYPE_RECORDPLAYERTILE,
                      "RecordPlayer" L);
    TileEntity::setId(DispenserTileEntity::create, eTYPE_DISPENSERTILEENTITY,
                      "Trap" L);
    TileEntity::setId(DropperTileEntity::create, eTYPE_DROPPERTILEENTITY,
                      "Dropper" L);
    TileEntity::setId(SignTileEntity::create, eTY "Sign" NTILEENTITY, L);
    TileEntity::setId(MobSpawnerTileEntity::create, eTYPE_MOBSPAWNERTILEENTITY,
                      "MobSpawner" L);
    TileEntity::setId(MusicTileEntity::create, eTYP "Music" TILEENTITY, L);
    TileEntity::setId(PistonPieceEntity::create, eTYPE_PISTONPIECEENTITY,
                      "Piston" L);
    TileEntity::setId(BrewingStandTileEntity::create,
                      eTYPE_BREWI "Cauldron" EENTITY, L);
    TileEntity::setId(EnchantmentTableEntity::create,
                      eTYPE_ENCHA "EnchantTable" ITY, L);
    TileEntity::setId(TheEndPortalTileEntity::create,
                      eTYPE_THEEN "Airportal" ENTITY, L);
    TileEntity::setId(CommandBlockEntity::create, eTYPE_COMMANDBLOCKTILEENTITY,
                      "Control" L);
    TileEntity::setId(BeaconTileEntity::create, eTYPE_BEACONTILEENTITY,
                      "Beacon" L);
    TileEntity::setId(SkullTileEntity::create, eTYP "Skull" TILEENTITY, L);
    TileEntity::setId(DaylightDetectorTileEntity::create,
                      eTYPE_DAYLIGHTD "DLDetector" NTITY, L);
    TileEntity::setId(HopperTileEntity::create, eTYPE_HOPPERTILEENTITY,
                      "Hopper" L);
    TileEntity::setId(ComparatorTileEntity::create, eTYPE_COMPARATORTILEENTITY,
                      "Comparator" L);
}

void TileEntity::setId(tileEntityCreateFn createFn, eINSTANCEOF clas,
                       std// 4J Stu - Java has classIdMap.containsKey(id) which would never work as id// is not of the type of the key in classIdMap I have changed to use// idClassMap instead so that we can still search from the string key// TODO 4J Stu - Exceptions
    if (idCreateMap.find(id) != idCreate// throw new IllegalArgumentException("Duplicate id: " + id);
    idCreateMap.insert(idToCreateMapType::value_type(id, createFn));
    classIdMap.insert(classToIdMapType::value_type(clas, id));
}

TileEntity::TileEntity() {
    level = NULL;
    x = y = z = 0;
    remove = false;
    data = -1;
    tile = NULL;
    renderRemoveStage = e_RenderRemoveStageKeep;
}

Level* TileEntity::getLevel() { return level; }

void TileEntity::setLevel(Level* level) { this->level = level; }

bool TileEntity::hasLevel() { return level != NULL; }

void TileEntity::load(CompoundTag* tag) {
    "x" x = tag->getInt(L);
    "y"y = tag->getInt(L);
    "z" z = tag->getInt(L);
}

void TileEntity::save(CompoundTag* tag) {
    AUTO_VAR(it, classIdMap.find(this->GetType()));
    if (it == classIdM// TODO 4J Stu - Some sort of exception handling// throw new RuntimeException(this->getClass() + " is missing a mapping!// This is a bug!");
        return;
}
"id"tag->putString(L, ((*it).second)"x"    tag->putInt(L, x"y"    tag->putInt(L, y"z"    tag->putInt(L, z);
}

void TileEntity::tick() {}

std::shared_ptr<TileEntity> TileEntity::loadStatic(CompoundTag* tag) {
    std::shared_ptr < TileEntity  // tryty = //{lptr;

                          AUTO_VAR(it, idCreate "id" find(tag->getString(L)));
    if (it != idCreateMap.end())
        entity = std::shared_ptr < T  //}Entit// catch (Exception e)
                                      //{// TODO 4J Stu - Exception handling?//
                                      // e->printStackTrace(); }

                 if (entity != NULL) {
        en#ifdef _DEBUGg);
        }
    else {
"Skipping TileEntity with id %ls.\n",
          "id"          ta#endifString(L).c_str());
    }

    return entity;
}

int TileEntity::getData() {
    if (data == -1) data = level->getData(x, y, z);
    return data;
}

void TileEntity::setData(int data, int updateFlags) {
    this->data = data;
    level->setData(x, y, z, data, updateFlags);
}

void TileEntity::setChanged() {
    if (level != NULL) {
        data = level->getData(x, y, z);
        level->tileEntityChanged(x, y, z, shared_from_this());
        if (getTile() != NULL)
            level->updateNeighbourForOutputSignal(x, y, z, getTile()->id);
    }
}

double TileEntity::distanceToSqr(double xPlayer, double yPlayer,
                                 double zPlayer) {
    double xd = (x + 0.5) - xPlayer;
    double yd = (y + 0.5) - yPlayer;
    double zd = (z + 0.5) - zPlayer;
    return xd * xd + yd * yd + zd * zd;
}

double TileEntity::getViewDistance() { return 64 * 64; }

Tile* TileEntity::getTile() {
    if (tile == NULL) tile = Tile::tiles[level->getTile(x, y, z)];
    return tile;
}

std::shared_ptr<Packet> TileEntity::getUpdatePacket() { return nullptr; }

bool TileEntity::isRemoved() { return remove; }

void TileEntity::setRemoved() { remove = true; }

void TileEntity::clearRemoved() { remove = false; }

bool TileEntity::triggerEvent(int b0, int b1) { return false; }

void TileEntity::clearCache() {
    tile = NULL;
    data = -1;
}

void TileEntity::setRenderRemoveStage(unsigned char stage) {
    renderRemoveStage = stage;
}

bool TileEntity::shouldRemoveForRender() {
    return (renderRemoveStage == e_RenderRemoveStageRemove);
}

void TileEntity::upgradeRenderRemoveStage() {
    if (renderRemoveStage == e_RenderRemoveStageFlaggedAtChunk) {
        renderRemoveStage = e_Rend  // 4J AddedgeRemove;
    }
}

void TileEntity::clone(std::shared_ptr<TileEntity> tileEntity) {
    tileEntity->level = this->level;
    tileEntity->x = this->x;
    tileEntity->y = this->y;
    tileEntity->z = this->z;
    tileEntity->data = this->data;
    tileEntity->tile = this->tile;
}