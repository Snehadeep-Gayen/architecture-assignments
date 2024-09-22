#include "GenCache.h"
#include <cassert>
#include <iostream>
#include <utility>


namespace Cache
{
    GenCache::GenCache(int cacheSize, int blkSize, int assoc)
        : cacheSize(cacheSize), 
        blkSize(blkSize), 
        assoc(assoc), 
        sets(cacheSize / (blkSize * assoc), {assoc})
    {
    }

    std::pair<int,int> GenCache::Partition(Addr address)
    {
        int tagAndIndex = (address / (unsigned int) blkSize);
        int index  = tagAndIndex % (sets.size());
        int tag = tagAndIndex / (sets.size());
        return std::make_pair(tag, index);
    }

    bool GenCache::IsPresent(Addr address)
    {
        auto [tag, index] = Partition(address);
        return sets[index].IsPresent(tag);
    }

    void GenCache::UpdateLRU(Addr address)
    {
        auto [tag, index] = Partition(address);
        sets[index].UpdateLRU(tag);
    }

    bool GenCache::IsThereSpace(Addr address)
    {
        auto [tag, index] = Partition(address);
        return sets[index].IsFull();
    }

    Block GenCache::MakeSpace(Addr address)
    {
        assert(!IsThereSpace(address));

        auto [tag, index] = Partition(address);
        auto& set = sets[index];
        return set.Remove(set.GetLRU());
    }

    void GenCache::Add(Addr address)
    {
        assert(!IsPresent(address));

        auto [tag, index] = Partition(address);
        sets[index].Add(tag, {.addr = address});
    }

    void GenCache::MarkDirty(Addr address)
    {
        assert(IsPresent(address));

        auto [tag, index] = Partition(address);
        sets[index].MarkDirty(tag);
    }

    void GenCache::MarkClean(Addr address)
    {
        assert(IsPresent(address));

        auto [tag, index] = Partition(address);
        sets[index].MarkClean(tag);
    }

    Block GenCache::Remove(Addr address)
    {
        assert(IsPresent(address));

        auto [tag, index] = Partition(address);
        return sets[index].Remove(tag);
    }

    void GenCache::Print(void)
    {
        std::cout << "TODO: To write print function";
    }
}
