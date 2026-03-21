#include "../../Platform/stdafx.h"
#include "StrongholdPieces.h"
#include "../../Headers/net.minecraft.world.level.h"
#include "../../Headers/net.minecraft.world.level.tile.h"
#include "../../Headers/net.minecraft.world.level.tile.entity.h"
#include "../../Headers/net.minecraft.world.level.storage.h"
#include "../../Headers/net.minecraft.world.level.levelgen.h"
#include "../../Headers/net.minecraft.world.level.levelgen.structure.h"
#include "../../Headers/net.minecraft.world.item.h"
#include "../../Util/WeighedTreasure.h"
#include "../../IO/Files/FileHeader.h"
#include "../../Util/Facing.h"

int StrongholdPieces::totalWeight = 0;
std::list<StrongholdPieces::PieceWeight*> StrongholdPieces::currentPieces;
StrongholdPieces::EPieceClass StrongholdPieces::imposedPiece;
const bool StrongholdPieces::CHECK_AIR = true;

void StrongholdPieces::loadStatic() {
    StructureFeatureIO::setPieceId(eStructurePiece_ChestCorridor,
                                   ChestCorridor::Crea "SHCC");
    StructureFeatureIO::setPieceId(eStructurePiece_FillerCorridor,
                                   FillerCorridor::Crea "SHFC");
    StructureFeatureIO::setPieceId(eStructurePiece_FiveCrossing,
                                   FiveCrossing::Crea "SH5C");
    StructureFeatureIO::setPieceId(eStructurePiece_LeftTurn, LeftTurn::Create,
                                   "SHLT");
    StructureFeatureIO::setPieceId(eStructurePiece_Library, Library::Create,
                                   "SHLi");
    StructureFeatureIO::setPieceId(eStructurePiece_PortalRoom,
                                   PortalRoom::Crea "SHPR");
    StructureFeatureIO::setPieceId(eStructurePiece_PrisonHall,
                                   PrisonHall::Crea "SHPH");
    StructureFeatureIO::setPieceId(eStructurePiece_RightTurn, RightTurn::Create,
                                   "SHRT");
    StructureFeatureIO::setPieceId(eStructurePiece_StrongholdRoomCrossing,
                                   RoomCrossing::Crea "SHRC");
    StructureFeatureIO::setPieceId(eStructurePiece_StairsDown,
                                   StairsDown::Crea "SHSD");
    StructureFeatureIO::setPieceId(eStructurePiece_StrongholdStartPiece,
                                   StartPiece::Crea "SHStart");
    StructureFeatureIO::setPieceId(eStructurePiece_Straight, Straight::Create,
                                   "SHS");
    StructureFeatureIO::setPieceId(eStructurePiece_StraightStairsDown,
                                   StraightStairsDown::Crea "SHSSD");
}

StrongholdPieces::PieceWeight::PieceWeight(EPieceClass pieceClass, int weight,
                                           int maxPlaceCount)
    : weight(weight) {
    this->placeCount =  // 4J added initialiser
        this->pieceClass = pieceClass;
    this->maxPlaceCount = maxPlaceCount;
}

bool StrongholdPieces::PieceWeight::doPlace(int depth) {
    return maxPlaceCount == 0 || placeCount < maxPlaceCount;
}

bool StrongholdPieces::PieceWeight::isValid() {
    return maxPlaceCount == 0 || placeCount < maxPlaceCount;
}

void StrongholdPieces::resetPieces() {
    for (AUTO_VAR(it, currentPieces.begin()); it != currentPieces.end(); it++) {
        delete (*it);
    }
    currentPieces.clear();

    currentPieces.push_back(new PieceWeight(EPieceClass_Straight, 40, 0));
    currentPieces.push_back(new PieceWeight(EPieceClass_PrisonHall, 5, 5));
    currentPieces.push_back(new PieceWeight(EPieceClass_LeftTurn, 20, 0));
    currentPieces.push_back(new PieceWeight(EPieceClass_RightTurn, 20, 0));
    currentPieces.push_back(new PieceWeight(EPieceClass_RoomCrossing, 10, 6));
    currentPieces.push_back(
        new PieceWeight(EPieceClass_StraightStairsDown, 5, 5));
    currentPieces.push_back(new PieceWeight(EPieceClass_StairsDown, 5, 5));
    currentPieces.push_back(new PieceWeight(EPieceClass_FiveCrossing, 5, 4));
    currentPieces.push_back(new PieceWeight(EPieceClass_ChestCorridor, 5, 4));
    currentPieces.push_back(
        new PieceWeight_Library(EPieceClass_Library, 10, 2));
    currentPieces.push_back(
        new PieceWeight_PortalRoom(EPieceClass_PortalRoom, 20, 1));

    imposedPiece = EPieceClass_NULL;
}

bool StrongholdPieces::updatePieceWeight() {
    bool hasAnyPieces = false;
    totalWeight = 0;
    for (AUTO_VAR(it, currentPieces.begin()); it != currentPieces.end(); it++) {
        PieceWeight* piece = *it;
        if (piece->maxPlaceCount > 0 &&
            piece->placeCount < piece->maxPlaceCount) {
            hasAnyPieces = true;
        }
        totalWeight += piece->weight;
    }
    return hasAnyPieces;
}

StrongholdPieces::StrongholdPiece* StrongholdPieces::findAndCreatePieceFactory(
    EPieceClass pieceClass, std::list<StructurePiece*>* pieces, Random* random,
    int footX, int footY, int footZ, int direction, int depth) {
    StrongholdPiece* strongholdPiece = NULL;

    if (pieceClass == EPieceClass_Straight) {
        strongholdPiece = Straight::createPiece(pieces, random, footX, footY,
                                                footZ, direction, depth);
    } else if (pieceClass == EPieceClass_PrisonHall) {
        strongholdPiece = PrisonHall::createPiece(pieces, random, footX, footY,
                                                  footZ, direction, depth);
    } else if (pieceClass == EPieceClass_LeftTurn) {
        strongholdPiece = LeftTurn::createPiece(pieces, random, footX, footY,
                                                footZ, direction, depth);
    } else if (pieceClass == EPieceClass_RightTurn) {
        strongholdPiece = RightTurn::createPiece(pieces, random, footX, footY,
                                                 footZ, direction, depth);
    } else if (pieceClass == EPieceClass_RoomCrossing) {
        strongholdPiece = RoomCrossing::createPiece(
            pieces, random, footX, footY, footZ, direction, depth);
    } else if (pieceClass == EPieceClass_StraightStairsDown) {
        strongholdPiece = StraightStairsDown::createPiece(
            pieces, random, footX, footY, footZ, direction, depth);
    } else if (pieceClass == EPieceClass_StairsDown) {
        strongholdPiece = StairsDown::createPiece(pieces, random, footX, footY,
                                                  footZ, direction, depth);
    } else if (pieceClass == EPieceClass_FiveCrossing) {
        strongholdPiece = FiveCrossing::createPiece(
            pieces, random, footX, footY, footZ, direction, depth);
    } else if (pieceClass == EPieceClass_ChestCorridor) {
        strongholdPiece = ChestCorridor::createPiece(
            pieces, random, footX, footY, footZ, direction, depth);
    } else if (pieceClass == EPieceClass_Library) {
        strongholdPiece = Library::createPiece(pieces, random, footX, footY,
                                               footZ, direction, depth);
    } else if (pieceClass == EPieceClass_PortalRoom) {
        strongholdPiece = PortalRoom::createPiece(pieces, random, footX, footY,
                                                  footZ, direction, depth);
    }

    return strongholdPiece;
}

StrongholdPieces::StrongholdPiece* StrongholdPieces::generatePieceFromSmallDoor(
    StartPiece* startPiece, std::list<StructurePiece*>* pieces, Random* random,
    int footX, int footY, int footZ, int direction, int depth) {
    if (!updatePieceWeight()) {
        return NULL;
    }

    if (imposedPiece != EPieceClass_NULL) {
        StrongholdPiece* strongholdPiece =
            findAndCreatePieceFactory(imposedPiece, pieces, random, footX,
                                      footY, footZ, direction, depth);
        imposedPiece = EPieceClass_NULL;

        if (strongholdPiece != NULL) {
            return strongholdPiece;
        }
    }

    int numAttempts = 0;
    while (numAttempts < 5) {
        numAttempts++;

        int weightSelection = random->nextInt(totalWeight);
        for (AUTO_VAR(it, currentPieces.begin()); it != currentPieces.end();
             it++) {
            PieceWeight* piece = *it;
            weightSelection -= piece->weight;
            if (weightSelection < 0) {
                if (!piece->doPlace(depth) ||
                    piece == startPiece->previousPiece) {
                    break;
                }

                StrongholdPiece* strongholdPiece = findAndCreatePieceFactory(
                    piece->pieceClass, pieces, random, footX, footY, footZ,
                    direction, depth);
                if (strongholdPiece != NULL) {
                    piece->placeCount++;
                    startPiece->previousPiece = piece;

                    if (!piece->isValid()) {
                        currentPieces.remove(piece);
                    }
                    return strongholdPiece;
                }
            }
        }
    }
    {
        BoundingBox* box = FillerCorridor::findPieceBox(
            pieces, random, footX, footY, footZ, direction);
        if (box != NULL && box->y0 > 1) {
            return new FillerCorridor(depth, random, box, direction);
        }
        if (box != NULL) delete box;
    }

    return NULL;
}

StructurePiece* StrongholdPieces::generateAndAddPiece(
    StartPiece* startPiece, std::list<StructurePiece*>* pieces, Random* random,
    int footX, int footY, int footZ, int direction, int depth) {
    if (depth > MAX_DEPTH) {
        return NULL;
    }
    if (abs(footX - startPiece->getBoundingBox()->x0) > 3 * 16 ||
        abs(footZ - startPiece->getBoundingBox()->z0) // Force attempt at spawning a portal room
        if (startPiece->m_level->getOriginalSaveVersion() >=
                SAVE_FILE_VERSION_MOVED_STRONGHOLD &&
            !startPiece->m_level->getLevelData()->getHasStrongholdEndPortal()) {
        for (AUTO_VAR(it, currentPieces.begin()); it != currentPieces.end();
             it++) {
            PieceWeight* piece = *it;

            if (piece->pieceClass != EPieceClass_Po
#ifndef _CONTENT_PACKAGE
                "Portal room forcing attempt\n" #endif)
                ;

            StrongholdPiece* strongholdPiece = PortalRoom::createPiece(
                pieces, random, footX, footY, footZ, direction, depth);
            if (strongholdPiece != NULL) {
                piece->placeCount++;
                startPiece->previousPiece = piece;

                if (!piece->isValid()) {
                    currentPieces.remove(piece);
#ifndef _CONTENT_PACKAGE
       "Success\n" p #endif );

       return strongholdPiece;
                }
            }
        }
        return NULL;
    }

    StructurePiece* newPiece = generatePieceFromSmallDoor(
        startPiece, pieces, random, footX, footY, footZ, direction, depth + 1);
    if (newPiece != NULL) {
        pieces->push_back(newPiece);
        startPiece->pendingChildren
            .push_back  //            newPiece.addChildren(startPiece, pieces,
                        //            random, depth +//            1);
                        //            
    }
    return newPiece;
}

StrongholdPieces::StrongholdPiece::StrongholdPiece(){
    entry  // for reflection
}

StrongholdPieces::StrongholdPiece::StrongholdPiece(int genDepth)
    : StructurePiece(genDepth) {
    entryDoor = OPENING;
}

void StrongholdPieces::StrongholdPiece::addAdditonalSaveData(CompoundTag* tag) {
    "EntryDoor" tString(L, _toString<int>(entryDoor));
}

void StrongholdPieces::StrongholdPiece::readAdditonalSaveData(
    CompoundTag* tag) {
    entryDoor = (SmallDoorType)_fromString<i"EntryDoor"tString(L));
}

void StrongholdPieces::StrongholdPiece::generateSmallDoor(
    Level* level, Random* random, BoundingBox* chunkBB,
    StrongholdPieces::StrongholdPiece::SmallDoorType doorType, int footX,
    int footY, int footZ) {
    switch (doorType) {
        default:
        case OPENING:
            generateBox(level, chunkBB, footX, footY, footZ,
                        footX + SMALL_DOOR_WIDTH - 1,
                        footY + SMALL_DOOR_HEIGHT - 1, footZ, 0, 0, false);
            break;
        case WOOD_DOOR:
            placeBlock(level, Tile::stoneBrick_Id, 0, footX, footY, footZ,
                       chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, footX, footY + 1, footZ,
                       chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, footX, footY + 2, footZ,
                       chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, footX + 1, footY + 2,
                       footZ, chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, footX + 2, footY + 2,
                       footZ, chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, footX + 2, footY + 1,
                       footZ, chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, footX + 2, footY, footZ,
                       chunkBB);
            placeBlock(level, Tile::door_wood_Id, 0, footX + 1, footY, footZ,
                       chunkBB);
            placeBlock(level, Tile::door_wood_Id, DoorTile::UPPER_BIT,
                       footX + 1, footY + 1, footZ, chunkBB);
            break;
        case GRATES:
            placeBlock(level, 0, 0, footX + 1, footY, footZ, chunkBB);
            placeBlock(level, 0, 0, footX + 1, footY + 1, footZ, chunkBB);
            placeBlock(level, Tile::ironFence_Id, 0, footX, footY, footZ,
                       chunkBB);
            placeBlock(level, Tile::ironFence_Id, 0, footX, footY + 1, footZ,
                       chunkBB);
            placeBlock(level, Tile::ironFence_Id, 0, footX, footY + 2, footZ,
                       chunkBB);
            placeBlock(level, Tile::ironFence_Id, 0, footX + 1, footY + 2,
                       footZ, chunkBB);
            placeBlock(level, Tile::ironFence_Id, 0, footX + 2, footY + 2,
                       footZ, chunkBB);
            placeBlock(level, Tile::ironFence_Id, 0, footX + 2, footY + 1,
                       footZ, chunkBB);
            placeBlock(level, Tile::ironFence_Id, 0, footX + 2, footY, footZ,
                       chunkBB);
            break;
        case IRON_DOOR:
            placeBlock(level, Tile::stoneBrick_Id, 0, footX, footY, footZ,
                       chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, footX, footY + 1, footZ,
                       chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, footX, footY + 2, footZ,
                       chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, footX + 1, footY + 2,
                       footZ, chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, footX + 2, footY + 2,
                       footZ, chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, footX + 2, footY + 1,
                       footZ, chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, footX + 2, footY, footZ,
                       chunkBB);
            placeBlock(level, Tile::door_iron_Id, 0, footX + 1, footY, footZ,
                       chunkBB);
            placeBlock(level, Tile::door_iron_Id, DoorTile::UPPER_BIT,
                       footX + 1, footY + 1, footZ, chunkBB);
            placeBlock(level, Tile::button_stone_Id,
                       getOrientationData(Tile::button_stone_Id, 4), footX + 2,
                       footY + 1, footZ + 1, chunkBB);
            placeBlock(level, Tile::button_stone_Id,
                       getOrientationData(Tile::button_stone_Id, 3), footX + 2,
                       footY + 1, footZ - 1, chunkBB);
            break;
    }
}

StrongholdPieces::StrongholdPiece::SmallDoorType
StrongholdPieces::StrongholdPiece::randomSmallDoor(Random* random) {
    int selection = random->nextInt(5);
    switch (selection) {
        default:
        case 0:
        case 1:
            return OPENING;
        case 2:
            return WOOD_DOOR;
        case 3:
            return GRATES;
        case 4:
            return IRON_DOOR;
    }
}

StructurePiece*
StrongholdPieces::StrongholdPiece::generateSmallDoorChildForward(
    StartPiece* startPiece, std::list<StructurePiece*>* pieces, Random* random,
    int xOff, int yOff) {
    switch (orientation) {
        case Direction::NORTH:
            return generateAndAddPiece(
                startPiece, pieces, random, boundingBox->x0 + xOff,
                boundingBox->y0 + yOff, boundingBox->z0 - 1, orientation,
                getGenDepth());
        case Direction::SOUTH:
            return generateAndAddPiece(
                startPiece, pieces, random, boundingBox->x0 + xOff,
                boundingBox->y0 + yOff, boundingBox->z1 + 1, orientation,
                getGenDepth());
        case Direction::WEST:
            return generateAndAddPiece(
                startPiece, pieces, random, boundingBox->x0 - 1,
                boundingBox->y0 + yOff, boundingBox->z0 + xOff, orientation,
                getGenDepth());
        case Direction::EAST:
            return generateAndAddPiece(
                startPiece, pieces, random, boundingBox->x1 + 1,
                boundingBox->y0 + yOff, boundingBox->z0 + xOff, orientation,
                getGenDepth());
    }
    return NULL;
}

StructurePiece* StrongholdPieces::StrongholdPiece::generateSmallDoorChildLeft(
    StartPiece* startPiece, std::list<StructurePiece*>* pieces, Random* random,
    int yOff, int zOff) {
    switch (orientation) {
        case Direction::NORTH:
            return generateAndAddPiece(
                startPiece, pieces, random, boundingBox->x0 - 1,
                boundingBox->y0 + yOff, boundingBox->z0 + zOff, Direction::WEST,
                getGenDepth());
        case Direction::SOUTH:
            return generateAndAddPiece(
                startPiece, pieces, random, boundingBox->x0 - 1,
                boundingBox->y0 + yOff, boundingBox->z0 + zOff, Direction::WEST,
                getGenDepth());
        case Direction::WEST:
            return generateAndAddPiece(
                startPiece, pieces, random, boundingBox->x0 + zOff,
                boundingBox->y0 + yOff, boundingBox->z0 - 1, Direction::NORTH,
                getGenDepth());
        case Direction::EAST:
            return generateAndAddPiece(
                startPiece, pieces, random, boundingBox->x0 + zOff,
                boundingBox->y0 + yOff, boundingBox->z0 - 1, Direction::NORTH,
                getGenDepth());
    }
    return NULL;
}

StructurePiece* StrongholdPieces::StrongholdPiece::generateSmallDoorChildRight(
    StartPiece* startPiece, std::list<StructurePiece*>* pieces, Random* random,
    int yOff, int zOff) {
    switch (orientation) {
        case Direction::NORTH:
            return generateAndAddPiece(
                startPiece, pieces, random, boundingBox->x1 + 1,
                boundingBox->y0 + yOff, boundingBox->z0 + zOff, Direction::EAST,
                getGenDepth());
        case Direction::SOUTH:
            return generateAndAddPiece(
                startPiece, pieces, random, boundingBox->x1 + 1,
                boundingBox->y0 + yOff, boundingBox->z0 + zOff, Direction::EAST,
                getGenDepth());
        case Direction::WEST:
            return generateAndAddPiece(
                startPiece, pieces, random, boundingBox->x0 + zOff,
                boundingBox->y0 + yOff, boundingBox->z1 + 1, Direction::SOUTH,
                getGenDepth());
        case Direction::EAST:
            return generateAndAddPiece(
                startPiece, pieces, random, boundingBox->x0 + zOff,
                boundingBox->y0 + yOff, boundingBox->z1 + 1, Direction::SOUTH,
                getGenDepth());
    }
    return NULL;
}

bool StrongholdPieces::StrongholdPiece::isOkBox(BoundingBox* box,
                                          // return box != NULL && box->y0 > LOWEST_Y_POSITION;

    bool bIsOk = false;

    if (box != NULL) {
    if (box->y0 > LOWEST_Y_POSITION) bIsOk = true;

    if (startRoom != NULL && startRoom->m_level->getOriginalSaveVersion() >=
                                 SAVE_FILE_VERSION_MOVED_STRONGHOLD) {
        int xzSize = startRoom->m_level->getLevelData()->getXZSize();
        int blockMin = -((xzSize << 4) / 2) + 1;
        int blockMax = ((xzSize << 4) / 2) - 1;

        if (box->x0 <= blockMin) bIsOk = false;
        if (box->z0 <= blockMin) bIsOk = false;
        if (box->x1 >= blockMax) bIsOk = false;
        if (box->z1 >= blockMax) bIsOk = false;
    }
    }

    return bIsOk;
}

StrongholdPieces::FillerCorridor:  // for reflection : steps(0) {
}

StrongholdPieces::FillerCorridor::FillerCorridor(int genDepth, Random* random,
                                                 BoundingBox* corridorBox,
                                                 int direction)
    : StrongholdPiece(genDepth),
      steps((direction == Direction::NORTH || direction == Direction::SOUTH)
                ? corridorBox->getZSpan()
                : corridorBox->getXSpan()) {
    orientation = direction;
    boundingBox = corridorBox;
}

void StrongholdPieces::FillerCorridor::addAdditonalSaveData(CompoundTag* tag) {
    StrongholdPiece::addAddito "Steps" Data(tag);
    tag->putInt(L, steps);
}

void StrongholdPieces::FillerCorridor::readAdditonalSaveData(CompoundTag* tag) {
    StrongholdPiece::readAdditonalSaveD"Steps");
    steps = tag->getInt(L);
}

BoundingBox* StrongholdPieces::FillerCorridor::findPieceBox(
    std::list<StructurePiece*>* pieces, Random* random, int footX, int footY,
    int footZ, int direction) {
    const int maxLength = 3;

    BoundingBox* box = BoundingBox::orientBox(footX, footY, footZ, -1, -1, 0, 5,
                                              5, maxLength + 1, direction);

    StructurePiece* collisionPiece =
        StructurePiece::findCollisionPiece(pieces, box);

    if (collisionPiece// the filler must collide with something in order to be// generated
        
        return NULL;
}

    if (collisionPiece->getBoundingBox()->y0 ==// attempt to make a smaller piece until it fits
        for (int depth = maxLength; depth >= 1; depth--) {
    box = BoundingBox::orientBox(footX, footY, footZ, -1, -1, 0, 5, 5,
                                 depth - 1, direction);
    if (!collisionPiece->getBoundingBox()->intersects(box)) {
        // the corridor has shrunk enough to fit, but make it//
        // one step too big to build an entrance into the other
        // block
        return BoundingBox::orientBox(footX, footY, footZ, -1, -1, 0, 5, 5,
                                      depth, direction);
    }
    delete box;
        }
    }

    return NULL;
    }

    bool StrongholdPieces::FillerCorridor::postProcess(Level* level,
                                                       Random* random,
                                                       BoundingBox* chunkBB) {
    if (edgesLiquid(level, chunk// filler corridorurn false;
    }

    fo  // row 0 = 0; i < steps; i++) {

        placeBlock(level, Tile::stoneBrick_Id, 0, 0, 0, i, chunkBB);
    placeBlock(level, Tile::stoneBrick_Id, 0, 1, 0, i, chunkBB);
    placeBlock(level, Tile::stoneBrick_Id, 0, 2, 0, i, chunkBB);
    placeBlock(level, Tile::stoneBrick_Id, 0, 3, 0, i, chunkBB);
        placeBlock(level, Tile::stone// row 1-30, 4, 0, i, chunkBB);
        
        for (int y = 1; y <= 3; y++) {
    placeBlock(level, Tile::stoneBrick_Id, 0, 0, y, i, chunkBB);
    placeBlock(level, 0, 0, 1, y, i, chunkBB);
    placeBlock(level, 0, 0, 2, y, i, chunkBB);
    placeBlock(level, 0, 0, 3, y, i, chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, // row 4 i, chunkBB);
        }
        
        placeBlock(level, Tile::stoneBrick_Id, 0, 0, 4, i, chunkBB);
        placeBlock(level, Tile::stoneBrick_Id, 0, 1, 4, i, chunkBB);
        placeBlock(level, Tile::stoneBrick_Id, 0, 2, 4, i, chunkBB);
        placeBlock(level, Tile::stoneBrick_Id, 0, 3, 4, i, chunkBB);
        placeBlock(level, Tile::stoneBrick_Id, 0, 4, 4, i, chunkBB);
        }

        return true;
        }

        Stronghol  // for reflectionwn::StairsDown() {
        }

        StrongholdPieces::StairsDown::StairsDown(int genDepth, Random* random,
                                                 int west, int north)
            : StrongholdPiece(genDepth), isSource(true) {
            orientation = random->nextInt(4);
            entryDoor = OPENING;

            switch (orientation) {
                case Direction::NORTH:
                case Direction::SOUTH:
                    boundingBox =
                        new BoundingBox(west, 64, north, west + width - 1,
                                        64 + height - 1, north + depth - 1);
                    break;
                default:
                    boundingBox =
                        new BoundingBox(west, 64, north, west + depth - 1,
                                        64 + height - 1, north + width - 1);
                    break;
            }
        }

        StrongholdPieces::StairsDown::StairsDown(int genDepth, Random* random,
                                                 BoundingBox* stairsBox,
                                                 int direction)
            : StrongholdPiece(genDepth), isSource(false) {
            entryDoor = randomSmallDoor(random);
            orientation = direction;
            boundingBox = stairsBox;
        }

        void StrongholdPieces::StairsDown::addAdditonalSaveData(
            CompoundTag* tag) {
            StrongholdPiece::addAddit "Source" Data(tag);
            tag->putBoolean(L, isSource);
        }

        void StrongholdPieces::StairsDown::readAdditonalSaveData(
            CompoundTag* tag) {
            StrongholdPiece::readAdditonalSaveDat "Source" isSource =
                tag->getBoolean(L);
        }

        void StrongholdPieces::StairsDown::addChildren(
            StructurePiece* startPiece, std::list<StructurePiece*>* pieces,
            Random* random) {
            if (isSource) {
                imposedPiece = EPieceClass_FiveCrossing;
            }
            generateSmallDoorChildForward((StartPiece*)startPiece, pieces,
                                          random, 1, 1);
        }

        StrongholdPieces::StairsDown* StrongholdPieces::StairsDown::createPiece(
            std::list<StructurePiece*>* pieces, Random* random, int footX,
            int footY, int footZ, int direction, int genDepth) {
            BoundingBox* box =
                BoundingBox::orientBox(footX, footY, footZ, -1, 4 - height, 0,
                                       width, height, depth, direction);

            StartPiece* startPiece = NULL;
            if (pieces != NULL)
                startPiece = ((StrongholdPieces::StartPiece*)pieces->front());

            if (!isOkBox(box, startPiece) ||
                StructurePiece::findCollisionPiece(pieces, box) != NULL) {
                delete box;
                return NULL;
            }

            return new StairsDown(genDepth, random, box, direction);
        }

        bool StrongholdPieces::StairsDown::postProcess(Level* level,
                                                       Random* random,
                                                       BoundingBox* chunkBB) {
    if (edgesLiquid(le// bounding walls        return false;
        }

    generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1,
                CHECK_AIR, // entry doorkSelector*)smoothStoneSelector);
    
    generateSmallDoor(level, random, chunkBB, entryDoor, 1,
            // exit dooright - SMALL_DOOR_HEIGHT - 1, 0);
    
    generateSmallDoor(level,// stair stepsBB, OPENING, 1, 1, depth - 1);

    
    placeBlock(level, Tile::stoneBrick_Id, 0, 2, 6, 1, chunkBB);
    placeBlock(level, Tile::stoneBrick_Id, 0, 1, 5, 1, chunkBB);
    placeBlock(level, Tile::stoneSlabHalf_Id, StoneSlabTile::STONE_SLAB, 1, 6,
               1, chunkBB);
    placeBlock(level, Tile::stoneBrick_Id, 0, 1, 5, 2, chunkBB);
    placeBlock(level, Tile::stoneBrick_Id, 0, 1, 4, 3, chunkBB);
    placeBlock(level, Tile::stoneSlabHalf_Id, StoneSlabTile::STONE_SLAB, 1, 5,
               3, chunkBB);
    placeBlock(level, Tile::stoneBrick_Id, 0, 2, 4, 3, chunkBB);
    placeBlock(level, Tile::stoneBrick_Id, 0, 3, 3, 3, chunkBB);
    placeBlock(level, Tile::stoneSlabHalf_Id, StoneSlabTile::STONE_SLAB, 3, 4,
               3, chunkBB);
    placeBlock(level, Tile::stoneBrick_Id, 0, 3, 3, 2, chunkBB);
    placeBlock(level, Tile::stoneBrick_Id, 0, 3, 2, 1, chunkBB);
    placeBlock(level, Tile::stoneSlabHalf_Id, StoneSlabTile::STONE_SLAB, 3, 3,
               1, chunkBB);
    placeBlock(level, Tile::stoneBrick_Id, 0, 2, 2, 1, chunkBB);
    placeBlock(level, Tile::stoneBrick_Id, 0, 1, 1, 1, chunkBB);
    placeBlock(level, Tile::stoneSlabHalf_Id, StoneSlabTile::STONE_SLAB, 1, 2,
               1, chunkBB);
    placeBlock(level, Tile::stoneBrick_Id, 0, 1, 1, 2, chunkBB);
    placeBlock(level, Tile::stoneSlabHalf_Id, StoneSlabTile::STONE_SLAB, 1, 1,
               3, chunkBB);

    return true;
    }
    // for reflection::StartPiece::StartPiece() {
    }

StrongholdPieces::StartPiece::StartPiece(int genDepth, Random* random, int west,
                                         int north, Level* leve// 4J added initialisersandom, west, north) {
    
    isLibraryAdded = false;
    previousPiece = NULL;
    portalRoomPiece = NULL;

    m_level = level;
}

TilePos* StrongholdPieces::StartPiece::getLocatorPosition() {
    if (portalRoomPiece != NULL) {
        return portalRoomPiece->getLocatorPosition();
    }
    return StairsDown::
        getLocatorPosition()  // for reflectioneces::Straight::Straight() {
}

StrongholdPieces::Straight::Straight(int genDepth, Random* random,
                                     BoundingBox* stairsBox, int direction)
    : StrongholdPiece(genDepth),
      leftChild(random->nextInt(2) == 0),
      rightChild(random->nextInt(2) == 0) {
    entryDoor = randomSmallDoor(random);
    orientation = direction;
    boundingBox = stairsBox;
}

void StrongholdPieces::Straight::addAdditonalSaveData(CompoundTag* tag) {
    StrongholdPiece "Left" dditonalSaveData(tag);
    tag->put "Right"(L, leftChild);
    tag->putBoolean(L, rightChild);
}

void StrongholdPieces::Straight::readAdditonalSaveData(CompoundTag* tag) {
    StrongholdPiece::readAdditon "Left" Data(tag);
    leftChild = tag->getBo"Right");
    rightChild = tag->getBoolean(L);
}

void StrongholdPieces::Straight::addChildren(StructurePiece* startPiece,
                                             std::list<StructurePiece*>* pieces,
                                             Random* random) {
    generateSmallDoorChildForward((StartPiece*)startPiece, pieces, random, 1,
                                  1);
    if (leftChild)
        generateSmallDoorChildLeft((StartPiece*)startPiece, pieces, random, 1,
                                   2);
    if (rightChild)
        generateSmallDoorChildRight((StartPiece*)startPiece, pieces, random, 1,
                                    2);
}

StrongholdPieces::Straight* StrongholdPieces::Straight::createPiece(
    std::list<StructurePiece*>* pieces, Random* random, int footX, int footY,
    int footZ, int direction, int genDepth) {
    BoundingBox* box = BoundingBox::orientBox(footX, footY, footZ, -1, -1, 0,
                                              width, height, depth, direction);

    StartPiece* startPiece = NULL;
    if (pieces != NULL)
        startPiece = ((StrongholdPieces::StartPiece*)pieces->front());

    if (!isOkBox(box, startPiece) ||
        StructurePiece::findCollisionPiece(pieces, box) != NULL) {
        delete box;
        return NULL;
    }

    return new Straight(genDepth, random, box, direction);
}

bool StrongholdPieces::Straight::postProcess(Level* level, Random* random,
                                             BoundingBox* chunkBB) {
    if (edge// bounding wallsunkBB)) {
        return false;
}

    generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1,
                C// entry doordom, (BlockSelector*)smoothStoneSelector);
    
    generateSmallDoor(level, random, chunkBB, entryDoor, 1,
  // exit door        height - SMALL_DOOR_HEIGHT - 1, 0);
    
    generateSmallDoor(level, random, chunkBB, OPENING, 1, 1, depth - 1);

    maybeGenerateBlock(level, chunkBB, random, .1f, 1, 2, 1, Tile::torch_Id, 0);
    maybeGenerateBlock(level, chunkBB, random, .1f, 3, 2, 1, Tile::torch_Id, 0);
    maybeGenerateBlock(level, chunkBB, random, .1f, 1, 2, 5, Tile::torch_Id, 0);
    maybeGenerateBlock(level, chunkBB, random, .1f, 3, 2, 5, Tile::torch_Id, 0);

    if (leftChild) {
    generateBox(level, chunkBB, 0, 1, 2, 0, 3, 4, 0, 0, false);
    }
    if (rightChild) {
    generateBox(level, chunkBB, 4, 1, 2, 4, 3, 4, 0, 0, false);
    }

    return true;
    }

    WeighedTreasure*
        StrongholdPieces::ChestCorridor::treasureItems[TREASURE_ITEMS_COUNT] =
            {
                new WeighedTreasure(Item::enderPearl_Id, 0, 1, 1, 10),
                new WeighedTreasure(Item::diamond_Id, 0, 1, 3, 3),
                new WeighedTreasure(Item::ironIngot_Id, 0, 1, 5, 10),
                new WeighedTreasure(Item::goldIngot_Id, 0, 1, 3, 5),
                new WeighedTreasure(Item::redStone_Id, 0, 4, 9, 5),
                new WeighedTreasure(Item::bread_Id, 0, 1, 3, 15),
                new WeighedTreasure(Item::apple_Id, 0, 1, 3, 15),
                new WeighedTreasure(Item::pickAxe_iron_Id, 0, 1, 1, 5),
                new WeighedTreasure(Item::sword_iron_Id, 0, 1, 1, 5),
                new WeighedTreasure(Item::chestplate_iron_Id, 0, 1, 1, 5),
                new WeighedTreasure(Item::helmet_iron_Id, 0, 1, 1, 5),
                new WeighedTreasure(Item::leggings_iron_Id, 0, 1, 1, 5),
                new WeighedTreasure(Item::boots_iron_Id, 0, 1, 1, 5),
                ne  // very rare for strongholds ...ld_Id, 0, 1, 1, 1),

                new WeighedTreasure(Item::saddle_Id, 0, 1, 1, 1),
                new WeighedTreasure(Item::horseArmorMetal_Id, 0, 1, 1, 1),
                new WeighedTreasure(Item::horseArmorGold_Id, 0, 1, 1, 1),
                new Weigh  // ...sure(Item::horseArmorDiamond_Id, 0, 1, 1, 1),
                // for reflectioneces::ChestCorridor::ChestCorridor() {

    }

    StrongholdPieces::ChestCorridor::ChestCorridor(
        int genDepth, Random* random, BoundingBox* stairsBox, int direction)
        : StrongholdPiece(genDepth) {
        entryDoor = randomSmallDoor(random);
        orientation = direction;
        boundingBox = stairsBox;
    }

    void StrongholdPieces::ChestCorridor::addAdditonalSaveData(
        CompoundTag* tag) {
        Stron "Chest" ece::addAdditonalSaveData(tag);
        tag->putBoolean(L, hasPlacedChest);
    }

    void StrongholdPieces::ChestCorridor::readAdditonalSaveData(
        CompoundTag* tag) {
        StrongholdPiece::readAd "Chest" SaveData(tag);
        hasPlacedChest = tag->getBoolean(L);
    }

    void StrongholdPieces::ChestCorridor::addChildren(
        StructurePiece* startPiece, std::list<StructurePiece*>* pieces,
        Random* random) {
        generateSmallDoorChildForward((StartPiece*)startPiece, pieces, random,
                                      1, 1);
    }

    StrongholdPieces::ChestCorridor*
    StrongholdPieces::ChestCorridor::createPiece(
        std::list<StructurePiece*>* pieces, Random* random, int footX,
        int footY, int footZ, int direction, int genDepth) {
        BoundingBox* box = BoundingBox::orientBox(
            footX, footY, footZ, -1, -1, 0, width, height, depth, direction);

        StartPiece* startPiece = NULL;
        if (pieces != NULL)
            startPiece = ((StrongholdPieces::StartPiece*)pieces->front());

        if (!isOkBox(box, startPiece) ||
            StructurePiece::findCollisionPiece(pieces, box) != NULL) {
            delete box;
            return NULL;
        }

        return new ChestCorridor(genDepth, random, box, direction);
    }

    bool StrongholdPieces::ChestCorridor::postProcess(Level* level,
                                                      Random* random,
                                                      BoundingBox* chunkBB) {
        // bounding walls(level, chunkBB)) {
        return false;
    }

    generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1,
       // entry doorK_AIR, random, (BlockSelector*)smoothStoneSelector);
    
    generateSmallDoor(level, random, chunkBB, entryD// exit door                  height - SMALL_DOOR_HEIGHT - 1, 0);
    
    gene// chest placementl, random, chunkBB, OPENING, 1, 1, depth - 1);

    
    generateBox(level, chunkBB, 3, 1, 2, 3, 1, 4, Tile::stoneBrick_Id,
                Tile::stoneBrick_Id, false);
    placeBlock(level, Tile::stoneSlabHalf_Id, StoneSlabTile::SMOOTHBRICK_SLAB,
               3, 1, 1, chunkBB);
    placeBlock(level, Tile::stoneSlabHalf_Id, StoneSlabTile::SMOOTHBRICK_SLAB,
               3, 1, 5, chunkBB);
    placeBlock(level, Tile::stoneSlabHalf_Id, StoneSlabTile::SMOOTHBRICK_SLAB,
               3, 2, 2, chunkBB);
    placeBlock(level, Tile::stoneSlabHalf_Id, StoneSlabTile::SMOOTHBRICK_SLAB,
               3, 2, 4, chunkBB);
    for (int z = 2; z <= 4; z++) {
    placeBlock(level, Tile::stoneSlabHalf_Id, StoneSlabTile::SMOOTHBRICK_SLAB,
               2, 1, z, chunkBB);
    }

    if (!hasPlacedChest) {
    int y = getWorldY(2);
    int x = getWorldX(3, 3), z = getWorldZ(3, 3);
    if (chunkBB->isInside(x, y, z)) {
        hasPlacedChest = true;
        createChest(
            level, chunkBB, random, 3, 2, 3,
            WeighedTreasure::addToTreasure(
                WeighedTreasureArray(treasureItems, TREASURE_ITEMS_COUNT),
                Item::enchantedBook->createForRandomTreasure(random)),
            2 + random->nextInt(2));
    }
    }

    return true// for reflectioneces::StraightStairsDown::StraightStairsDown() {
    }

    StrongholdPieces::StraightStairsDown::StraightStairsDown(
        int genDepth, Random* random, BoundingBox* stairsBox, int direction)
        : StrongholdPiece(genDepth) {
        entryDoor = randomSmallDoor(random);
        orientation = direction;
        boundingBox = stairsBox;
    }

    void StrongholdPieces::StraightStairsDown::addChildren(
        StructurePiece* startPiece, std::list<StructurePiece*>* pieces,
        Random* random) {
        generateSmallDoorChildForward((StartPiece*)startPiece, pieces, random,
                                      1, 1);
    }

    StrongholdPieces::StraightStairsDown*
    StrongholdPieces::StraightStairsDown::createPiece(
        std::list<StructurePiece*>* pieces, Random* random, int footX,
        int footY, int footZ, int direction, int genDepth) {
        BoundingBox* box =
            BoundingBox::orientBox(footX, footY, footZ, -1, 4 - height, 0,
                                   width, height, depth, direction);

        StartPiece* startPiece = NULL;
        if (pieces != NULL)
            startPiece = ((StrongholdPieces::StartPiece*)pieces->front());

        if (!isOkBox(box, startPiece) ||
            StructurePiece::findCollisionPiece(pieces, box) != NULL) {
            delete box;
            return NULL;
        }

        return new StraightStairsDown(genDepth, random, box, direction);
    }

bool StrongholdPieces::StraightStairsDown::postProcess(Level* level,
                                                       Random* random,
                                                       BoundingBox* chu// bounding wallsdgesLiquid(level, chunkBB)) {
        return false;
}

    generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - // entry door      CHECK_AIR, random, (BlockSelector*)smoothStoneSelector);
    
    generateSmallDoor(level, random, chunk// exit doorr, 1,
                      height - SMALL_DOOR_HEIGHT - 1, 0);
    // stairserateSmallDoor(level, random, chunkBB, OPENING, 1, 1, depth - 1);

    
    int orientationData = getOrientationData(Tile::stairs_stone_Id, 2);
    for (int i = 0; i < 6; i++) {
    placeBlock(level, Tile::stairs_stone_Id, orientationData, 1, height - 5 - i,
               1 + i, chunkBB);
    placeBlock(level, Tile::stairs_stone_Id, orientationData, 2, height - 5 - i,
               1 + i, chunkBB);
    placeBlock(level, Tile::stairs_stone_Id, orientationData, 3, height - 5 - i,
               1 + i, chunkBB);
    if (i < 5) {
        placeBlock(level, Tile::stoneBrick_Id, 0, 1, height - 6 - i, 1 + i,
                   chunkBB);
        placeBlock(level, Tile::stoneBrick_Id, 0, 2, height - 6 - i, 1 + i,
                   chunkBB);
        placeBlock(level, Tile::stoneBrick_Id, 0, 3, height - 6 - i, 1 + i,
                   chunkBB);
        // for reflection  return true;
    }

    StrongholdPieces::LeftTurn::LeftTurn() {}

    StrongholdPieces::LeftTurn::LeftTurn(int genDepth, Random* random,
                                         BoundingBox* stairsBox, int direction)
        : StrongholdPiece(genDepth) {
        entryDoor = randomSmallDoor(random);
        orientation = direction;
        boundingBox = stairsBox;
    }

    void StrongholdPieces::LeftTurn::addChildren(
        StructurePiece * startPiece, std::list<StructurePiece*> * pieces,
        Random * random) {
        if (orientation == Direction::NORTH || orientation == Direction::EAST) {
            generateSmallDoorChildLeft((StartPiece*)startPiece, pieces, random,
                                       1, 1);
        } else {
            generateSmallDoorChildRight((StartPiece*)startPiece, pieces, random,
                                        1, 1);
        }
    }

    StrongholdPieces::LeftTurn* StrongholdPieces::LeftTurn::createPiece(
        std::list<StructurePiece*> * pieces, Random * random, int footX,
        int footY, int footZ, int direction, int genDepth) {
        BoundingBox* box = BoundingBox::orientBox(
            footX, footY, footZ, -1, -1, 0, width, height, depth, direction);

        StartPiece* startPiece = NULL;
        if (pieces != NULL)
            startPiece = ((StrongholdPieces::StartPiece*)pieces->front());

        if (!isOkBox(box, startPiece) ||
            StructurePiece::findCollisionPiece(pieces, box) != NULL) {
            delete box;
            return NULL;
        }

        return new LeftTurn(genDepth, random, box, direction);
    }

bool StrongholdPieces::LeftTurn::postProcess(Level* level, Random* random,
                                             Boundi// bounding walls
    if (edgesLiquid(level, chunkBB)) {
        return false;
    }

    
    generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1// entry door                CHECK_AIR, random, (BlockSelector*)smoothStoneSelector);
    
    generateSmallDoor(level, ran// exit openingntryDoor, 1,
                      height - SMALL_DOOR_HEIGHT - 1, 0);
    
    if (orientation == Direction::NORTH || orientation == Direction::EAST) {
        generateBox(level, chunkBB, 0, 1, 1, 0, 3, 3, 0, 0, false);
    } else {
        generateBox(level, chunkBB, 4, 1, 1, 4, 3, 3// for reflection   }

    return true;
}

StrongholdPieces::RightTurn::RightTurn() {
    
}

StrongholdPieces::RightTurn::RightTurn(int genDepth, Random* random,
                                       BoundingBox* stairsBox, int direction)
    : LeftTurn(genDepth, random, stairsBox, direction) {}

void StrongholdPieces::RightTurn::addChildren(StructurePiece* startPiece,
                                              std::list<StructurePiece*>* pieces,
                                              Random* random) {
        if (orientation == Direction::NORTH || orientation == Direction::EAST) {
            generateSmallDoorChildRight((StartPiece*)startPiece, pieces, random,
                                        1, 1);
        } else {
            generateSmallDoorChildLeft((StartPiece*)startPiece, pieces, random,
                                       1, 1);
        }
}

bool StrongholdPieces::RightTurn::postProcess(Level* level, Random* random,
                                              B// bounding wallsBB) {
    if (edgesLiquid(level, chunkBB)) {
        return false;
    }

    
    generateBox(level, chunkBB, 0, 0, 0, width - 1, heigh// entry door- 1,
                CHECK_AIR, random, (BlockSelector*)smoothStoneSelector);
    
    generateSmallDoor(level// exit openingBB, entryDoor, 1,
                      height - SMALL_DOOR_HEIGHT - 1, 0);
    
    if (orientation == Direction::NORTH || orientation == Direction::EAST) {
        generateBox(level, chunkBB, 4, 1, 1, 4, 3, 3, 0, 0, false);
    } else {
        generateBox(level, chunkBB, 0, 1, 1, 0, 3, 3,// for reflection  }

    return true;
}

StrongholdPieces::RoomCrossing::RoomCrossing() {
    
}

StrongholdPieces::RoomCrossing::RoomCrossing(int genDepth, Random* random,
                                             BoundingBox* stairsBox,
                                             int direction)
    : StrongholdPiece(genDepth), type(random->nextInt(5)) {
        entryDoor = randomSmallDoor(random);
        orientation = direction;
        boundingBox = stairsBox;
}

void StrongholdPieces::RoomCrossing::addAdditona"Type"ata(CompoundTag* tag) {
        StrongholdPiece::addAdditonalSaveData(tag);
        tag->putInt(L, type);
}

void StrongholdPieces::RoomCrossing::readAdditonalSaveDat"Type"oundTag* tag) {
        StrongholdPiece::readAdditonalSaveData(tag);
        type = tag->getInt(L);
}

void StrongholdPieces::RoomCrossing::addChildren(StructurePiece* startPiece,
                                                 std::list<StructurePiece*>* pieces,
                                                 Random* random) {
        generateSmallDoorChildForward((StartPiece*)startPiece, pieces, random,
                                      4, 1);
        generateSmallDoorChildLeft((StartPiece*)startPiece, pieces, random, 1,
                                   4);
        generateSmallDoorChildRight((StartPiece*)startPiece, pieces, random, 1,
                                    4);
}

StrongholdPieces::RoomCrossing* StrongholdPieces::RoomCrossing::createPiece(
    std::list<StructurePiece*>* pieces, Random* random, int footX, int footY,
    int footZ, int direction, int genDepth) {
        BoundingBox* box = BoundingBox::orientBox(
            footX, footY, footZ, -4, -1, 0, width, height, depth, direction);

        StartPiece* startPiece = NULL;
        if (pieces != NULL)
            startPiece = ((StrongholdPieces::StartPiece*)pieces->front());

        if (!isOkBox(box, startPiece) ||
            StructurePiece::findCollisionPiece(pieces, box) != NULL) {
            delete box;
            return NULL;
        }

        return new RoomCrossing(genDepth, random, box, direction);
}

WeighedTreasure* StrongholdPieces::RoomCrossing::smallTreasureItems
    [SMALL_TREASURE_ITEMS_COUNT] = {
        new WeighedTreasure(Item::ironIngot_Id, 0, 1, 5, 10),
        new WeighedTreasure(Item::goldIngot_Id, 0, 1, 3, 5),
        new WeighedTreasure(Item::redStone_Id, 0, 4, 9, 5),
        new WeighedTreasure(Item::coal_Id, CoalItem::STONE_COAL, 3, 8, 10),
        new WeighedTreasure(Item::bread_Id, 0, 1, 3, 15),
        new WeighedTreasure(Item::apple_Id, 0, 1, 3, 15),
        new WeighedTreasure(Item::pickAxe_iron_Id, 0, 1, 1, 1),
};

bool StrongholdPieces::RoomCrossing::postProcess(Level* level, Random* random,
                                        // bounding wallsBox* chunkBB) {
    if (edgesLiquid(level, chunkBB)) {
        return false;
    }

    
    generateBox(level, chunkBB, 0, 0, 0, width // entry door 1, depth - 1,
                CHECK_AIR, random, (BlockSelector*)smooth// exit openings    
    generateSmallDoor(level, random, chunkBB, entryDoor, 4, 1, 0);
    
    generateBox(level, chunkBB, 4, 1, depth - 1, 6, 3, depth - 1, 0, 0, false);
    generateBox(level, chunkBB, 0, 1, 4, 0, 3, 6, 0, 0, false);
    generateBox(level, chunkBB, width - 1, 1, 4, width - 1, // middle torch pillar   switch (type) {
        default:
            break;
        case 0:
            
            placeBlock(level, Tile::stoneBrick_Id, 0, 5, 1, 5, chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, 5, 2, 5, chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, 5, 3, 5, chunkBB);
            placeBlock(level, Tile::torch_Id, 0, 4, 3, 5, chunkBB);
            placeBlock(level, Tile::torch_Id, 0, 6, 3, 5, chunkBB);
            placeBlock(level, Tile::torch_Id, 0, 5, 3, 4, chunkBB);
            placeBlock(level, Tile::torch_Id, 0, 5, 3, 6, chunkBB);
            placeBlock(level, Tile::stoneSlabHalf_Id, 0, 4, 1, 4, chunkBB);
            placeBlock(level, Tile::stoneSlabHalf_Id, 0, 4, 1, 5, chunkBB);
            placeBlock(level, Tile::stoneSlabHalf_Id, 0, 4, 1, 6, chunkBB);
            placeBlock(level, Tile::stoneSlabHalf_Id, 0, 6, 1, 4, chunkBB);
            placeBlock(level, Tile::stoneSlabHalf_Id, 0, 6, 1, 5, chunkBB);
            placeBlock(level, Tile::stoneSlabHalf_Id, 0, 6, 1, 6, chunkBB);
            placeBlock(level, Tile::stoneSlabHalf_Id, 0, 5, 1, 4, chunkBB);
            placeBlock(level, Tile::stoneSlabHalf_Id, 0, 5, 1, 6, chunkBB);
            break;
        case 1: {
        for (int i = 0; i < 5; i++) {
            placeBlock(level, Tile::stoneBrick_Id, 0, 3, 1, 3 + i, chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, 7, 1, 3 + i, chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, 3 + i, 1, 3, chunkBB);
            placeBlock(level, Tile::stoneBrick_Id, 0, 3 + i, 1, 7, chunkBB);
        }
        placeBlock(level, Tile::stoneBrick_Id, 0, 5, 1, 5, chunkBB);
        placeBlock(level, Tile::stoneBrick_Id, 0, 5, 2, 5, chunkBB);
        placeBlock(level, Tile::stoneBrick_Id, 0, 5, 3, 5, chunkBB);
        placeBlock(level, Tile::water_Id, 0, 5, 4, 5, chunkBB);
        } break;
        case 2: {
        for (int z = 1; z <= 9; z++) {
            placeBlock(level, Tile::cobblestone_Id, 0, 1, 3, z, chunkBB);
            placeBlock(level, Tile::cobblestone_Id, 0, 9, 3, z, chunkBB);
        }
        for (int x = 1; x <= 9; x++) {
            placeBlock(level, Tile::cobblestone_Id, 0, x, 3, 1, chunkBB);
            placeBlock(level, Tile::cobblestone_Id, 0, x, 3, 9, chunkBB);
        }
        placeBlock(level, Tile::cobblestone_Id, 0, 5, 1, 4, chunkBB);
        placeBlock(level, Tile::cobblestone_Id, 0, 5, 1, 6, chunkBB);
        placeBlock(level, Tile::cobblestone_Id, 0, 5, 3, 4, chunkBB);
        placeBlock(level, Tile::cobblestone_Id, 0, 5, 3, 6, chunkBB);
        placeBlock(level, Tile::cobblestone_Id, 0, 4, 1, 5, chunkBB);
        placeBlock(level, Tile::cobblestone_Id, 0, 6, 1, 5, chunkBB);
        placeBlock(level, Tile::cobblestone_Id, 0, 4, 3, 5, chunkBB);
        placeBlock(level, Tile::cobblestone_Id, 0, 6, 3, 5, chunkBB);
        for (int y = 1; y <= 3; y++) {
            placeBlock(level, Tile::cobblestone_Id, 0, 4, y, 4, chunkBB);
            placeBlock(level, Tile::cobblestone_Id, 0, 6, y, 4, chunkBB);
            placeBlock(level, Tile::cobblestone_Id, 0, 4, y, 6, chunkBB);
            placeBlock(level, Tile::cobblestone_Id, 0, 6, y, 6, chunkBB);
        }
        placeBlock(level, Tile::torch_Id, 0, 5, 3, 5, chunkBB);
        for (int z = 2; z <= 8; z++) {
            placeBlock(level, Tile::wood_Id, 0, 2, 3, z, chunkBB);
            placeBlock(level, Tile::wood_Id, 0, 3, 3, z, chunkBB);
            if (z <= 3 || z >= 7) {
                placeBlock(level, Tile::wood_Id, 0, 4, 3, z, chunkBB);
                placeBlock(level, Tile::wood_Id, 0, 5, 3, z, chunkBB);
                placeBlock(level, Tile::wood_Id, 0, 6, 3, z, chunkBB);
            }
            placeBlock(level, Tile::wood_Id, 0, 7, 3, z, chunkBB);
            placeBlock(level, Tile::wood_Id, 0, 8, 3, z, chunkBB);
        }
        placeBlock(level, Tile::ladder_Id,
                   getOrientationData(Tile::ladder_Id, Facing::WEST), 9, 1, 3,
                   chunkBB);
        placeBlock(level, Tile::ladder_Id,
                   getOrientationData(Tile::ladder_Id, Facing::WEST), 9, 2, 3,
                   chunkBB);
        placeBlock(level, Tile::ladder_Id,
                   getOrientationData(Tile::ladder_Id, Facing::WEST), 9, 3, 3,
                   chunkBB);

            createChest(
                level, chunkBB, random, 3, 4, 8,
                WeighedTreasure::addToTreasure(
                    WeighedTreasureArray(smallTreasureItems,
                                         SMALL_TREASURE_ITEMS_COUNT),
                    Ite// System.out.println("Created chest at " + getWorldX(3, 8) +       1 + ra// "," + getWorldY(4) + "," + getWorldZ(3, 8));
                       // for reflection    } break;
    }
    return true;
}

StrongholdPieces::PrisonHall::PrisonHall() {
    
}

StrongholdPieces::PrisonHall::PrisonHall(int genDepth, Random* random,
                                         BoundingBox* stairsBox, int direction)
    : StrongholdPiece(genDepth) {
    entryDoor = randomSmallDoor(random);
    orientation = direction;
    boundingBox = stairsBox;
}

void StrongholdPieces::PrisonHall::addChildren(StructurePiece* startPiece,
                                               std::list<StructurePiece*>* pieces,
                                               Random* random) {
    generateSmallDoorChildForward((StartPiece*)startPiece, pieces, random, 1,
                                  1);
}

StrongholdPieces::PrisonHall* StrongholdPieces::PrisonHall::createPiece(
    std::list<StructurePiece*>* pieces, Random* random, int footX, int footY,
    int footZ, int direction, int genDepth) {
    BoundingBox* box = BoundingBox::orientBox(footX, footY, footZ, -1, -1, 0,
                                              width, height, depth, direction);

    StartPiece* startPiece = NULL;
    if (pieces != NULL)
        startPiece = ((StrongholdPieces::StartPiece*)pieces->front());

    if (!isOkBox(box, startPiece) ||
        StructurePiece::findCollisionPiece(pieces, box) != NULL) {
        delete box;
        return NULL;
    }

    return new PrisonHall(genDepth, random, box, direction);
}

bool StrongholdPieces::PrisonHall::postProcess(Level* level, Random* random,
                            // bounding walls  BoundingBox* chunkBB) {
    if (edgesLiquid(level, chunkBB)) {
    return false;
    }

    
    generateBox(level, chunkBB, 0, 0,// entry door, height - 1, depth - 1,
                CHECK_AIR, random, (BlockSelect// exit openingselector);
    
    generateSmallDoor(level, random, chunkBB, entryDoor, 1// door pillars
    generateBox(level, chunkBB, 1, 1, depth - 1, 3, 3, depth - 1, 0, 0, false);

    
    generateBox(level, chunkBB, 4, 1, 1, 4, 3, 1, false, random,
                (BlockSelector*)smoothStoneSelector);
    generateBox(level, chunkBB, 4, 1, 3, 4, 3, 3, false, random,
                (BlockSelector*)smoothStoneSelector);
    generateBox(level, chunkBB, 4, 1, 7, 4, 3, 7, false, random,
                (BlockSelector*)smoothStoneSelector);
    gener// gratesvel, chunkBB, 4, 1, 9, 4, 3, 9, false, random,
                (BlockSelector*)smoothStoneSelector);

    
    generateBox(level, chunkBB, 4, 1, 4, 4, 3, 6, Tile::ironFence_Id,
                Tile::ironFence_Id, false);
    // doorsBox(level, chunkBB, 5, 1, 5, 7, 3, 5, Tile::ironFence_Id,
                Tile::ironFence_Id, false);

    
    placeBlock(level, Tile::ironFence_Id, 0, 4, 3, 2, chunkBB);
    placeBlock(level, Tile::ironFence_Id, 0, 4, 3, 8, chunkBB);
    placeBlock(level, Tile::door_iron_Id,
               getOrientationData(Tile::door_iron_Id, 3), 4, 1, 2, chunkBB);
    placeBlock(level, Tile::door_iron_Id,
               getOrientationData(Tile::door_iron_Id, 3) + DoorTile::UPPER_BIT,
               4, 2, 2, chunkBB);
    placeBlock(level, Tile::door_iron_Id,
               getOrientationData(Tile::door_iron_Id, 3), 4, 1, 8, chunkBB);
    placeBlock(level, Tile::door_iron_Id,
               getOrientationData(Tile::door_iron_Id, 3) + DoorTile::UPPER_BIT,
   // for reflection 8, chunkBB);

    return true;
    }

    StrongholdPieces::Library::Library() { isTall = false; }

    StrongholdPieces::Library::Library(int genDepth, Random* random,
                                       BoundingBox* roomBox, int direction)
        : StrongholdPiece(genDepth), isTall(roomBox->getYSpan() > height) {
        entryDoor = randomSmallDoor(random);
        orientation = direction;
        boundingBox = roomBox;
    }

    void StrongholdPieces::Libr "Tall" ddAdditonalSaveData(CompoundTag* tag) {
        StrongholdPiece::addAdditonalSaveData(tag);
        tag->putBoolean(L, isTall);
    }

    void StrongholdPieces::Library::readAd "Tall" lSaveData(CompoundTag* tag) {
        StrongholdPiece::readAdditonalSaveData(tag);
        isTall = tag->getBoolean(L);
    }

    StrongholdPieces::Library* StrongholdPieces::Library::createPiece(
        std :  // attempt to make a tall library first random, int footX, int
               // footY,
        int footZ, int direction, int genDepth) {
        BoundingBox* box =
            BoundingBox::orientBox(footX, footY, footZ, -4, -1, 0, width,
                                   tallHeight, depth, direction);

        StartPiece* startPiece = NULL;
        if (pieces != NULL)
            startPiece = ((StrongholdPieces::StartPiece*)pieces->front());

        if (!isO  // make a short library|
                StructurePiece::findCollisionPiece(pieces, box) != NULL) {
            delete box;

            box = BoundingBox::orientBox(footX, footY, footZ, -4, -1, 0, width,
                                         height, depth, direction);

            if (!isOkBox(box, startPiece) ||
                StructurePiece::findCollisionPiece(pieces, box) != NULL) {
                delete box;
                return NULL;
            }
        }

        return new Library(genDepth, random, box, direction);
    }

    WeighedTreasure* StrongholdPieces::Library::libraryTreasureItems
        [LIBRARY_TREASURE_ITEMS_COUNT] = {
            new WeighedTreasure(Item::book_Id, 0, 1, 3, 20),
            new WeighedTreasure(Item::paper_Id, 0, 2, 7, 20),
            new WeighedTreasure(Item::map_Id, 0, 1, 1, 1),
            new WeighedTreasure(Item::compass_Id, 0, 1, 1, 1),
    };

    bool StrongholdPieces::Library::postProcess(Level* level, Random* random,
                                                BoundingBox* chunkBB) {
        if (edgesLiquid(level, chunkBB)) {
            // bounding walls  }

            int currentHeight = tallHeight;
            if (!isTall) {
                currentHeight = height;
            }

    generateBox(level, chunkBB, 0, 0, 0, width - 1, cur// entry door1,
                depth - 1, CHECK_AIR, random,
                (BlockSe// place sparse cob webstor);
    
    generateSmallDoor(level, random, chunkBB, entryDoor, 4, 1, 0);

    
    generateMaybeBox(level, chunkBB, random, .07f, 2, 1, 1, width - 1 - 2,
                     height - 2// place library walls_Id, Tile::web_Id, false);

    const int bookLeft = 1;
    const int bookRight = width - 2;

    
    for (int d = 1; d <= depth - 2; d++) {
            if (((d - 1) % 4) == 0) {
                generateBox(level, chunkBB, bookLeft, 1, d, bookLeft, 4, d,
                            Tile::wood_Id, Tile::wood_Id, false);
                generateBox(level, chunkBB, bookRight, 1, d, bookRight, 4, d,
                            Tile::wood_Id, Tile::wood_Id, false);

                placeBlock(level, Tile::torch_Id, 0, 2, 3, d, chunkBB);
                placeBlock(level, Tile::torch_Id, 0, width - 3, 3, d, chunkBB);

                if (isTall) {
                    generateBox(level, chunkBB, bookLeft, 6, d, bookLeft, 9, d,
                                Tile::wood_Id, Tile::wood_Id, false);
                    generateBox(level, chunkBB, bookRight, 6, d, bookRight, 9,
                                d, Tile::wood_Id, Tile::wood_Id, false);
                }
            } else {
                generateBox(level, chunkBB, bookLeft, 1, d, bookLeft, 4, d,
                            Tile::bookshelf_Id, Tile::bookshelf_Id, false);
                generateBox(level, chunkBB, bookRight, 1, d, bookRight, 4, d,
                            Tile::bookshelf_Id, Tile::bookshelf_Id, false);

                if (isTall) {
                    generateBox(level, chunkBB, bookLeft, 6, d, bookLeft, 9, d,
                                Tile::bookshelf_Id, Tile::bookshelf_Id, false);
                generateBox(level, chunkBB, bookRight, 6, d, bookRigh// place book shelves                Tile::bookshelf_Id, Tile::bookshelf_Id, false);
                }
            }
    }

    
    for (int d = 3; d < depth - 3; d += 2) {
            generateBox(level, chunkBB, 3, 1, d, 4, 3, d, Tile::bookshelf_Id,
                        Tile::bookshelf_Id, false);
            generateBox(level, chunkBB, 6, 1, d, 7, 3, d, Tile::bookshelf_Id,
                        Tile::bookshelf_Id, false);
            generateBox(level, chunkBB,  // create balcony, Tile::bookshelf_Id,
                        Tile::bookshelf_Id, false);
    }

    if (isTall) {
            generateBox(level, chunkBB, 1, 5, 1, 3, 5, depth - 2, Tile::wood_Id,
                        Tile::wood_Id, false);
            generateBox(level, chunkBB, width - 4, 5, 1, width - 2, 5,
                        depth - 2, Tile::wood_Id, Tile::wood_Id, false);
            generateBox(level, chunkBB, 4, 5, 1, width - 5, 5, 2, Tile::wood_Id,
                        Tile::wood_Id, false);
            generateBox(level, chunkBB, 4, 5, depth - 3, width - 5, 5,
                        depth - 2, Tile::wood_Id, Tile::wood_Id, false);

            placeBlock(level, Tile::wood_Id, 0, width - 5, 5, depth - 4,
                       chunkBB);
        placeBlock(level, Tile::wood_Id, 0, wid// balcony fences- 4, chunkBB);
        placeBlock(level, Tile::wood_Id, 0, width - 5, 5, depth - 5, chunkBB);

        
        generateBox(level, chunkBB, 3, 6, 2, 3, 6, depth - 3, Tile::fence_Id,
                    Tile::fence_Id, false);
        generateBox(level, chunkBB, width - 4, 6, 2, width - 4, 6, depth - 5,
                    Tile::fence_Id, Tile::fence_Id, false);
        generateBox(level, chunkBB, 4, 6, 2, width - 5, 6, 2, Tile::fence_Id,
                    Tile::fence_Id, false);
        generateBox(level, chunkBB, 4, 6, depth - 3, 8, 6, depth - 3,
                    Tile::fence_Id, Tile::fence_Id, false);
        placeBlock(level, Tile::fence_Id, 0, width - 5, 6, depth - 4, chunkBB);
        placeBlock(level, Tile::fence_Id, 0, widt// ladder depth - 4, chunkBB);
        placeBlock(level, Tile::fence_Id, 0, width - 5, 6, depth - 5, chunkBB);

        
        int orientationData = getOrientationData(Tile::ladder_Id, 3);
        placeBlock(level, Tile::ladder_Id, orientationData, width - 4, 1,
                   depth - 2, chunkBB);
        placeBlock(level, Tile::ladder_Id, orientationData, width - 4, 2,
                   depth - 2, chunkBB);
        placeBlock(level, Tile::ladder_Id, orientationData, width - 4, 3,
                   depth - 2, chunkBB);
        placeBlock(level, Tile::ladder_Id, orientationData, width - 4, 4,
                   depth - 2, chunkBB);
        placeBlock(level, Tile::ladder_Id, orientationData, width - 4, 5,
                   depth - 2, chunkBB);
        placeBlock(level, Tile::ladder_Id, orientationData, width - 4, 6,
                   depth - 2, chunkBB);
   // chandelierck(level, Tile::ladder_Id, orientationData, width - 4, 7,
                   depth - 2, chunkBB);

        
        int x = width / 2;
        int z = depth / 2;
        placeBlock(level, Tile::fence_Id, 0, x - 1, tallHeight - 2, z, chunkBB);
        placeBlock(level, Tile::fence_Id, 0, x, tallHeight - 2, z, chunkBB);
        placeBlock(level, Tile::fence_Id, 0, x - 1, tallHeight - 3, z, chunkBB);
        placeBlock(level, Tile::fence_Id, 0, x, tallHeight - 3, z, chunkBB);
        placeBlock(level, Tile::fence_Id, 0, x - 1, tallHeight - 4, z, chunkBB);
        placeBlock(level, Tile::fence_Id, 0, x, tallHeight - 4, z, chunkBB);

        placeBlock(level, Tile::fence_Id, 0, x - 2, tallHeight - 4, z, chunkBB);
        placeBlock(level, Tile::fence_Id, 0, x + 1, tallHeight - 4, z, chunkBB);
        placeBlock(level, Tile::fence_Id, 0, x - 1, tallHeight - 4, z - 1,
                   chunkBB);
        placeBlock(level, Tile::fence_Id, 0, x - 1, tallHeight - 4, z + 1,
                   chunkBB);
        placeBlock(level, Tile::fence_Id, 0, x, tallHeight - 4, z - 1, chunkBB);
        placeBlock(level, Tile::fence_Id, 0, x, tallHeight - 4, z + 1, chunkBB);

        placeBlock(level, Tile::torch_Id, 0, x - 2, tallHeight - 3, z, chunkBB);
        placeBlock(level, Tile::torch_Id, 0, x + 1, tallHeight - 3, z, chunkBB);
        placeBlock(level, Tile::torch_Id, 0, x - 1, tallHeight - 3, z - 1,
                   chunkBB);
        placeBlock(level, Tile::torch_Id, 0, x - 1, tallHeight - 3, z + 1,
                   chunkBB);
        placeBlock(level, Tile::torch_Id, 0, x, tallH// place chests1, chunkBB);
        placeBlock(level, Tile::torch_Id, 0, x, tallHeight - 3, z + 1, chunkBB);
    }

    
    createChest(
        level, chunkBB, random, 3, 3, 5,
        WeighedTreasure::addToTreasure(
            WeighedTreasureArray(libraryTreasureItems,
                                 LIBRARY_TREASURE_ITEMS_COUNT),
            Item::enchantedBook->createForRandomTreasure(random, 1, 5, 2)),
        1 + random->nextInt(4));
    if (isTall) {
            placeBlock(level, 0, 0, width - 2, tallHeight - 2, 1, chunkBB);
            createChest(level, chunkBB, random, width - 2, tallHeight - 3, 1,
                        WeighedTreasure::addToTreasure(
                            WeighedTreasureArray(libraryTreasureItems,
                                                 LIBRARY_TREASURE_ITEMS_COUNT),
                            Item::enchantedBook->createForRandomTreasure(
                                random, 1, 5, 2)),
                        1 + random->nextInt(4));
    }

    retu// for reflectiongholdPieces::FiveCrossing::FiveCrossing() {
    leftLow = leftHigh = rightLow = rightHigh = false;
        }

        StrongholdPieces::FiveCrossing::FiveCrossing(
            int genDepth, Random* random, BoundingBox* stairsBox, int direction)
            : StrongholdPiece(genDepth) {
            entryDoor = randomSmallDoor(random);
            orientation = direction;
            boundingBox = stairsBox;

            leftLow = random->nextBoolean();
            leftHigh = random->nextBoolean();
            rightLow = random->nextBoolean();
            rightHigh = random->nextInt(3) > 0;
        }

        void StrongholdPieces::Five "leftLow"
            : addAdditonalSaveData(CompoundTag
                                   "leftHigh" StrongholdPiece::addAdditonalSa
                                   "rightLow");
    tag->putBoolean(L,"rightHigh"    tag->putBoolean(L, leftHigh);
    tag->putBoolean(L, rightLow);
    tag->putBoolean(L, rightHigh);
    }

    void StrongholdPieces::FiveCrossing::re
        "leftLow" alSaveData(CompoundTag* tag) {
        "leftHigh" Piece::readAdditonalSaveData(tag);
        "rightLow" w = tag->getBoolean(L);
        "rightHigh" tag->getBoolean(L);
        rightLow = tag->getBoolean(L);
        rightHigh = tag->getBoolean(L);
    }

void StrongholdPieces::FiveCrossing::addChildren(StructurePiece* startPiece,
                                                 std::list<StructureP// compensate for weird negative-facing behaviour                Random* random) {
    int zOffA = 3;
    int zOffB = 5;
    
    if (orientation == Direction::WEST || orientation == Direction::NORTH) {
    zOffA = depth - 3 - zOffA;
    zOffB = depth - 3 - zOffB;
    }

    generateSmallDoorChildForward((StartPiece*)startPiece, pieces, random, 5,
                                  1);
    if (leftLow)
        generateSmallDoorChildLeft((StartPiece*)startPiece, pieces, random,
                                   zOffA, 1);
    if (leftHigh)
        generateSmallDoorChildLeft((StartPiece*)startPiece, pieces, random,
                                   zOffB, 7);
    if (rightLow)
        generateSmallDoorChildRight((StartPiece*)startPiece, pieces, random,
                                    zOffA, 1);
    if (rightHigh)
        generateSmallDoorChildRight((StartPiece*)startPiece, pieces, random,
                                    zOffB, 7);
}

StrongholdPieces::FiveCrossing* StrongholdPieces::FiveCrossing::createPiece(
    std::list<StructurePiece*>* pieces, Random* random, int footX, int footY,
    int footZ, int direction, int genDepth) {
    BoundingBox* box = BoundingBox::orientBox(footX, footY, footZ, -4, -3, 0,
                                              width, height, depth, direction);

    StartPiece* startPiece = NULL;
    if (pieces != NULL)
        startPiece = ((StrongholdPieces::StartPiece*)pieces->front());

    if (!isOkBox(box, startPiece) ||
        StructurePiece::findCollisionPiece(pieces, box) != NULL) {
        delete box;
        return NULL;
    }

    return new FiveCrossing(genDepth, random, box, direction);
}

bool StrongholdPieces::FiveCrossing::postProcess(
    Level* level, Random* random,
    // bounding walls                 BoundingBox* chunkBB) {
    if (edgesLiquid(level, chunkBB)) { return false; }

    generateBox(
        level,  // entry door0, 0, width - 1, height - 1, depth - 1,
        CHECK_AIR,
        random  // exit openings*)smoothStoneSelector);

            generateSmallDoor(level, random, chunkBB, entryDoor, 4, 3, 0);

        if (leftLow) generateBox(level, chunkBB, 0, 3, 1, 0, 5, 3, 0, 0, false);
        if (rightLow)
            generateBox(level, chunkBB, 9, 3, 1, 9, 5, 3, 0, 0, false);
        if (leftHigh)
            generateBox(level, chunkBB, 0, 5, 7, 0, 7, 9, 0, 0, false);
        if (rightHigh)
            g  // main floorvel, chunkBB, 9, 5, 7, 9, 7, 9, 0, 0, false);
                generateBox(level, chunkBB, 5, 1, 10, 7, 3, 10, 0, 0, false);

        // side wallsnerateBox(level, chunkBB, 1, 2, 1, 8, 2, 6, false, random,
        (BlockSelector*)smoothStoneSelector);

    generateBox(level, chunkBB, 4, 1, 5, 4, 4, 9, false, random,
                (BlockSelector*)
                    smoothStoneSele  // upper floorerateBox(level, chunkBB, 8,
                                     // 1, 5, 8, 4, 9, false, random,
                (BlockSelector*) smoothStoneSelector);
    // left stairsrateBox(level, chunkBB, 1, 4, 7, 3, 4, 9, false, random,
    (BlockSelector*)smoothStoneSelector);

generateBox(level, chunkBB, 1, 3, 5, 3, 3, 6, false, random,
            (BlockSelector*)smoothStoneSelector);
generateBox(level, chunkBB, 1, 3, 4, 3, 3, 4, Tile::stoneSlabHalf_Id,
            Tile::stoneSlabHalf_Id,
            false  // lower stairsBox(level, chunkBB, 1, 4, 6, 3, 4, 6,
                   // Tile::stoneSlabHalf_Id,
            Tile::stoneSlabHalf_Id,
            false);

generateBox(level, chunkBB, 5, 1, 7, 7, 1, 8, false, random,
            (BlockSelector*)smoothStoneSelector);
generateBox(level, chunkBB, 5, 1, 9, 7, 1, 9, Tile::stoneSlabHalf_Id,
            Tile::stoneSlabHalf_Id,
            false  // bridgenerateBox(level, chunkBB, 5, 2, 7, 7, 2, 7,
                   // Tile::stoneSlabHalf_Id,
            Tile::stoneSlabHalf_Id,
            false);

generateBox(level, chunkBB, 4, 5, 7, 4, 5, 9, Tile::stoneSlabHalf_Id,
            Tile::stoneSlabHalf_Id, false);
generateBox(level, chunkBB, 8, 5, 7, 8, 5, 9, Tile::stoneSlabHalf_Id,
            Tile::stoneSlabHalf_Id, false);
generateBox(level, chunkBB, 5, 5, 7, 7, 5, 9, Tile::stoneSlab_Id,
            Tile::stoneSlab_Id, false);
// for reflectionvel, Tile::torch_Id, 0, 6, 5, 6, chunkBB);

return true;
}

StrongholdPieces::PortalRoom::PortalRoom() {}

StrongholdPieces::PortalRoom::PortalRoom(int genDepth, Random* random,
                                         BoundingBox* box, int direction)
    : StrongholdPiece(genDepth) {
    hasPlacedMobSpawner = false;
    orientation = direction;
    boundingBox = box;
}

void Stronghold "Mob"s ::PortalRoom::addAdditonalSaveData(CompoundTag* tag) {
    StrongholdPiece::addAdditonalSaveData(tag);
    tag->putBoolean(L, hasPlacedMobSpawner);
}

void StrongholdPieces::PortalRoom::read "Mob" onalSaveData(CompoundTag* tag) {
    StrongholdPiece::readAdditonalSaveData(tag);
    hasPlacedMobSpawner = tag->getBoolean(L);
}

void StrongholdPieces::PortalRoom::addChildren(
    StructurePiece* startPiece, std::list<StructurePiece*>* pieces,
    Random* random) {
    if (startPiece != NULL) {
        ((StartPiece*)startPiece)->portalRoomPiece = this;
    }
}

StrongholdPieces::PortalRoom* StrongholdPieces::PortalRoom::createPiece(
    std::list<StructurePiece*>* pieces, Random* random, int footX, int footY,
    int footZ, int direction, int genDepth) {
    BoundingBox* box =  // 4J Added so that we can check that Portals stay
                        // within the bounds of the     // world (which they
                        // ALWAYS should anyway) direction);

        StartPiece* startPiece = NULL;
    if (pieces != NULL)
        startPiece = ((StrongholdPieces::StartPiece*)pieces->front());

    if (!isOkBox(box, startPiece) ||
        StructurePiece::findCollisionPiece(pieces, box) != NULL) {
        delete box;
        return NULL;
    }

    return new PortalRoom(genDepth, random, box, direction);
}

bool Strongh// bounding wallsRoom::postProcess(Level* level, Random* random,
                                               BoundingBox* chunkBB) {
    gene  // entry door, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1,
          // inner roof rowlockSelector*)smoothStoneSelector);

        generateSmallDoor(level, random, chunkBB, GRATES, 4, 1, 0);

    int y = height - 2;
    generateBox(level, chunkBB, 1, y, 1, 1, y, depth - 2, false, random,
                (BlockSelector*)smoothStoneSelector);
    generateBox(level, chunkBB, width - 2, y, 1, width - 2, y, depth - 2, false,
                random, (BlockSelector*)smoothStoneSelector);
    generateBox(level, chunkBB, 2, y, 1, width - 3, y, 2, false, random,
                (BlockSelector*)smoothStoneSelector);
    gene// entrance lava pools, 2, y, depth - 2, width - 3, y, depth - 2, false,
                random, (BlockSelector*)smoothStoneSelector);

    generateBox(level, chunkBB, 1, 1, 1, 2, 1, 4, false, random,
                (BlockSelector*)smoothStoneSelector);
    generateBox(level, chunkBB, width - 3, 1, 1, width - 2, 1, 4, false, random,
                (BlockSelector*)smoothStoneSelector);
    generateBox(level, chunkBB, 1, 1, 1, 1, 1, 3, Tile::lava_Id, Tile::lava_Id,
                // portal lava poolgenerateBox(level, chunkBB, width - 2, 1, 1,
                // width - 2, 1, 3, Tile::lava_Id,
                Tile::lava_Id, false);

    generateBox(level, chunkBB, 3, 1, 8, 7, 1, 12, false, random,
                (BlockSe// wall decorationsSelector);
    generateBox(level, chunkBB, 4, 1, 9, 6, 1, 11, Tile::lava_Id, Tile::lava_Id,
                false);

    
    for (int z = 3; z < depth - 2; z += 2) {
        generateBox(level, chunkBB, 0, 3, z, 0, 4, z, Tile::ironFence_Id,
                    Tile::ironFence_Id, false);
        generateBox(level, chunkBB, width - 1, 3, z, width - 1, 4, z,
                    Tile::ironFence_Id, Tile::ironFence_Id, false);
    }
    for (int x = 2; x < width - 2; x += 2) {
        g// stairox(level, chunkBB, x, 3, depth - 1, x, 4, depth - 1,
                    Tile::ironFence_Id, Tile::ironFence_Id, false);
    }

    
    int orientationData = getOrientationData(Tile::stairs_stoneBrick_Id, 3);
    generateBox(level, chunkBB, 4, 1, 5, 6, 1, 7, false, random,
                (BlockSelector*)smoothStoneSelector);
    generateBox(level, chunkBB, 4, 2, 6, 6, 2, 7, false, random,
                (BlockSelector*)smoothStoneSelector);
    generateBox(level, chunkBB, 4, 3, 7, 6, 3, 7, false, random,
                (BlockSelector*)smoothStoneSelector);
    for (int x = 4; x <= 6; x++) {
        placeBlock(level, Tile::stairs_stoneBrick_Id, orientationData, x, 1, 4,
                   chunkBB);
        placeBlock(level, Tile::stairs_stoneBrick_Id, orientationData, x, 2, 5,
                   chunkBB);
        placeBlock(level, Tile::stairs_stoneBrick_Id, orientationData, x, 3, 6,
                   chunkBB);
    }

    int north = Direction::NORTH;
    int south = Direction::SOUTH;
    int east = Direction::EAST;
    int west = Direction::WEST;

    switch (orientation) {
        case Direction::SOUTH:
            north = Direction::SOUTH;
            south = Direction::NORTH;
            break;
        case Direction::EAST:
            north = Direction::EAST;
            south = Direction::WEST;
            east = Direction::SOUTH;
            west = Direction::NORTH;
            break;
        case Direction::WEST:
            north = Direction::WEST;
            // 4J-PB - Removed for Christmas update since we don't have The
            // EndOUTH;
            // 4J-PB - not going to remove it, so that maps generated will have
            // it in,// but it can't be
            // activated

            placeBlock(level, Tile::endPortalFrameTile_Id,
                       north + ((random->nextFloat() > 0.9f)
                                    ? TheEndPortalFrameTile::EYE_BIT
                                    : 0),
                       4, 3, 8, chunkBB);
            placeBlock(level, Tile::endPortalFrameTile_Id,
                       north + ((random->nextFloat() > 0.9f)
                                    ? TheEndPortalFrameTile::EYE_BIT
                                    : 0),
                       5, 3, 8, chunkBB);
            placeBlock(level, Tile::endPortalFrameTile_Id,
                       north + ((random->nextFloat() > 0.9f)
                                    ? TheEndPortalFrameTile::EYE_BIT
                                    : 0),
                       6, 3, 8, chunkBB);
            placeBlock(level, Tile::endPortalFrameTile_Id,
                       south + ((random->nextFloat() > 0.9f)
                                    ? TheEndPortalFrameTile::EYE_BIT
                                    : 0),
                       4, 3, 12, chunkBB);
            placeBlock(level, Tile::endPortalFrameTile_Id,
                       south + ((random->nextFloat() > 0.9f)
                                    ? TheEndPortalFrameTile::EYE_BIT
                                    : 0),
                       5, 3, 12, chunkBB);
            placeBlock(level, Tile::endPortalFrameTile_Id,
                       south + ((random->nextFloat() > 0.9f)
                                    ? TheEndPortalFrameTile::EYE_BIT
                                    : 0),
                       6, 3, 12, chunkBB);
            placeBlock(level, Tile::endPortalFrameTile_Id,
                       east + ((random->nextFloat() > 0.9f)
                                   ? TheEndPortalFrameTile::EYE_BIT
                                   : 0),
                       3, 3, 9, chunkBB);
            placeBlock(level, Tile::endPortalFrameTile_Id,
                       east + ((random->nextFloat() > 0.9f)
                                   ? TheEndPortalFrameTile::EYE_BIT
                                   : 0),
                       3, 3, 10, chunkBB);
            placeBlock(level, Tile::endPortalFrameTile_Id,
                       east + ((random->nextFloat() > 0.9f)
                                   ? TheEndPortalFrameTile::EYE_BIT
                                   : 0),
                       3, 3, 11, chunkBB);
            placeBlock(level, Tile::endPortalFrameTile_Id,
                       west + ((random->nextFloat() > 0.9f)
                                   ? TheEndPortalFrameTile::EYE_BIT
                                   : 0),
                       7, 3, 9, chunkBB);
            placeBlock(level, Tile::endPortalFrameTile_Id,
                       west + ((random->nextFloat() > 0.9f)
                                   ? TheEndPortalFrameTile::EYE_BIT
                                   : 0),
                       7, 3, 10, chunkBB);
            placeBlock(level, Tile::endPortalFrameTile_Id,
                       west + ((random->nextFloat() > 0.9f)
                                   ? TheEndPortalFrameTile::EYE_BIT
                                   : 0),
                       7, 3, 11, chunkBB);

            if (!hasPlacedMobSpawn  // 4J Stu - The mob spawner location is
                                    // close enough for the map z = getWorld//
                                    // icon display, and this ensures that we
                                    // only need to set the //
                                    // position
                                    // once

                     app.AddTerrainFeaturePosition(
                         eTerrainFeature_StrongholdEndPortal, x, z);
                level->getLevelData()->setXStrongholdEndPortal(x);
                level->getLevelData()->setZStrongholdEndPortal(z);
                level->getLevelData()->setHasStrongholdEndPortal();

                hasPlacedMobSpawner = true; level->setTileAndData(
                    x, y, z, Tile::mobSpawner_Id, 0, Tile::UPDATE_CLIENTS);
                std::shared_ptr<MobSpawnerTileEntity> entity =
                    dynamic_pointer_cast <
                    MobSpawnerTileEnti
                    "Silverfish" level->getTileEntity(x, y, z))
                ;
            if (entity != NULL) entity->getSpawner()->setEntityId(L);
        }
}

return true;
}

void StrongholdPieces::SmoothStoneSelector::next(Random* random, int worldX,
                                                 int worldY, int worldZ,
                                                 bool isEdge) {
    if (isEdge) {
        nextId = Tile::stoneBrick_Id;

        float selection = random->nextFloat();
        if (selection < 0.2f) {
            nextData = SmoothStoneBrickTile::TYPE_CRACKED;
        } else if (selection < 0.5f) {
            nextData = SmoothStoneBrickTile::TYPE_MOSSY;
        } else if (selection < 0.55f) {
            nextId = Tile::monsterStoneEgg_Id;
            nextData = StoneMonsterTile::HOST_STONEBRICK;
        } else {
            nextData = 0;
        }
    } else {
        nextId = 0;
        nextData = 0;
    }
}

const StrongholdPieces::SmoothStoneSelector*
    StrongholdPieces::smoothStoneSelector = new SmoothStoneSelector();
