#pragma once

class DataLayer {
public:
    byteArray data;

private:
    const int depthBits;
    const int depthBitsPlusFour;

public:
    DataLayer(int length, int depthBits);
    DataLayer(byteArray data, int depthBits);
    ~DataLayer();

    int std::get(int x, int y, int z);

    void std::set(int x, int y, int z, int val);
    bool isValid();
    void setAll(int br);
};
