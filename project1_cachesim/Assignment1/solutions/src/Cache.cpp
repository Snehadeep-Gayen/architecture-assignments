#include "Cache.h"
#include <cstring>
#include <iostream>

namespace Cache{

    void Cache::Print(void)
    {
        cache.Print();
    }

    void Cache::PrintVC(void)
    {
        if(vc.has_value())
        {
            std::cout << "set 0:\t";
            vc->Print();
        }
    }

    void Cache::PrintStats(int type)
    {
        std::cout << "Stats Information:" << std::endl;
        std::cout << "Reads:       " << stats.reads << std::endl;
        std::cout << "Read Misses: " << stats.readMiss << std::endl;
        std::cout << "Writes:      " << stats.writes << std::endl;
        std::cout << "Write Misses:" << stats.writeMiss << std::endl;
        std::cout << "Swap Reqs:   " << stats.swapReqs << std::endl;
        std::cout << "Swaps:       " << stats.swaps << std::endl;
    }

    Cache::Cache(int cacheSize, int blkSize, int assoc, bool vcEnable, int vcBlks) 
        : cache(cacheSize, blkSize, assoc), cacheSize(cacheSize), blkSize(blkSize), assoc(assoc), vcBlks(vcBlks), vcEnable(vcEnable)
        {
            std::memset(&stats, 0, sizeof(stats));
            if(vcEnable)
                vc.emplace(vcBlks);
        }

    
    unsigned int Cache::Partition(Addr address)
    {
        return (address / (unsigned int) blkSize); // tag and index
    }

    Cache::OperationStatus Cache::Read(Addr address)
    {
        stats.reads++;
        if(cache.IsPresent(address))
        {
            cache.UpdateLRU(address);
            return {.hit = true,
                .dirtyEvicted = false,
                .address = 0};
        }
        else if(cache.IsThereSpace(address))
        {
            cache.Add(address);
            stats.readMiss++;
            return {.hit = false,
                .dirtyEvicted = false,
                .address = 0};
        }
        else 
        {
            stats.readMiss++;
            // have to make space in the cache
            Block victimBlk = cache.MakeSpace(address);
            cache.Add(address);

            if(!vcEnable)
            {
                // that means this block has to be removed
                Cache::OperationStatus ret;
                ret.hit = false;
                ret.dirtyEvicted = victimBlk.dirty;
                ret.address = victimBlk.address;
                return ret;
            }

            stats.swapReqs++;

            // check if there is block in the victim cache
            auto tagAndIndex = Partition(address);

            Cache::OperationStatus ret;
            ret.hit = false;
            ret.dirtyEvicted = false;

            if(vc->IsPresent(tagAndIndex))
            {
                ret.hit = true; // victim cache hit is same as L1 hit
                stats.swaps++;
                Block blk = vc->Remove(tagAndIndex);
                if(blk.dirty)
                    cache.MarkDirty(address);
            }

            // now its ensured that the vc has no block for the oldTag

            // next we have to ensure that the vc has enough space
            if(vc->IsFull())
            {
                Block blk = vc->Remove(vc->GetLRU());
                ret.dirtyEvicted = blk.dirty;
                ret.address = blk.address;
            }

            // add the evicted block to victim cache
            vc->Add(Partition(victimBlk.address), 
                {.addr = victimBlk.address,
                    .dirty = victimBlk.dirty });

            return ret;
        }
    }

    Cache::OperationStatus Cache::Write(Addr address)
    {

        stats.writes++;
        if(cache.IsPresent(address))
        {
            cache.UpdateLRU(address);
            cache.MarkDirty(address);
            return {.hit = true,
                .dirtyEvicted = false,
                .address = 0};
        }
        else if(cache.IsThereSpace(address))
        {
            cache.Add(address);
            cache.MarkDirty(address);
            stats.writeMiss++;
            return {.hit = false,
                .dirtyEvicted = false,
                .address = 0};
        }
        else 
        {
            stats.writeMiss++;
            // have to make space in the cache
            Block victimBlk = cache.MakeSpace(address);
            cache.Add(address);
            cache.MarkDirty(address);

            if(!vcEnable)
            {
                // that means this block has to be removed
                Cache::OperationStatus ret;
                ret.hit = false;
                ret.dirtyEvicted = victimBlk.dirty;
                ret.address = victimBlk.address;
                return ret;
            }

            stats.swapReqs++;

            // check if there is block in the victim cache
            auto tagAndIndex = Partition(address);

            Cache::OperationStatus ret;
            ret.hit = false;
            ret.dirtyEvicted = false;

            if(vc->IsPresent(tagAndIndex))
            {
                ret.hit = true; // victim cache hit is same as L1 hit
                stats.swaps++;
                Block blk = vc->Remove(tagAndIndex);
                if(blk.dirty)   // Not needed, but still keeping it for clarity
                    cache.MarkDirty(address);
            }

            // now its ensured that the vc has no block for the oldTag

            // next we have to ensure that the vc has enough space
            if(vc->IsFull())
            {
                Block blk = vc->Remove(vc->GetLRU());
                ret.dirtyEvicted = blk.dirty;
                ret.address = blk.address;
            }

            // add the evicted block to victim cache
            vc->Add(Partition(victimBlk.address), // victim cache stores block address (Tag+index) not the tag. 
                {.addr = victimBlk.address,
                .dirty = victimBlk.dirty});

            return ret;
        }

    }

}