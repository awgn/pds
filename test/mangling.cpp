#include "pds/tuple.hpp"
#include "pds/hash.hpp"
#include "pds/range.hpp"
#include "pds/sketch.hpp"
#include "pds/reversible.hpp"
#include "pds/cartesian.hpp"
#include "pds/mangling.hpp"
#include "pds/hyperloglog.hpp"

#include <pcap/pcap.h>

#include <iostream>
#include <random>

#include <yats.hpp>

#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <netinet/in.h>

using namespace yats;
using namespace pds;



auto g = Group("PCAP")

    .Single("mangling", [] {
            auto x0 = pds::mangling<17>(0xdeadbeef);
            Assert(pds::demangling<17>(x0) == 0xdeadbeef);
            
            auto x1 = pds::mangling<81>(0xdeadbeef);
            Assert(pds::demangling<81>(x1) == 0xdeadbeef);
    })

    .Single("ip", [] {

            auto x0 = ip2tuple<13>(0xdeadbeef);
            Assert(tuple2ip<13>(x0) == 0xdeadbeef);
            
            auto x1 = ip2tuple<81>(0xdeadbeef);
            Assert(tuple2ip<81>(x1) == 0xdeadbeef);
    })
    
    ;


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}

