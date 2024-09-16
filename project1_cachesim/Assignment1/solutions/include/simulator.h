#include <string>
#include <utility>
#include <fstream>
#include <vector>
#include <iostream>
#include "cache.hpp"

namespace Simulator
{
    struct L1Config
    {
        int sz;
        int assoc;
        int blksz;
        int vc_num_blk;
    };

    struct L2Config
    {
        int sz;
        int assoc;
    };

    struct Config
    {
        struct L1Config l1;
        struct L2Config l2;
        std::string traceFilename;
    };



    // Function to print the Stats struct
    void printStats(const Cache::Cache::Stats& stats) {
        std::cout << "Stats Information:" << std::endl;
        std::cout << "Reads:       " << stats.reads << std::endl;
        std::cout << "Read Misses: " << stats.readMiss << std::endl;
        std::cout << "Writes:      " << stats.writes << std::endl;
        std::cout << "Write Misses:" << stats.writeMiss << std::endl;
        std::cout << "Swap Reqs:   " << stats.swapReqs << std::endl;
        std::cout << "Swaps:       " << stats.swaps << std::endl;
    }

    class Simulator
    {
        public:

        struct Trace
        {
            bool read;
            int64_t mem;
        };
            // Function to print Trace struct
        void printTrace(const Trace& trace) const {
            std::cout << "Trace - Read: " << (trace.read ? "True" : "False")
                    << ", Memory: " << trace.mem << std::endl;
        }

        void PrintConfig(void)
        {
            std::cout << "===== Simulator configuration =====\n";
            std::cout << " L1_SIZE:\t" << conf.l1.sz << "\n";
            std::cout << " L1_ASSOC:\t" << conf.l1.assoc << "\n";
            std::cout << " L1_BLOCKSIZE:\t" << conf.l1.blksz << "\n";
            std::cout << " VC_NUM_BLOCKS:\t" << conf.l1.vc_num_blk << "\n";
            std::cout << " L2_SIZE:\t" << conf.l2.sz << "\n";
            std::cout << " L2_ASSOC:\t" << conf.l2.assoc << "\n";
            std::cout << " trace_file:\t" << conf.traceFilename << "\n\n";
        }

        Simulator(Config conf) : conf(conf) 
        {
            // ready the L1 cache
            l1.emplace(conf.l1.sz, conf.l1.blksz, conf.l1.assoc, conf.l1.vc_num_blk!=0, conf.l1.vc_num_blk);

            // ready the l2 cache if sz is not 0
            if(conf.l2.sz != 0)
                l2.emplace(conf.l2.sz, conf.l1.blksz, conf.l2.assoc, false, 0);
            
            // parse the file and get the trace
            std::ifstream tracefile(conf.traceFilename);
            assert(tracefile.is_open());

            while(!tracefile.eof())
            {
                char c = 0;
                std::string memStr;
                int64_t mem;

                tracefile >> c >> memStr;
                if(c!='r' && c!='w')
                    break;
                mem = std::stoll(memStr, nullptr, 16);
                traces.push_back({c=='r', mem});
            }

            PrintConfig();
        };

        void Start(void) 
        {
            for(auto trace : traces)
            {
                printTrace(trace);
                if(trace.read)
                {
                    auto status1 = l1.value().Read(trace.mem);
                    if(status1 == Cache::MemStatus::MISS)
                    {
                        l1.value().Allocate(trace.mem);
                        // try in L2 cache
                        if(l2.has_value() && l2.value().Read(trace.mem)==Cache::MemStatus::MISS)
                        {
                            l2.value().Allocate(trace.mem);
                        }
                    }
                }
                else
                {
                    auto status1 = l1.value().Write(trace.mem);
                    if(status1 == Cache::MemStatus::MISS)
                    {
                        l1.value().Allocate(trace.mem, false);
                        // try in L2 cache
                        if(l2.has_value() && l2.value().Write(trace.mem)==Cache::MemStatus::MISS)
                        {
                            l2.value().Allocate(trace.mem, false);
                        }
                    }
                }
            }

            std::cout << "===== L1 contents =====\n";
            l1.value().Print();
            std::cout << "\n";

            auto stats1 = l1.value().GetStats();
            printStats(stats1);
        };

        private:

        Config conf;
        std::optional<Cache::Cache> l1, l2;
        std::vector<struct Trace> traces;
    };
    
}