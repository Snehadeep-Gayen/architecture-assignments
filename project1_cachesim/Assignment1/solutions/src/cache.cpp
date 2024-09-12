#include "cache.hpp"

namespace Cache{

    ///////////// Definitions for Fully Associative Cache //////////////

    FullyAssociativeCache::FullyAssociativeCache(int numBlks) 
        : numBlks(numBlks), operationCount(0), currentSize(0)
        {}

    MemStatus FullyAssociativeCache::Access(int tag)
    {
        if(operationMap.find(tag)==operationMap.end())
            return MemStatus::MISS;

        // remove the old set
        tags.erase(tags.find({operationMap[tag], tag}));

        // update the map
        int64_t opCnt = ++operationCount;
        operationMap[tag] = opCnt;

        // insert the new set
        tags.insert({operationMap[tag], tag});

        return MemStatus::HIT;
    }

    AllocStatus FullyAssociativeCache::Allocate(int tag)
    {
        assert(operationMap.find(tag)==operationMap.end());

        if(tags.size()>=numBlks)
            return AllocStatus::FULL;

        // update the map
        int64_t opCnt = ++operationCount;
        operationMap[tag] = opCnt;

        // insert the new set
        tags.insert({operationMap[tag], tag});

        return AllocStatus::SUCCESS;
    }

    int FullyAssociativeCache::Evict(void)
    {
        assert(tags.size()==numBlks);
        int retTag = (*tags.begin()).second;
        tags.erase(tags.begin());
        return retTag;
    }

    void FullyAssociativeCache::Evict(int tag)
    {
        assert(operationMap.find(tag)!=operationMap.end());
        tags.erase(tags.find({operationMap[tag], tag}));
    }

    bool FullyAssociativeCache::IsFull(void)
    {
        return tags.size()>=numBlks;
    }

    ///////////////////////////////////////////////////////////////////



    //////////////////// Definitions for Cache ////////////////////////


    Cache::Cache(int cacheSize, int blkSize, int assoc, bool vcEnable, int vcBlks)
        : cacheSize(cacheSize), blkSize(blkSize), assoc(assoc), vcEnable(vcEnable), vcBlks(vcBlks), 
        sets(cacheSize / (blkSize * assoc), {assoc}), vc(vcBlks)
    {
        stats = {0, 0, 0, 0, 0, 0}; // Initialize the stats
    }

    MemStatus Cache::Read(unsigned int address)
    {
        auto [tag, index] = Partition(address);
        return Read(tag, index);
    }

    MemStatus Cache::Write(unsigned int address)
    {
        auto [tag, index] = Partition(address);
        return Write(tag, index);
    }

    void Cache::Allocate(int address)
    {
        auto [tag, index] = Partition(address);
        // Allocation logic here (not expanded)
    }

    std::pair<int, int> Cache::Partition(unsigned int address)
    {
        int tagAndIndex = (address / (unsigned int) blkSize);
        int index = tagAndIndex % assoc;
        int tag = tagAndIndex / assoc;
        return std::make_pair(tag, index);
    }

    MemStatus Cache::Read(int tag, int index)
    {
        stats.reads++;
        if(sets[index].Access(tag)==MemStatus::HIT)
            return MemStatus::HIT;

        if(!sets[index].IsFull())
        {
            stats.readMiss++;
            return MemStatus::MISS;
        }
        
        if(vcEnable)
        {
            stats.swapReqs++;
            if(vc.Access(tag)==MemStatus::HIT)
            {
                stats.swaps++;
                // now I need to remove a particular element from the cache
                vc.Evict(tag);
                int victimBlk = sets[index].Evict();
                sets[index].Allocate(tag);
                vc.Allocate(victimBlk);
                return MemStatus::HIT;
            }
        }

        stats.readMiss++;
        return MemStatus::MISS;
    }

    // Write is same as read, because there is no Allocation
    MemStatus Cache::Write(int tag, int index)
    {

        stats.writes++;
        if(sets[index].Access(tag)==MemStatus::HIT)
            return MemStatus::HIT;

        if(!sets[index].IsFull())
        {
            stats.writeMiss++;
            return MemStatus::MISS;
        }
        
        if(vcEnable)
        {
            stats.swapReqs++;
            if(vc.Access(tag)==MemStatus::HIT)
            {
                stats.swaps++;
                // now I need to remove a particular element from the cache
                vc.Evict(tag);
                int victimBlk = sets[index].Evict();
                sets[index].Allocate(tag);
                vc.Allocate(victimBlk);
                return MemStatus::HIT;
            }
        }

        stats.writeMiss++;
        return MemStatus::MISS;
    }


    ///////////////////////////////////////////////////////////////////

}