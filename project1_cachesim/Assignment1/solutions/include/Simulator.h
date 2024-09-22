#pragma once

#include <cassert>
#include <optional>
#include <string>
#include <utility>
#include <fstream>
#include <vector>
#include <iostream>
#include "Cache.h"

namespace Simulator
{
    struct L1Config
    {
        int64_t sz;
        int assoc;
        int blksz;
        int vc_num_blk;
    };

    struct L2Config
    {
        int64_t sz;
        int assoc;
    };

    struct Config
    {
        struct L1Config l1;
        struct L2Config l2;
        std::string traceFilename;
    };

    class Simulator
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
        } l1stats, l2stats, vcstats;

        class Trace
        {
        public:
            bool read;
            int64_t mem;
        };

        struct Config conf;
        Cache::Cache l1;
        std::optional<Cache::FullyAssociativeCache> vc;
        std::optional<Cache::Cache> l2;
        std::vector<Trace> traces;

        Simulator(struct Config conf);

        void PrintConfig(void);
        void Start(void);

        // Function to print the Trace struct
        void printTrace(const Trace& trace) const;
    };

    // Function to print the Stats struct
    void PrintStats(const Simulator::Simulator::Stats& stats);
}

