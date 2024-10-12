#include <cassert>
#include <cstring>
#include <string>
#include "simulator.h"

int main(int argc, char* argv[])
{
    assert(argc==4 || argc==5); // Maybe change it to a throw_if later

    Simulator::Config conf;

    // collect the command line arguments
    if(std::strcmp(argv[1], "bimodal")==0)
        conf.bimodal = true;
    else if(std::strcmp(argv[1], "gshare")==0)
        conf.bimodal = false;
    else
    {
        std::cerr << "Unrecognised branch predictor type: " << argv[1];
        exit(0);
    }

    conf.m = std::atoi(argv[2]);
    conf.trace_filename = std::string(argv[argc-1]);
    conf.n = (argc==4)? 0 : std::atoi(argv[3]);

    // output the input command here
    std::cout << "COMMAND\n";
    for(int i=0; i<argc; i++)
        std::cout << argv[i] << " ";
    std::cout << "\n";

    // now start the simulator with the config
    Simulator::Simulator bpsim(conf);
    bpsim.Start();

}