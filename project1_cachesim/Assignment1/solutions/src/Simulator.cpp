#include "Simulator.h"
#include <cstring>
#include "Parse.h"

namespace Simulator
{
    std::unique_ptr<CacheMetrics> Simulator::GetMetrics(int sz, int blksize, int assoc)
    {
        CacheMetrics cm;
        if(get_cacti_results(sz, blksize, assoc, &cm.accessTime, &cm.energy, &cm.area)<0)
        {
            cm.accessTime = cm.area = cm.energy = -1;
        }
        return std::make_unique<CacheMetrics>(cm);
    }

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
                L1status = l1.Read(trace.mem);
            else
                L1status = l1.Write(trace.mem);

            if(!l2.has_value())
            {
                if(L1status.dirtyEvicted)
                    writeback_from_L1_plus_VC++;
                continue;
            }

            if(!L1status.hit)
            {
                if(L1status.dirtyEvicted)
                {
                    writeback_from_L1_plus_VC++;
                    Cache::Cache::OperationStatus L2Wstatus = l2->Write(L1status.address);
                    if(L2Wstatus.dirtyEvicted)
                        writeback_from_L2++;
                }

                // means data has to be read from L2
                Cache::Cache::OperationStatus L2Rstatus = l2->Read(trace.mem);
                if(L2Rstatus.dirtyEvicted)
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
            std::cout << "\n\n";
        }

        if(l2.has_value())
        {
            std::cout << "===== L2 contents =====\n";
            l2->Print();
            std::cout << "\n";
        }

        Cache::Cache::Stats l1stats = l1.GetStats();

        std::cout << "===== Simulation results (raw) =====\n";
        
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
            std::cout << "n. L2 miss rate:\t\t\t" << std::fixed << std::setprecision(4) << 0.0f << std::endl;
        }
        std::cout << "o. number of writebacks from L2:\t" << writeback_from_L2 << std::endl;
        if(l2.has_value())
        {
            std::cout << "p. total memory traffic:\t\t\t" << (l2stats.readMiss + l2stats.writeMiss + writeback_from_L2) << std::endl;
        }
        else
        {
            std::cout << "p. total memory traffic:\t\t\t" << (l1stats.readMiss + l1stats.writeMiss - l1stats.swaps + writeback_from_L1_plus_VC) << std::endl;
        }

        std::cout << "\n===== Simulation results (performance) =====\n";
        CacheMetrics l1_metrics, l2_metrics, vc_metrics;
        l1_metrics = *GetMetrics(conf.l1.sz, conf.l1.blksz, conf.l1.assoc);
        assert(l1_metrics.accessTime>0);
        if(l2.has_value())
        {
            l2_metrics = *GetMetrics(conf.l2.sz, conf.l1.blksz, conf.l2.assoc);
            assert(l2_metrics.accessTime>0);
        }
        if(conf.l1.vc_num_blk>0)
        {
            vc_metrics = *GetMetrics(conf.l1.vc_num_blk * conf.l1.blksz, conf.l1.blksz, conf.l1.vc_num_blk);
            if(vc_metrics.accessTime<0)
                vc_metrics.accessTime = 0.2;    // default is 0.2ns
        }

        // calculate the performance metrics

        float l1_time = l1_metrics.accessTime * (l1stats.reads + l1stats.writes);
        float vc_time = vc_metrics.accessTime * (l1stats.swapReqs);
        float l2_time = l2_metrics.accessTime * (l2stats.reads + l2stats.writes);
        float mm_time = (20.0 + conf.l1.blksz / 16.0) * (writeback_from_L1_plus_VC + writeback_from_L2);
        float total_time = (l1_time + vc_time + l2_time + mm_time);
        float aat =  total_time / (l1stats.reads + l1stats.writes);

        std::cout << "1. average access time:\t\t\t" << std::fixed << std::setprecision(4) << aat << "\n";

        float l1_energy = l1_metrics.energy * (l1stats.reads + l1stats.writes + l1stats.readMiss + l2stats.writeMiss);
        float vc_energy = 0;
        if(vc.has_value())
        {
            vc_energy = vc_metrics.energy * (l1stats.swapReqs) * 2;
        }
        float l2_energy = 0;
        float mem_energy = 0;
        if(l2.has_value())
        {
            l2_energy = l2_metrics.energy * (l2stats.reads + l2stats.writes + l2stats.readMiss + l2stats.writeMiss);
            mem_energy = 0.05 * (writeback_from_L2 + l2stats.readMiss + l2stats.writeMiss);
        }
        else
        {
            mem_energy = 0.05 * (writeback_from_L1_plus_VC + l1stats.readMiss + l1stats.writeMiss - l1stats.swaps);
        }
        float total_energy = l1_energy + vc_energy + l2_energy + mem_energy;
        std::cout << "2. energy-delay product:\t\t\t" << total_energy * total_time << "\n";

        float total_area = l1_metrics.area + l2_metrics.area + vc_metrics.area;
        std::cout << "3. total area:\t\t" << total_area << "\n";
    }

    // Function to print the Trace struct
    void Simulator::printTrace(const Trace& trace) const
    {
        std::cout << "Trace - Read: " << (trace.read ? "True" : "False")
                  << ", Memory: " << trace.mem << std::endl;
    }
}
