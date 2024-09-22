#include <cassert>
#include "Simulator.h"

int main(int argc, char* argv[])
{
    assert(argc==8); // Maybe change it to a throw_if later

    Simulator::Config conf;

    // collect the command line arguments
    conf.l1.sz = std::atoi(argv[1]);
    conf.l1.assoc = std::atoi(argv[2]);
    conf.l1.blksz = std::atoi(argv[3]);
    conf.l1.vc_num_blk = std::atoi(argv[4]);
    conf.l2.sz = std::atoi(argv[5]);
    conf.l2.assoc = std::atoi(argv[6]);
    conf.traceFilename = std::string(argv[7]);

    // now start the simulator with the config
    Simulator::Simulator cachesim(conf);
    cachesim.Start();

    // get the outputs and print them


}