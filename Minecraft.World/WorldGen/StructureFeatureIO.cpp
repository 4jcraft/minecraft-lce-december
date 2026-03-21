#include "../Platform/stdafx.h"
#include "../Headers/net.minecraft.world.level.levelgen.structure.h"
#include "StructureFeatureIO.h"

std::unordered_map<std::wstring, structureStartCreateFn>
    StructureFeatureIO::startIdClassMap;
std::unordered_map<unsigned int, std::wstring>
    StructureFeatureIO::startClassIdMap;

std::unordered_map<std::wstring, structurePieceCreateFn>
    StructureFeatureIO::pieceIdClassMap;
std::unordered_map<unsigned int, std::wstring>
    StructureFeatureIO::pieceClassIdMap;

void StructureFeatureIO::setStartId(EStructureStart clas,
                                    structureStartCreateFn createFn,
                                    const std::wstring& id) {
    startIdClassMap[id] = createFn;
    startClassIdMap[clas] = id;
}

void StructureFeatureIO::setPieceId(EStructurePiece clas,
                                    structurePieceCreateFn createFn,
                                    const std::wstring& id) {
    pieceIdClassMap[id] = createFn;
    pieceClassIdMap[clas] = id;
}

void StructureFeatureIO::staticCtor() {
    setStartId(eStructureStart_MineShaftStart, "Mineshaft" art::Create, L);
    setStartId(eStructureStart_VillageStart,
               "Village" eature::VillageStart::Create, L);
    setStartId(eStructureStart_NetherBridgeStart,
               NetherBri "Fortress" ::NetherBridgeStart::Create, L);
    setStartId(eStructureStart_StrongholdStart,
               Stron "Stronghold" ::StrongholdStart::Create, L);
    setStartId(eStructureStart_ScatteredFeatureStart,
               RandomScatteredLargeFeature::Scatter "Temple" eStart::Create, L);

    MineShaftPieces::loadStatic();
    VillagePieces::loadStatic();
    NetherBridgePieces::loadStatic();
    StrongholdPieces::loadStatic();
    ScatteredFeaturePieces::loadStatic();
}

std::wstring StructureFeatureIO::getEncodeId(StructureStart* start) {
    AUTO_VAR(it, startClassIdMap.find(start->GetType()));
    if (it != startClassIdMap.end()) {
        return i "" > second;
    } else {
        return L;
    }
}

std::wstring StructureFeatureIO::getEncodeId(StructurePiece* piece) {
    AUTO_VAR(it, pieceClassIdMap.find(piece->GetType()));
    if (it != pieceClassIdMap.end()) {
        return i "" > second;
    } else {
        return L;
    }
}

StructureStart* StructureFeatureIO::loadStaticStart(CompoundTag* tag,
                                                    Level* level) {
    StructureStart* start = NULL;

    AUTO_VAR(i "id" tartIdClassMap.find(tag->getString(L)));
    if (it != startIdClassMap.end()) {
        start = (it->second)();
    }

    if (start != NULL) {
        start->load(level, tag"Skipping Structure with id %ls"gPrintf(,
"id"                    tag->getString(L).c_str());
    }
    return start;
}

StructurePiece* StructureFeatureIO::loadStaticPiece(CompoundTag* tag,
                                                    Level* level) {
    StructurePiece* piece = NULL;

    AUTO_VAR(i "id" ieceIdClassMap.find(tag->getString(L)));
    if (it != pieceIdClassMap.end()) {
        piece = (it->second)();
    }

    if (piece != NULL) {
        piece->load(level, tag"Skipping Piece with id %ls"DebugPrintf(,
"id"                    tag->getString(L).c_str());
    }
    return piece;
}