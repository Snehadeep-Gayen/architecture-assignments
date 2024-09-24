#include "FullAssocCache.h"
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <climits>


namespace Cache{

    void FullyAssociativeCache::Print(void)
    {
        auto& copy = tags;

        sort(copy.begin(), copy.end(), ([](const Entry& e1, const Entry& e2){
            if(e1.valid && !e2.valid)
                return true;
            
            if(e1.valid && e2.valid && e1.lastAccessed > e2.lastAccessed)
                return true;

            return false;
        }));

        for(const auto& e : copy)
        {
            if(e.valid)
            {
                std::cout << std::hex << e.tag << std::dec << "\t";
                char dirty = e.dirty? 'D' : ' ';
                std::cout << dirty << "\t";
            }
        }
    }

    FullyAssociativeCache::FullyAssociativeCache(int numBlks) : tags(numBlks)
    {
    }

    bool FullyAssociativeCache::IsFull()
    {
        return std::all_of(tags.begin(), tags.end(), 
            [](const Entry& e) 
            { 
                return e.valid; 
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
        assert(IsFull());
        int lastAccessed = INT_MAX;
        int tag = -1;
        for(const auto& e : tags)
            if(e.valid && e.lastAccessed < lastAccessed)
            {
                tag = e.tag;
                lastAccessed = e.lastAccessed;
            }
        assert(tag != -1); // atleast one block should be there
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
        assert(!IsFull());

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
                break;
            }
        }
    }
}