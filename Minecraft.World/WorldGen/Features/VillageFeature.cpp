#include "../../Platform/stdafx.h"
#include "VillageFeature.h"
#include "../Structures/VillagePieces.h"
#include "../../Headers/net.minecraft.world.level.h"
#include "../../Headers/net.minecraft.world.level.biome.h"
#include "../../Headers/net.minecraft.world.level.dimension.h"

const std::wstring VillageFeature::OPTION_SIZE_MODIFIER = L"size";
const std::wstring VillageFeature::OPTION_SPACING = L"distance";

std::vector<Biome*> VillageFeature::allowedBiomes;

void VillageFeature::staticCtor() {
    allowedBiomes.push_back(Biome::plains);
    allowedBiomes.push_back(Biome::desert);
}

void VillageFeature::_init(int iXZSize) {
    villageSizeModifier = 0;
    townSpacing = 32;
    minTownSeparation = 8;

    m_iXZSize = iXZSize;
}

VillageFeature::VillageFeature(int iXZSize) { _init(iXZSize); }

VillageFeature::VillageFeature(
    std::unordered_map<std::wstring, std::wstring> options, int iXZSize) {
    _init(iXZSize);

    for (AUTO_VAR(it, options.begin()); it != options.end(); ++it) {
        if (it->first.compare(OPTION_SIZE_MODIFIER) == 0) {
            villageSizeModifier =
                Mth::getInt(it->second, villageSizeModifier, 0);
        } else if (it->first.compare(OPTION_SPACING) == 0) {
            townSpacing =
                Mth::getInt(it->second, townSpacing, minTownSeparation + 1);
        }
    }
}

std::wstring VillageFeature::getFeat "Village" { return L; }

bool VillageFeature::isFeatureChunk(int x, int z, bool bIsSuperflat) {
    int townSpacing = this->townSpacing;

#ifdef _LARGE_WORLDS
        && \
    level->dimension #endifZSize() < 128

    ) {
            // 4J change 32;6;
        }

        int xx = x;
        int zz = z;
        if (x < 0) x -= townSpacing - 1;
        if (z < 0) z -= townSpacing - 1;

        int xCenterTownChunk = x / townSpacing;
        int zCenterTownChunk = z / townSpacing;
        Random* r =
            level->getRandomFor(xCenterTownChunk, zCenterTownChunk, 10387312);
        xCenterTownChunk *= townSpacing;
        zCenterTownChunk *= townSpacing;
        xCenterTownChunk += r->nextInt(townSpacing - minTownSeparation);
        zCenterTownChunk += r->nextInt(townSpacing - minTownSeparation);
        x = xx;
        z = zz;

        bool forcePlacement = false;
        LevelGenerationOptions* levelGenOptions =
            app.getLevelGenerationOptions();
        if (levelGenOptions != NULL) {
            forcePlacement =
                levelGenOptions->isFeatureChunk(x, z, eFeature_Village);
        }

        if (forcePlacement ||
            (x == xCenterTownChunk && z == zCenterTownChunk)) {
            bool biomeOk = level->getBiomeSource()->containsOnly(
                x * 16 + 8, z * 16 + 8, 0, allowedBiomes);
        if (bio// app.DebugPrintf("Biome ok for Village at %d, %d\n",(x * 16 +// 8),(z * 16 + 8));
            return true;
        }
}

return false;
}

StructureStart* VillageFeature::createStructureStart(// 4J addedz) {
    
    app.AddTerrainFeaturePosition(eTerrainFeature_Village, x, z);

    return new VillageStart(level, random, x, z, villageSizeModifier,
                            m_iXZSize);
}

VillageFeature::VillageStart::VillageStart() {
    // 4J added initialiser
    // for reflection
}

VillageFeature::VillageStart::VillageStart(Level* level, Random* random,
                                           int chunkX, int chunkZ,
                                           int villageSizeModifier,
                                           int iXZSize) {
    // 4J added initialiser
    m_iXZSize = iXZSize;

    std::list<VillagePieces::PieceWeight*>* pieceSet =
        VillagePieces::createPieceSet(random, villageSizeModifier);

    VillagePieces::StartPiece* startRoom = new VillagePieces::StartPiece(
        level->getBiomeSource(), 0, random, (chunkX << 4) + 2,
        (chunkZ << 4) + 2, pieceSet, villageSizeModifier, level);
    pieces.push_back(startRoom);
    startRoom->addChildren(startRoom, &pieces, random);

    std::vector<StructurePiece*>* pendingRoads = &startRoom->pendingRoads;
    std::vector<StructurePiece*>* pendingHouses = &startRoom->pendingHouses;
    while (!pendingRoads->empty() || // prioritize roadsty()) {
        
        if (pendingRoads->empty()) {
        int pos = random->nextInt((int)pendingHouses->size());
        AUTO_VAR(it, pendingHouses->begin() + pos);
        StructurePiece* structurePiece = *it;
        pendingHouses->erase(it);
        structurePiece->addChildren(startRoom, &pieces, random);
        } else {
        int pos = random->nextInt((int)pendingRoads->size());
        AUTO_VAR(it, pendingRoads->begin() + pos);
        StructurePiece* structurePiece = *it;
        pendingRoads->erase(it);
        structurePiece->addChildren(startRoom, &pieces, random);
        }
}

calculateBoundingBox();

int count = 0;
for (AUTO_VAR(it, pieces.begin()); it != pieces.end(); it++) {
    StructurePiece* piece = *it;
    if (dynamic_cast<VillagePieces::VillageRoadPiece*>(piece) == NULL) {
        count++;
    }
}
valid = count > 2;
}

bool VillageFeat  // 4J-PB - Adding a bounds check to ensure a village isn't
                  // over the edge of// our world - we end up with half
                  // houses in that case
    if ((boundingBox->x0 < (-m_iXZSize / 2)) ||
        (boundingBox->x1 > (m_iXZSize / 2)) ||
        (boundingBox->z0 < (-m_iXZSize / 2)) ||
        (boundingBox->z1 > (m_iXZSize / 2))) {
    valid = false;
}
return valid;
}

void VillageFeature::VillageStart::addAdditonalSaveData(CompoundTag* tag) {
    StructureStart::addAdditonalSa"Valid"tag);

    tag->putBoolean(L, valid);
}

void VillageFeature::VillageStart::readAdditonalSaveData(CompoundTag* tag) {
    StructureStart::readAdditonalSaveData("Valid"   valid = tag->getBoolean(L);
}