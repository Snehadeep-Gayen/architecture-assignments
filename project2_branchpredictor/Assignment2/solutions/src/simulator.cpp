#include <cstring>
#include "simulator.h"
#include <iomanip>

namespace Simulator
{
    Simulator::Simulator(Config conf) : conf(conf), traces(), bp(conf.m, conf.n)
    {
        // parse the file and get the trace
        std::ifstream tracefile(conf.trace_filename);
        assert(tracefile.is_open());

        while (!tracefile.eof())
        {
            char c = 0;
            std::string memStr;
            int64_t mem;

            tracefile >> memStr >> c;
            if (c != 't' && c != 'n')
                break;
            mem = std::stoll(memStr, nullptr, 16);
            traces.push_back({.mem = mem, .taken = (c == 't')});
        }
    }

    void Simulator::Start(void)
    {
        // std::cerr << traces.size() << " is the number of traces\n";
        for(auto trace : traces)
            bp.predict_and_check(trace.mem, trace.taken);
        auto stats = bp.GetStats();
        std::cout << "OUTPUT\n";
        std::cout << " number of predictions: " << stats.branches << "\n";
        std::cout << " number of mispredictions: " << stats.mispreds << "\n";
        std::cout << " misprediction rate: " << std::fixed << std::setprecision(2) << (static_cast<float>(stats.mispreds)*100 / stats.branches) << "%\n";
        if(conf.bimodal)
            std::cout << "FINAL BIMODAL CONTENTS\n";
        else
            std::cout << "FINAL GSHARE CONTENTS\n";
        bp.print_counters();
    }

    // Function to print the Trace struct
    void Simulator::printTrace(const Trace& trace) const
    {
        std::cout << "Trace - Taken: " << (trace.taken ? "True" : "False")
                  << ", Memory: " << trace.mem << std::endl;
    }
}
