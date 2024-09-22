#pragma once

#include <cstdint>
#include <list>
#include <set>
#include <unordered_map>
#include <vector>

namespace Cache
{

    using Addr = unsigned int;

    enum MemStatus{
        HIT,
        MISS
    };

    enum AllocStatus{
        FULL,
        SUCCESS
    };

    struct Block
    {
        Addr address;
        bool dirty;
    };

    class FullyAssociativeCache 
    {
    public:

        struct metadata_t
        {
            Addr addr;
            bool dirty;
        };

        FullyAssociativeCache(int numBlks);

        bool IsPresent(int tag); // doesn't affect any state of the cache

        int GetLRU(void); // does not change the state of the cache, assumes that the cache is full

        void MarkDirty(int tag); // Marks the block with that particular tag as Dirty. Fails if the block is not present in the cache

        void MarkClean(int tag); // Clears the dirty bit of the corresponding block. Fails if the block is not present in the cache

        void UpdateLRU(int tag); // updates the block with the particular tag as the MRU

        Block Remove(int tag); // removes the particular tag from the cache, LRU is updated accordingly

        void Add(int tag, metadata_t meta); // simply adds the particular block with the tag to the cache. Updates the Block added to the MRU 
        // Assumes that the tag is not present in the cache already and that the cache has enough space

        bool IsFull(void);

    private:

        class Entry
        {
        public:
            bool valid = false;
            int tag = 0;
            bool dirty = false;
            int lastAccessed = 0; //  higher the value the more recently it is accessed
            metadata_t meta;
        };

        // configuration
        int numBlks;

        // other variables
        int64_t operationCount = 1;

        // memory
        std::vector<Entry> tags;
    };

    class Cache2
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

        Cache2(int cacheSize, int blkSize, int assoc, bool vcEnable, int vcBlks);

        // Returns HIT if the address is present in the cache or the VC
        // Note that no allocation is done on MISS
        MemStatus Read(unsigned int address);

        // Returns HIT if the address is present in the cache or the VC
        // Note that no Allocation is done on MISS
        MemStatus Write(unsigned int address);

        void Allocate(int address);

    private:

        // Returns tag and index from address
        std::pair<int,int> Partition(unsigned int address);

        MemStatus Read(int tag, int index);

        MemStatus Write(int tag, int index);

        // default variables
        int cacheSize;
        int blkSize;
        int assoc;
        int vcBlks;
        bool vcEnable;

        std::vector<FullyAssociativeCache> sets;
        FullyAssociativeCache vc;

        // initialise the stats
        struct Stats stats;
    };
}