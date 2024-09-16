#include <set>
#include <unordered_map>
#include <vector>

namespace Cache
{

    enum MemStatus{
        HIT,
        MISS
    };

    enum AllocStatus{
        FULL,
        SUCCESS
    };

    class FullyAssociativeCache 
    {
    public:

        FullyAssociativeCache(int numBlks);

        MemStatus Access(int tag, bool read=true);
        AllocStatus Allocate(int tag, bool read=true);
        int Evict(void);

        // used to specifically evict the block with tag
        // helpful for victim cache 
        void Evict(int tag);

        bool IsFull(void);

        void Print(void);

    private:
        // configuration
        int numBlks;

        // other variables
        int currentSize;
        int64_t operationCount;

        // memory
        std::set<std::pair<int64_t,int>, std::greater<std::pair<int64_t, int>>> tags;
        std::unordered_map<int,int64_t> operationMap;
        std::unordered_map<int, bool> dirty;
    };

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

        // Returns HIT if the address is present in the cache or the VC
        // Note that no allocation is done on MISS
        MemStatus Read(unsigned int address);

        // Returns HIT if the address is present in the cache or the VC
        // Note that no Allocation is done on MISS
        MemStatus Write(unsigned int address);

        void Allocate(int address, bool read=true);

        struct Stats GetStats(void) { return stats; };

        void Print(void);

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