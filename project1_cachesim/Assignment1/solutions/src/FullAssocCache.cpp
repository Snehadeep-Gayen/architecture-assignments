#include "FullAssocCache.h"
#include <algorithm>
#include <cassert>
#include <stdexcept>


namespace Cache{

    FullyAssociativeCache::FullyAssociativeCache(int numBlks) : tags(numBlks)
    {
    }

    bool FullyAssociativeCache::IsFull()
    {
        return std::any_of(tags.begin(), tags.end(), 
            [](const Entry& e) 
            { 
                return !e.valid; 
            });
    }

    bool FullyAssociativeCache::IsPresent(int tag)
    {
        return std::any_of(tags.begin(), tags.end(),
            [tag](const Entry& e)
            {
                return e.valid && e.tag == tag;
            });
    }

    int FullyAssociativeCache::GetLRU(void)
    {
        int lastAccessed = -1;
        int tag = -1;
        for(const auto& e : tags)
            if(e.valid && e.lastAccessed > lastAccessed)
                tag = e.tag;
        return tag;
    }

    void FullyAssociativeCache::MarkDirty(int tag)
    {
        assert(IsPresent(tag));

        for(auto& e : tags)
            if(e.valid && e.tag == tag)
                e.dirty = true;
    }


    void FullyAssociativeCache::MarkClean(int tag)
    {
        assert(IsPresent(tag));

        for(auto& e : tags)
            if(e.valid && e.tag == tag)
                e.dirty = false;
    }

    void FullyAssociativeCache::UpdateLRU(int tag)
    {
        assert(IsPresent(tag));

        for(auto& e : tags)
            if(e.valid && e.tag == tag)
                e.lastAccessed = operationCount++;
    }

    Block FullyAssociativeCache::Remove(int tag)
    {
        assert(IsPresent(tag));

        for(auto& e : tags)
            if(e.valid && e.tag == tag)
            {
                e.valid = false;
                return Block{
                    .address = e.meta.addr,
                    .dirty = e.dirty
                };
            }
        
        throw std::runtime_error("Impossible 1");
        return Block();
    }

    void FullyAssociativeCache::Add(int tag, metadata_t meta)
    {
        assert(!IsPresent(tag));

        for(auto& e : tags)
        {
            if(!e.valid)
            {
                // better to use a constructor here
                e.valid = true;
                e.dirty = meta.dirty;
                e.lastAccessed = operationCount++;
                e.meta = meta;
                e.tag = tag;
            }
        }
    }
}