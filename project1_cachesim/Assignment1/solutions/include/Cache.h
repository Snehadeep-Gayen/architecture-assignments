#pragma once

#include "GenCache.h"
#include <optional>

namespace Cache{


    class Cache
    {
    public:

        struct Stats
        {
            int reads;
            int readMiss;
            int writes;
            int writeMiss;
            int swapReqs;
            int swaps;
        };
        
        Cache(int cacheSize, int blkSize, int assoc, bool vcEnable, int vcBlks);

        struct OperationStatus
        {
            bool hit;
            bool dirtyEvicted;
            Addr address;
        };

        OperationStatus Read(Addr address);

        OperationStatus Write(Addr address);

        void Print(void);

        void PrintStats(int type=0);

        Stats GetStats(void) { return stats; };

    private:

        std::pair<int, int> Partition(Addr address);

        GenCache cache;
        std::optional<FullyAssociativeCache> vc;

        // default variables
        int64_t  cacheSize;
        int blkSize;
        int assoc;
        int vcBlks;
        bool vcEnable;

        Stats stats;
    };
}