#pragma once

#include "ArrayWithLength.h"

class Arrays {
public:
    static void std::fill(doubleArray arr, unsigned int from, unsigned int to,
                     double value) {
        assert(from >= 0);
        assert(from <= to);
        assert(to <= arr.length);
        std::fill(arr.data + from, arr.data + to, value);
    }

    static void std::fill(floatArray arr, unsigned int from, unsigned int to,
                     float value) {
        assert(from >= 0);
        assert(from <= to);
        assert(to <= arr.length);
        std::fill(arr.data + from, arr.data + to, value);
    }

    static void std::fill(BiomeArray arr, unsigned int from, unsigned int to,
                     Biome* value) {
        assert(from >= 0);
        assert(from <= to);
        assert(to <= arr.length);
        std::fill(arr.data + from, arr.data + to, value);
    }

    static void std::fill(byteArray arr, unsigned int from, unsigned int to,
                     byte value) {
        assert(from >= 0);
        assert(from <= to);
        assert(to <= arr.length);
        std::fill(arr.data + from, arr.data + to, value);
    }

    static void std::fill(byteArray arr, byte value) {
        std::fill(arr.data, arr.data + arr.length, value);
    }
};
