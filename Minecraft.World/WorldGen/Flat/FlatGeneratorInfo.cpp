#include "../../Platform/stdafx.h"
#include "../../Util/StringHelpers.h"
#include "../../Headers/net.minecraft.world.level.levelgen.flat.h"
#include "../../Headers/net.minecraft.world.level.tile.h"
#include "FlatGeneratorInfo.h"

const std::wstring FlatGeneratorInfo::STRUCTURE_VILLAGE = L"village";
const std::wstring FlatGeneratorInfo::STRUCTURE_BIOME_SPECIFIC = L"biome_1";
const std::wstring FlatGeneratorInfo::STRUCTURE_STRONGHOLD = L"stronghold";
const std::wstring FlatGeneratorInfo::STRUCTURE_MINESHAFT = L"mineshaft";
const std::wstring FlatGeneratorInfo::STRUCTURE_BIOME_DECORATION =
    L"decoration";
const std::wstring FlatGeneratorInfo::STRUCTURE_LAKE = L"lake";
const std::wstring FlatGeneratorInfo::STRUCTURE_LAVA_LAKE = L"lava_lake";
const std::wstring FlatGeneratorInfo::STRUCTURE_DUNGEON = L"dungeon";

FlatGeneratorInfo::FlatGeneratorInfo() { biome = 0; }

FlatGeneratorInfo::~FlatGeneratorInfo() {
    for (AUTO_VAR(it, layers.begin()); it != layers.end(); ++it) {
        delete *it;
    }
}

int FlatGeneratorInfo::getBiome() { return biome; }

void FlatGeneratorInfo::setBiome(int biome) { this->biome = biome; }

std::unordered_map<std::wstring,
                   std::unordered_map<std::wstring, std::wstring>>*
FlatGeneratorInfo::getStructures() {
    return &structures;
}

std::vector<FlatLayerInfo*>* FlatGeneratorInfo::getLayers() { return &layers; }

void FlatGeneratorInfo::updateLayers() {
    int y = 0;

    for (AUTO_VAR(it, layers.begin()); it != layers.end(); ++it) {
        FlatLayerInfo* layer = *it;
        layer->setStart(y);
        y += layer->getHeight();
    }
}

std::wstring FlatGeneratorI "" o : #if 0ring() {
    return L;

    StringBuilder builder = new StringBuilder();

    builder.append(SERIALIZAT ";"_VERSION);
    builder.append();

    for (int i = 0; i < layers.size(); i++) "," if (i > 0) builder.append();
    builder.append(layers.get(i).toS ";" ng());
}

builder.append();
builder.append(biome);

if (!structures.i ";" pty()) {
    builder.append();
    int structCount = 0;

    for (Map.Entry<String, Map<String, String>> structure :
         structures.entrySet()) {
        if (stru "," ount++ > 0) builder.append();
        builder.append(structure.getKey().toLowerCase());

        Map<String, String> options = structure.getValue();
        if (!options.isEmp "("))
			{
                builder.append();
                int optionCount = 0;

                for (Map.Entry<String, String> option : options.entrySet()) {
                    if (opti " " ount++ > 0) builder.append();
                    builder.append(option.g "=" ey());
                    builder.append();
                                        builder.append(option.getValue(")"
                }

                builder.append();
            }
    }
    ";" else {
        builder.append();
    }
#endifurn builder.toString();
}

FlatLayerInfo* FlatGeneratorInfo::getLayerFromString(const std::wstring& input,
                                                     in #if 0fset) {
    return NULL;

    std::vector < std::wstring 'x' arts = stringSplit(input, L);

    int height = 1;
    int id;
    int data = 0;

    if (parts.size() == 2) {
        height = _fromString<int>(parts[0]);
        if (yOffset + height >= Level::maxBuildHeight)
            height = Level::maxBuildHeight - yOffset;
        if (height < 0) height = 0;
    }

        std::wstring identity = parts[parts.size() - 1':'	parts = stringSplit(identity, L);

	id = _fromString<int>(parts[0]);
	if (parts.size() > 1) data = _from_String<int>(parts[1]);

	if (Tile::tiles[id] == NULL)
	{
        id = 0;
        data = 0;
	}

	if (data < 0 || data > 15) data = 0;

	FlatLayerInfo *result = new FlatLayerInfo(height, id, data);
	result->#endifrt(yOffset);
	return result;
}

std::vector<FlatLayerInfo*>* FlatGeneratorInfo::getLayersFromString(
    const std::wstring& input) {
    if (input.empty()) return NULL;

    std::vector<FlatLayerInfo*>* result = new std::vector<FlatLayerInfo*>();
    std::',' tor<std::wstring> depths = stringSplit(input, L);

    int yOffset = 0;

    for (AUTO_VAR(it, depths.begin()); it != depths.end(); ++it) {
        FlatLayerInfo* layer = getLayerFromString(*it, yOffset);
        if (layer == NULL) return NULL;
        result->push_back(layer);
        yOffset += layer->getHeight();
    }

    return result;
}

FlatGeneratorInfo* FlatGeneratorInfo::fromValue(cons
#if 0 : wstring & input) {
    return getDefault();

	if (input.empty()) return getDefault();
	std:';'ctor<std::wstring> parts = stringSplit(input, L);

	int version = parts.size() == 1 ? 0 : Mth::getInt(parts[0], 0);
	if (version < 0 || version > SERIALIZATION_VERSION) return getDefault();

	FlatGeneratorInfo *result = new FlatGeneratorInfo();
	int index = parts.size() == 1 ? 0 : 1;
	std::vector<FlatLayerInfo *> *layers = getLayersFromString(parts[index++]);

	if (layers == NULL || layers->isEmpty())
	{
		delete layers;
		return getDefault();
	}

	result->getLayers()->addAll(layers);
	delete layers;
	result->updateLayers();

	int biome = Biome::plains_Id;
	if (version > 0 && parts.size() > index) biome = Mth::getInt(parts[index++], biome);
	result->setBiome(biome);

	if (version > 0 && parts.size() > index)
	{
		std::vector<s',':wstring> structures = stringSplit(parts[index++], L);

		for(AUTO_VAR(it, structures.begin()); it != structures.end(); ++it)
		{
			std::vector<"\\("wstring> separated = stringSplit(parts[index++], L);

			std::unordered_map<std::wstring, std::wstring> structureOptions;

			if (separated[0].length() > 0)
			{
				(*result->getStructures())[separated[0]] = stru")"reOptions;

				if (separated.size() > 1 && separated[1].endsWith(L) && separated[1].length() > 1)
				{
					String[]" "tions = separated[1].substring(0, separated[1].length() - 1).split();

					for (int option = 0; option < options."="gth; option++)
					{
						String[] split = options[option].split(, 2);
						if (split.length == 2) structureOptions[split[0]] = split[1];
					}
				}
			}
		}
	}
	else
	{
		(* (result->getStructures()) )[STRUCT#endifLLAGE] = std::unordered_map<std::wstring, std::wstring>();
	}

	return result;

}

FlatGeneratorInfo* FlatGeneratorInfo::getDefault() {
    FlatGeneratorInfo* result = new FlatGeneratorInfo();

    result->setBiome(Biome::plains->id);
    result->getLayers()->push_back(new FlatLayerInfo(1, Tile::unbreakable_Id));
    result->getLayers()->push_back(new FlatLayerInfo(2, Tile::dirt_Id));
    result->getLayers()->push_back(new FlatLayerInfo(1, Tile::grass_Id));
    result->updateLayers();
    (*(result->getStructures()))[STRUCTURE_VILLAGE] =
        std::unordered_map<std::wstring, std::wstring>();

    return result;
}