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
        for (auto trace : traces)
        {
            printTrace(trace);

            Cache::Cache::OperationStatus L1status;

            if(trace.read)
            {
                L1status = l1.Read(trace.mem);
            }
            else
            {
                L1status = l1.Write(trace.mem);
            }

            if(!l2.has_value())
                continue;


            // ASK which happens first
            
            if(L1status.dirtyEvicted)
            {
                l2->Write(trace.mem);
            }

            if(!L1status.hit)
            {
                // means data has to be read from L2
                l2->Read(trace.mem);
            }
        }

        std::cout << "===== L1 contents =====\n";
        l1.Print();
        std::cout << "\n";

        l1.PrintStats();
    }

    // Function to print the Trace struct
    void Simulator::printTrace(const Trace& trace) const
    {
        std::cout << "Trace - Read: " << (trace.read ? "True" : "False")
                  << ", Memory: " << trace.mem << std::endl;
    }
}
