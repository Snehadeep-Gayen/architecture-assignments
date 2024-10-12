#include "branchpredictor.h"
#include <cstdint>
#include <cstring>
#include <iostream>

namespace BranchPredictor 
{
    Gshare::Gshare(int m, int n) : n(n), m(m), bhr(n), counters(1<<m)
    {
        memset(&stats, 0, sizeof(stats));   // clear the stats
    }

    void Gshare::predict_and_check(uint64_t mem, bool taken)
    {
        // determine the first n bits of the index
        uint64_t bhrval = bhr.get_value();
        uint64_t memval = (mem>>(m-n+2));
        uint64_t higherBits = (bhrval^memval);
        uint64_t lowerBits =  ((mem>>2) & ((1<<(m-n))-1));
        uint64_t index = ((higherBits<<(m-n)) + lowerBits) & ((1<<m)-1);

        // std::cerr << "######################################\n";
        // std::cerr << "Memory value: " << mem << "\n";
        // std::cerr << "BHR value: " << bhrval << "\n";
        // std::cerr << "Higher bits: " << higherBits << "\n";
        // std::cerr << "Lower bits: " << lowerBits << "\n";
        // std::cerr << "Index: " << index << "\n";
        // std::cerr << "######################################\n";

        assert(index < counters.size());
        bool prediction = counters[index].predict();
        if(prediction != taken)
            stats.mispreds++;
        stats.branches++;
        if(taken)
            counters[index].increment();
        else
            counters[index].decrement();
        bhr.update(taken);
    }

    void Gshare::print_counters(void)
    {
        for(int i=0; i<counters.size(); i++)
        {
            std::cout << i << "\t";
            counters[i].print();
            std::cout << "\n";
        }
    }
}