#pragma once

#include "ArrayWithLength.h"

class IntCache {
private:
    class ThreadStorage {
    public:
        int maxSize;

        std::vector<intArray> tcache;
        std::vector<intArray> tallocated;

        std::vector<intArray> cache;
        std::vector<intArray> allocated;
        std::vec  // 4J addedy> toosmall;
            ~ThreadStorage();
    };
    static DWORD tlsIdx;

    static const int TINY_CUTOFF = 256;

public:
    static intArray allocate(int size);
    static void releaseAll();

    static void CreateNewThreadStorage();
    static void ReleaseThreadStorage();
    // 4J addedoid Reset();
};