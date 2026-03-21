#pragma once

class FlatLayerInfo;

class FlatGeneratorInfo {
public:
    static const int SERIALIZATION_VERSION = 2;
    static const std::wstring STRUCTURE_VILLAGE;
    static const std::wstring STRUCTURE_BIOME_SPECIFIC;
    static const std::wstring STRUCTURE_STRONGHOLD;
    static const std::wstring STRUCTURE_MINESHAFT;
    static const std::wstring STRUCTURE_BIOME_DECORATION;
    static const std::wstring STRUCTURE_LAKE;
    static const std::wstring STRUCTURE_LAVA_LAKE;
    static const std::wstring STRUCTURE_DUNGEON;

private:
    vector<FlatLayerInfo*> layers;
    unordered_map<std::wstring, unordered_map<std::wstring, std::wstring> >
        structures;
    int biome;

public:
    FlatGeneratorInfo();
    ~FlatGeneratorInfo();

    int getBiome();
    void setBiome(int biome);
    unordered_map<std::wstring, unordered_map<std::wstring, std::wstring> >*
    getStructures();
    vector<FlatLayerInfo*>* getLayers();
    void updateLayers();
    std::wstring toString();

private:
    static FlatLayerInfo* getLayerFromString(const std::wstring& input,
                                             int yOffset);
    static vector<FlatLayerInfo*>* getLayersFromString(
        const std::wstring& input);

public:
    static FlatGeneratorInfo* fromValue(const std::wstring& input);
    static FlatGeneratorInfo* getDefault();
};