#pragma once

#include <algorithm>
#include <cstdint>
#include <cassert>
#include <deque>
#include <iostream>
#include <vector>

namespace BranchPredictor
{
    struct Stats
    {
        int branches = 0;
        int mispreds = 0;
    };

    template <int LIMIT>
    class Counter
    {
    public:
        Counter()
        {
            var = (LIMIT+1)/2;
            AssertBounds();
        }

        void increment()
        {
            AssertBounds();
            if(var<LIMIT-1)
                var++;
            AssertBounds();
        }

        void decrement()
        {
            AssertBounds();
            if(var>0)
                var--;
            AssertBounds();
        }

        bool predict()
        {
            AssertBounds();
            if(2*var>=LIMIT)
                return true;
            return false;
        }

        void print()
        {
            std::cout << var;
        }

    private:

        void AssertBounds()
        {
            assert(var>=0 && var<LIMIT);
        }

        int var;
    };

    class BranchHistoryReg
    {
    public:
        BranchHistoryReg(int SZ) : SZ(SZ), value(0), bits(SZ, false)
        {
            // initalise with all values as zero

            // how much to add for setting the first bit to 1
            msb_place_value = (SZ==0)? 0 : (1<<(SZ-1));

            CheckConsistency();
        }

        int64_t get_value(void)
        {
            CheckConsistency();
            return value;
        }

        void update(bool taken)
        {
            CheckConsistency();
            if(!bits.empty())
            {
                bits.pop_back();
                bits.push_front(taken);
            }
            value /= 2;
            if(taken)
                value += msb_place_value;
            CheckConsistency();
        }

    private:

        void CheckConsistency()
        {
            assert(bits.size()==SZ);
            int valcopy = value;
            auto bitscopy = bits;
            std::reverse(bitscopy.begin(), bitscopy.end());
            int current_value = 1;
            for(auto bit : bitscopy)
            {
                if(bit)
                    valcopy -= current_value;
                current_value *= 2;
            }
            assert(valcopy==0);
        }

        int SZ;
        int64_t value;
        std::deque<bool> bits;
        int64_t msb_place_value;
    };


    class Gshare
    {
    public:
        
        /*
        `n` is global history register size
        `m` is number of bits of PC to use (excluding the last 2 bits) 

        It is guaranteed that `m > n`

        if `n` is zero, means Gshare reduces to bimodal 
        */
        Gshare(int n, int m);

        void predict_and_check(uint64_t mem, bool taken);
        void print_counters(void);

        Stats GetStats(void) { return stats; };

    private:

        int n, m;
        Stats stats;

        BranchHistoryReg bhr;
        std::vector<Counter<4>> counters;
    };
}