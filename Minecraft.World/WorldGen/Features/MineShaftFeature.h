#pragma once

#include "StructureFeature.h"

class MineShaftFeature : public StructureFeature {
public:
    static const std::wstring OPTION_CHANCE;

private:
    double chance;

public:
    MineShaftFeature();

    std::wstring getFeatureName();

    MineShaftFeature(unordered_map<wstring, wstring> options);

protected:
    virtual bool isFeatureChunk(int x, int z, bool bIsSuperflat = false);
    virtual StructureStart* createStructureStart(int x, int z);
};