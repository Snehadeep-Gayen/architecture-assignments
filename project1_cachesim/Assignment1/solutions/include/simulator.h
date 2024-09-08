#include <string>

namespace Simulator
{
    struct L1Config
    {
        int sz;
        int assoc;
        int blksz;
        int vc_num_blk;
    };

    struct L2Config
    {
        int sz;
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

        Simulator(Config conf) : conf(conf) {};

        void Start(void) {};

        private:

        Config conf;
    };
    
}