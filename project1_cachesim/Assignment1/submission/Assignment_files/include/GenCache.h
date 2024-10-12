#pragma once 

#include "FullAssocCache.h"

namespace Cache{


class GenCache 
{
    public:

        GenCache(int cacheSize, int blkSize, int assoc);

        bool IsPresent(Addr address); // does not update the state of the cache, just tells if the block is there in the cache or not

        void UpdateLRU(Addr address); // updates the block address as the MRU in its set. Assumes that the address is already present in the cache 

        bool IsThereSpace(Addr address); // does not update any status of the cache. Just checks if there is enough space to fit the block in the cache

        Block MakeSpace(Addr address); // assumes that there is no space for the address

        void Add(Addr address); // adds the particular block with the 
        
        void MarkDirty(Addr address); // marks the cache block dirty

        void MarkClean(Addr address); // marks the corresponding cache block as clean

        Block Remove(Addr address); // removes the block and returns it. Also updates LRU accordingly

        void Print(void);

        // ideally this function should be private, but because of my bad design choice, currently im sticking to this
        // need to change it later to something that is more clear
        std::pair<int, int> Partition(Addr address);

    private:

        // default variables
        int64_t  cacheSize;
        int blkSize;
        int assoc;

        std::vector<FullyAssociativeCache> sets;
};


}