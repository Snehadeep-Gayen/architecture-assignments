#include "cache.hpp"
#include <iostream>

namespace Cache{

    ///////////// Definitions for Fully Associative Cache //////////////

    FullyAssociativeCache::FullyAssociativeCache(int numBlks) 
        : numBlks(numBlks), operationCount(0), currentSize(0)
        {}

    MemStatus FullyAssociativeCache::Access(int tag, bool read)
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
        dirty[tag] = !read;

        return MemStatus::HIT;
    }

    AllocStatus FullyAssociativeCache::Allocate(int tag, bool read)
    {
        assert(operationMap.find(tag)==operationMap.end());

        if(tags.size()>=numBlks)
            return AllocStatus::FULL;

        // update the map
        int64_t opCnt = ++operationCount;
        operationMap[tag] = opCnt;

        // insert the new set
        tags.insert({operationMap[tag], tag});
        dirty[tag] = !read;

        return AllocStatus::SUCCESS;
    }

    int FullyAssociativeCache::Evict(void)
    {
        assert(tags.size()==numBlks);
        int retTag = (*tags.begin()).second;
        tags.erase(tags.begin());
        operationMap.erase(retTag);
        dirty.erase(retTag);
        // TODO: Handle Dirty blocks here
        return retTag;
    }

    void FullyAssociativeCache::Evict(int tag)
    {
        assert(operationMap.find(tag)!=operationMap.end());
        tags.erase(tags.find({operationMap[tag], tag}));
        operationMap.erase(tag);
        dirty.erase(tag);
        // TODO: Handle dirty blocks here
    }

    bool FullyAssociativeCache::IsFull(void)
    {
        return tags.size()>=numBlks;
    }

    void FullyAssociativeCache::Print(void)
    {
        for(auto [counter, tag] : tags)
        {
            std::cout << std::hex << tag << " " << (dirty[tag]?"D":" ") << "\t";
        }
        std::cout << std::dec << "\n";
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

    void Cache::Allocate(int address, bool read)
    {
        auto [tag, index] = Partition(address);
        std::cout << "Allocating " << tag << " " << index << "\n";
        if(sets[index].IsFull())
            sets[index].Evict();
        assert(sets[index].Allocate(tag, read)==AllocStatus::SUCCESS);
    }

    void Cache::Print(void)
    {
        for(int i=0; i<sets.size(); i++)
        {
            std::cout << " set " << i << ":\t";
            sets[i].Print();
        }
    }

    std::pair<int, int> Cache::Partition(unsigned int address)
    {
        int tagAndIndex = (address / (unsigned int) blkSize);
        int index = tagAndIndex % (sets.size());
        int tag = tagAndIndex / (sets.size());

        std::cout << "Address: " << address << ", tag: " << tag << ", index" << index << "\n";
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
            if(vc.Access(tag)==MemStatus::HIT)  // This access will change the LRU status of the VC, what to do now
            {
                stats.swaps++;
                // now I need to remove a particular element from the cache
                vc.Evict(tag);
                int victimBlk = sets[index].Evict();
                sets[index].Allocate(tag); // handle dirty blocks here in a better way
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
        if(sets[index].Access(tag, false)==MemStatus::HIT)
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
                sets[index].Allocate(tag, false);
                vc.Allocate(victimBlk);
                return MemStatus::HIT;
            }
        }

        stats.writeMiss++;
        return MemStatus::MISS;
    }


    ///////////////////////////////////////////////////////////////////

}