#include "Simulator.h"
#include <cstring>

namespace Simulator
{
    Simulator::Simulator(struct Config conf) : conf(conf), traces(),
        l1(conf.l1.sz, conf.l1.blksz, conf.l1.assoc, conf.l1.vc_num_blk != 0, conf.l1.vc_num_blk)
    {

        std::memset(&l1stats, 0, sizeof(Stats));
        std::memset(&l2stats, 0, sizeof(Stats));
        std::memset(&vcstats, 0, sizeof(Stats));

        // ready the l2 cache if sz is not 0
        if (conf.l2.sz != 0)
            l2.emplace(conf.l2.sz, conf.l1.blksz, conf.l2.assoc, false, 0);

        // parse the file and get the trace
        std::ifstream tracefile(conf.traceFilename);
        assert(tracefile.is_open());

        while (!tracefile.eof())
        {
            char c = 0;
            std::string memStr;
            int64_t mem;

            tracefile >> c >> memStr;
            if (c != 'r' && c != 'w')
                break;
            mem = std::stoll(memStr, nullptr, 16);
            traces.push_back({c == 'r', mem});
        }

        PrintConfig();
    }

    void Simulator::PrintConfig(void)
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

    void Simulator::Start(void)
    {
        int writeback_from_L1_plus_VC = 0;
        int writeback_from_L2 = 0;
        for (auto trace : traces)
        {
            // printTrace(trace);

            Cache::Cache::OperationStatus L1status;

            // std::cout << (L1status.hit? "hit ":"miss ");
            // if(L1status.dirtyEvicted)
            //     std::cout << "evict";
            // std::cout << "\n";

            if(trace.read)
            {
                L1status = l1.Read(trace.mem);
            }
            else
            {
                L1status = l1.Write(trace.mem);
            }

            if(!l2.has_value())
            {
                if(L1status.dirtyEvicted)
                    writeback_from_L1_plus_VC++;
                continue;
            }

            // ASK which happens first

            if(!L1status.hit)
            {
                // means data has to be read from L2
                Cache::Cache::OperationStatus L2Rstatus = l2->Read(trace.mem);
                if(L2Rstatus.dirtyEvicted)
                    writeback_from_L2++;
            }

            if(L1status.dirtyEvicted)
            {
                writeback_from_L1_plus_VC++;
                Cache::Cache::OperationStatus L2Wstatus = l2->Write(L1status.address);
                if(L2Wstatus.dirtyEvicted)
                    writeback_from_L2++;
            }
        }

        std::cout << "===== L1 contents =====\n";
        l1.Print();
        std::cout << "\n";

        if(conf.l1.vc_num_blk > 0)  // means victim cache is present
        {
            std::cout << "===== VC contents =====\n";
            l1.PrintVC();
            std::cout << "\n";
        }

        if(l2.has_value())
        {
            std::cout << "===== L2 contents =====\n";
            l2->Print();
            std::cout << "\n";
        }

        Cache::Cache::Stats l1stats = l1.GetStats();

        std::cout << "\n===== Simulation results (raw) =====\n";
        
        std::cout << "a. number of L1 reads:\t\t\t" << l1stats.reads << std::endl;
        std::cout << "b. number of L1 read misses:\t\t\t" << l1stats.readMiss << std::endl;
        std::cout << "c. number of L1 writes:\t\t\t" << l1stats.writes << std::endl;
        std::cout << "d. number of L1 write misses:\t\t\t" << l1stats.writeMiss << std::endl;
        std::cout << "e. number of swap requests:\t\t\t" << l1stats.swapReqs << std::endl;
        std::cout << "f. swap request rate:\t\t\t" << std::fixed << std::setprecision(4) << l1stats.swapReqs * 1.0f / (l1stats.reads + l1stats.writes) << std::endl;
        std::cout << "g. number of swaps:\t\t\t" << l1stats.swaps << "\n";
        std::cout << "h. combined L1+VC miss rate:\t\t\t" << (l1stats.readMiss + l1stats.writeMiss - l1stats.swaps) * 1.0f / (l1stats.reads + l1stats.writes) << std::endl;
        std::cout << "i. number writebacks from L1/VC:\t\t\t" << writeback_from_L1_plus_VC << std::endl;

        Cache::Cache::Stats l2stats;
        std::memset(&l2stats, 0, sizeof(l2stats));

        if(l2.has_value())
        {
            l2stats = l2->GetStats();
        }

        std::cout << "j. number of L2 reads:\t\t\t" << l2stats.reads << std::endl;
        std::cout << "k. number of L2 read misses:\t\t\t" << l2stats.readMiss << std::endl;
        std::cout << "l. number of L2 writes:\t\t\t" << l2stats.writes << std::endl;
        std::cout << "m. number of L2 write misses:\t\t\t" << l2stats.writeMiss << std::endl;
        if(l2.has_value())
        {
            std::cout << "n. L2 miss rate:\t\t\t" << std::fixed << std::setprecision(4) << (l2stats.readMiss) * 1.0f / (l2stats.reads) << std::endl;
        }
        else
        {
            std::cout << "n. L2 miss rate:\t\t\t" << std::fixed << std::setprecision(4) << 0 << std::endl;
        }
        std::cout << "o. number of writebacks from L2 to memory:\t" << writeback_from_L2 << std::endl;
        if(l2.has_value())
        {
            std::cout << "p. total memory traffic:\t\t\t" << (l2stats.readMiss + l2stats.writeMiss + writeback_from_L2) << std::endl;
        }
        else
        {
            std::cout << "p. total memory traffic:\t\t\t" << (l1stats.readMiss + l1stats.writeMiss - l1stats.swaps + writeback_from_L1_plus_VC) << std::endl;
        }
    }

    // Function to print the Trace struct
    void Simulator::printTrace(const Trace& trace) const
    {
        std::cout << "Trace - Read: " << (trace.read ? "True" : "False")
                  << ", Memory: " << trace.mem << std::endl;
    }
}
