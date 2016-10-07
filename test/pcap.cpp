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


using loglog_t = pds::hyperloglog                                                                               
    <  uint8_t                               
    ,  64
    ,  std::hash<std::tuple<uint32_t, uint16_t, uint16_t>>         
    >;                                                                                                                 
      

pds::sketch< loglog_t                      
    , (1 << 16)
    , pds::ModularHash<BIT_8(H1), BIT_8(H1)>   // IP components...
    , pds::ModularHash<BIT_8(H2), BIT_8(H2)> 
    , pds::ModularHash<BIT_8(H3), BIT_8(H3)> 
    , pds::ModularHash<BIT_8(H4), BIT_8(H4)> 
    , pds::ModularHash<BIT_8(H5), BIT_8(H5)> 
    > test_sketch;



void
packet_handler(u_char *, const struct pcap_pkthdr *h, const u_char *payload)
{
    if (h->caplen < 54)
        return;

    auto ip = reinterpret_cast<const iphdr *>(payload + 14);

    switch(ip->protocol) {

    case 6: { // TCP

        if (h->caplen < (14 + ip->ihl * 4))
            return;

        auto tcp = reinterpret_cast<const tcphdr *>(payload + 14 + ip->ihl * 4);

        test_sketch.foreach_bucket(ip2tuple<13>(ip->daddr), [&](auto &hllc) 
        {
            auto src_ip   = pds::mangling<13>(ip->saddr);
            auto src_port = ntohs(tcp->source);
            auto dst_port = ntohs(tcp->dest);

            hllc(std::make_tuple(src_ip, src_port, dst_port));
        });

    } break;

    case 17: { // UDP

        if (h->caplen < (14 + ip->ihl * 4))
            return;

        auto udp = reinterpret_cast<const udphdr *>(payload + 14 + ip->ihl * 4);

        test_sketch.foreach_bucket(ip2tuple<13>(ip->daddr), [&](auto &hllc) 
        {
            auto src_ip   = pds::mangling<13>(ip->saddr);
            auto src_port = ntohs(udp->source);
            auto dst_port = ntohs(udp->dest);

            hllc(std::make_tuple(src_ip, src_port, dst_port));
        });

    } break;

    }
}


auto g = Group("PCAP")

    .Main("flow-count",  [] (int argc, char *argv[]) {

        char errbuf[PCAP_ERRBUF_SIZE];

        if (argc < 3)
            throw std::runtime_error("usage: pcap:FILE number_injected_flows:INT threshold:INT");

        auto mem = test_sketch.size().first * test_sketch.size().second *  loglog_t{}.size();
        std::cout << "MEMORY: " << mem << " bytes (" << (static_cast<double>(mem)/ (1024*1024)) << " Mb)" << std::endl;

        std::cout << "+ loading sketch..." << std::endl;

        auto p = pcap_open_offline(argv[0], errbuf);
        if (p == nullptr)
            throw std::runtime_error("pcap_open: " + std::string(errbuf));

        if (pcap_loop(p, -1, packet_handler, nullptr) == -1)
            throw std::runtime_error("pcap_loop: " + std::string(pcap_geterr(p)));

        /* inject heavy-hitter flow...*/

        auto dst_ip = inet_addr("1.1.1.1");

        std::mt19937 rand;

        for(auto n = 0; n < atoi(argv[1]); n++)
        {
            test_sketch.foreach_bucket(ip2tuple<13>(dst_ip), [&](auto &hllc) 
            {
                auto src_ip   = rand();
                auto src_port = rand();
                auto dst_port = rand();

                hllc(std::make_tuple(src_ip, src_port, dst_port));
            });
        }

        // 
        // get the index of the buckets whose value is greater than 1000...
        //

        auto thr = atoi(argv[2]);

        auto idx = test_sketch.index_buckets([=](auto &b) { return b.cardinality() > thr; });

        std::cout << "+ reversing sketch..." << std::endl;

        auto start = std::chrono::system_clock::now();

        auto res = pds::reverse_sketch<uint16_t, uint16_t>(test_sketch, idx); 

        std::cout << "> " << res.size() << " candidates found!" << std::endl;

        for(auto & t: res)
            std::cout << "  candidate -> " << inet_ntoa({tuple2ip<13>(t.value)}) << std::endl;

        std::cout << "! reversing sketch done in " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count() << " usec" << std::endl;

    })
    
    ;


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}

