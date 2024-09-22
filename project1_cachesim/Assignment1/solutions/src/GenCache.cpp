#include "GenCache.h"
#include <cassert>
#include <iostream>
#include <utility>


namespace Cache
{
    Cache::Cache(int cacheSize, int blkSize, int assoc, bool vcEnable, int vcBlks)
        : cacheSize(cacheSize), 
        blkSize(blkSize), 
        assoc(assoc), 
        vcEnable(vcEnable), 
        vcBlks(vcBlks), 
        sets(cacheSize / (blkSize * assoc), {assoc})
    {
    }

    std::pair<int,int> Cache::Partition(Addr address)
    {
        int tagAndIndex = (address / (unsigned int) blkSize);
        int index  = tagAndIndex % (sets.size());
        int tag = tagAndIndex / (sets.size());
        return std::make_pair(tag, index);
    }

    bool Cache::IsPresent(Addr address)
    {
        auto [tag, index] = Partition(address);
        return sets[index].IsPresent(tag);
    }

    void Cache::UpdateLRU(Addr address)
    {
        auto [tag, index] = Partition(address);
        sets[index].UpdateLRU(tag);
    }

    bool Cache::IsThereSpace(Addr address)
    {
        auto [tag, index] = Partition(address);
        return sets[index].IsFull();
    }

    Block Cache::MakeSpace(Addr address)
    {
        assert(!IsThereSpace(address));

        auto [tag, index] = Partition(address);
        auto& set = sets[index];
        return set.Remove(set.GetLRU());
    }

    void Cache::Add(Addr address)
    {
        assert(!IsPresent(address));

        auto [tag, index] = Partition(address);
        sets[index].Add(tag, {.addr = address});
    }

    void Cache::MarkDirty(Addr address)
    {
        assert(IsPresent(address));

        auto [tag, index] = Partition(address);
        sets[index].MarkDirty(tag);
    }

    void Cache::MarkClean(Addr address)
    {
        assert(IsPresent(address));

        auto [tag, index] = Partition(address);
        sets[index].MarkClean(tag);
    }

    Block Cache::Remove(Addr address)
    {
        assert(IsPresent(address));

        auto [tag, index] = Partition(address);
        return sets[index].Remove(tag);
    }

    void Cache::Print(void)
    {
        std::cout << "TODO: To write print function";
    }
}
