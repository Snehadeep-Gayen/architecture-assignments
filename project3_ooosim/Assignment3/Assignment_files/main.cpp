#include <vector>
#include <memory>
#include <iostream>
#include <cassert>
#include <optional>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>

using namespace std;

/*
    Check this:
    I think that tag may not be assigned to an instruction with no dst register
*/
struct Instruction
{
    static int instruction_uid;
    // book keeping
    int64_t tag;
    optional<int> dst_tag;
    optional<int> src1_tag;
    optional<int> src2_tag;
    int stage;  // one of Not started(0), IF (1), ID (2), IS (3), EX (4), WB (5), Completed (6).
    vector<int64_t> time_spent; // array of seven values

    // mentioned in the question
    uint32_t pc;
    int op;
    optional<int> dst;
    optional<int> src1;
    optional<int> src2;

    Instruction(uint32_t addr, int op, int dst, int src1, int src2) : pc(addr), op(op)
    {
        if(dst!=-1)
            Instruction::dst = dst;
        else
            Instruction::dst = nullopt;

        if(src1!=-1)
            Instruction::src1 = src1;
        else
            Instruction::src1 = nullopt;

        if(src2!=-1)
            Instruction::src2 = src2;
        else
            Instruction::src2 = nullopt;

        dst_tag = src1_tag = src2_tag = nullopt;
        tag = Instruction::instruction_uid++;
        stage = 0;
        time_spent = vector<int64_t>(7, 0);
    }

};
// Overload << operator to output Instruction in the specified format
ostream& operator<<(ostream& os, const Instruction& inst) {
    os << inst.tag << "\t";
    os << "fu{" << inst.op << "}\t";
    
    os << "src{" 
       << (inst.src1 ? inst.src1.value() : -1) << ","
       << (inst.src2 ? inst.src2.value() : -1) 
       << "}\t";
       
    os << "dst{" << (inst.dst ? *inst.dst : -1) << "} ";

    // Format time_spent for each pipeline stage
    int sum_time = inst.time_spent[0]-1;
    os << "IF{" << sum_time << "," << inst.time_spent[1] << "}\t";
    sum_time += inst.time_spent[1];
    os << "ID{" << sum_time << "," << inst.time_spent[2] << "}\t";
    sum_time += inst.time_spent[2];
    os << "IS{" << sum_time << "," << inst.time_spent[3] << "}\t";
    sum_time += inst.time_spent[3];
    os << "EX{" << sum_time << "," << inst.time_spent[4] << "}\t";
    sum_time += inst.time_spent[4];
    os << "WB{" << sum_time << "," << inst.time_spent[5] << "}\t";

    return os;
}

int Instruction::instruction_uid = 0;

int main(int argc, char* argv[])
{
    assert(argc==4);
    int n = atoi(argv[1]);
    int s = atoi(argv[2]);
    ifstream tracefile(argv[3]);
    if(!tracefile.is_open())
        throw std::runtime_error("Tracefile not found");


    std::vector<Instruction> insts;
    while(!tracefile.eof())
    {
        uint32_t addr;
        tracefile >> std::hex >> addr;
        int op, dst, s1, s2;
        tracefile >> std::dec >> op >> dst >> s1 >> s2;
        insts.push_back({addr, op, dst, s1, s2});
    }

    // now start the pipeline
    int noEx=0;     // number of instructions using the functional units
    int noSched=0;  // number of instructions in the Scheduling queue
    int noDispatch=0;   // number of instructions in the dispatch q
    int noWaitingFetch=0; // number of instructions fetched
    int simtime=0;
    int tag=0;

    vector<int> regs(128, -1);
    while(1)
    {
        std::cout << string(30, '*') << "\n";
        std::cout << noWaitingFetch << ",\t" << noDispatch << ",\t" << noSched << ",\t" << noEx << "\n";

        // check if any instruction is in writeback stage
        for(auto& inst : insts)
        {
            if(inst.stage!=5)
                continue;

            if(inst.dst.has_value() and inst.tag>=regs[inst.dst.value()])
                regs[inst.dst.value()] = -1;    // means there is no tag, and the register value is up to date
            inst.time_spent[inst.stage]++;
            inst.stage++;
            std::cout << "Instruction #" << inst.tag << " completes WB\n";
        }

        // check if any instruction is still executing
        for(auto& inst : insts)
        {
            if(inst.stage!=4)
                continue;

            inst.time_spent[inst.stage]++;
            int time_ex = inst.time_spent[inst.stage];
            if(inst.op==0 && time_ex==1 || 
               inst.op==1 && time_ex==2 ||
               inst.op==2 && time_ex==10)
            {
                std::cout << "Instruction #" << inst.tag << " completes execution\n";
                // execute stage is over
                noEx--;
                inst.stage++;
            }
        }

        // check the waiting instructions in IS stage & issue them is possible
        for(auto& inst : insts)
        {
            if(inst.stage!=3)
                continue;

            inst.time_spent[inst.stage]++;
            // check if all sources are present
            if(inst.src1_tag.has_value() && insts[inst.src1_tag.value()].stage>=5) // means that first source operand is ready
                inst.src1_tag = nullopt;
            if(inst.src2_tag.has_value() && insts[inst.src2_tag.value()].stage>=5) // means that second source operand is ready
                inst.src2_tag = nullopt;
            if(noEx<n && !inst.src1_tag.has_value() && !inst.src2_tag.has_value())
            {
                std::cout << "Instruction #" << inst.tag << " starts execution\n";
                noEx++;
                inst.stage++;
                noSched--;
            }
            else if(inst.tag==1)
            {
                // std::cout 
                std::cout << noEx << " " << inst.src1_tag.has_value() << " " << inst.src2_tag.has_value() << "\n";
            }
        }

        int noDispatched = 0;
        // check the waiting instructions in ID stage & Dispatch them to Scheduling area
        for(auto& inst : insts)
        {
            if(inst.stage!=2)
                continue;

            inst.time_spent[inst.stage]++;
            if(noDispatched<n && noSched<s)
            {
                // check if all operands are present
                if(inst.src1.has_value() and regs[inst.src1.value()]!=-1)
                    inst.src1_tag = regs[inst.src1.value()];
                if(inst.src2.has_value() and regs[inst.src2.value()]!=-1)
                    inst.src2_tag = regs[inst.src2.value()];
                // mark the destination as incorrect
                if(inst.dst.has_value() && inst.tag>regs[inst.dst.value()])
                    regs[inst.dst.value()] = inst.tag;
                std::cout << "Instruction #" << inst.tag << " scheduled\n";
                inst.stage++;
                noDispatched++;
                noSched++;
                noDispatch--;
            }
        }

        int noInitiated = 0;
        // check the waiting instructions in IF stage & Initiate them them to Dispatch area
        for(auto& inst : insts)
        {
            if(inst.stage!=1)
                continue;

            inst.time_spent[inst.stage]++;
            if(noInitiated<n && noDispatch<2*n)
            {
                std::cout << "Instruction #" << inst.tag << " dispatched\n";
                inst.stage++;
                noInitiated++;
                noDispatch++;
                noWaitingFetch--;
            }
        }

        int noFetched = 0;
        // check number of fetched instructions
        for(auto& inst : insts)
        {
            if(inst.stage!=0)
                continue;

            inst.time_spent[inst.stage]++;
            if(noFetched<n && noWaitingFetch<n)
            {
                std::cout << "Instruction #" << inst.tag << " fetched\n";
                inst.stage++;
                noWaitingFetch++;
                noFetched++;
            }
        }

        bool done = true;
        for(const auto& inst : insts)
            if(inst.stage!=6)
            {
                done = false;
                break;
            }
        if(done)
            break;
    }

    for(const auto& inst : insts)
    {
        cout << inst << "\n";
    }

}