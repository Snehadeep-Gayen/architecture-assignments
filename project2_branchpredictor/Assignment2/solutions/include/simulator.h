#pragma once

#include <cassert>
#include <optional>
#include <string>
#include <utility>
#include <fstream>
#include <vector>
#include <iostream>
#include <memory>
#include "branchpredictor.h"

namespace Simulator
{
    struct Config
    {
        bool bimodal;
        int n;
        int m;
        std::string trace_filename;
    };

    class Simulator
    {
    public:
    
        struct Trace
        {
            int64_t mem;
            bool taken;
        };

        Simulator(struct Config conf);

        void Start(void);

        // Function to print the Trace struct
        void printTrace(const Trace& trace) const;

    private:

        struct Config conf;
        std::vector<Trace> traces;
        BranchPredictor::Gshare bp;
    };

    // Function to print the Stats struct
    void PrintStats(const BranchPredictor::Stats& stats);
}

