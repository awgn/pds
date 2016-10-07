#include "pds/hash.hpp"
#include "pds/loglog.hpp"
#include "pds/hyperloglog.hpp"
#include "pds/mangling.hpp"

#include <pcap/pcap.h>

#include <tuple>
#include <iostream>
#include <random>

#include <yats.hpp>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace yats;


using loglog_t = pds::hyperloglog                                                                               
    <  uint8_t                               
    ,  64
    ,  std::hash<std::tuple<uint32_t>>         
    >;                                                                                                                 


loglog_t test_loglog;


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

        // auto tcp = reinterpret_cast<const tcphdr *>(payload + 14 + ip->ihl * 4);

        auto dst_ip   = pds::mangling<13>(ip->daddr);

        test_loglog(std::make_tuple(dst_ip));

    } break;

    case 17: { // UDP

        if (h->caplen < (14 + ip->ihl * 4))
            return;

        // auto udp = reinterpret_cast<const udphdr *>(payload + 14 + ip->ihl * 4);

        auto dst_ip = pds::mangling<13>(ip->daddr);

        test_loglog(std::make_tuple(dst_ip));
        
    } break;

    }
}


auto h = Group("Hyper")

    .Main("pcap", [] (int argc, char *argv[]) 
    {
        char errbuf[PCAP_ERRBUF_SIZE];

        if (argc < 1)
            throw std::runtime_error("usage: pcap:FILE");

        auto p = pcap_open_offline(argv[0], errbuf);
        if (p == nullptr)
            throw std::runtime_error("pcap_open: " + std::string(errbuf));

        if (pcap_loop(p, -1, packet_handler, nullptr) == -1)
            throw std::runtime_error("pcap_loop: " + std::string(pcap_geterr(p)));


        std::cout << "loglog: " << test_loglog.cardinality() << std::endl;

    })
    .Single("simple", []
    {
        pds::hyperloglog<uint8_t, 1024, std::hash<std::string>> llc;

        for(int n = 0; n < 10; n++)
        {
            llc("123ABC" + std::to_string(n));
            std::cout << (n+1) << ": -> " << llc.cardinality() << std::endl;
        }
    })

    .Single("merge", []
    {
        pds::hyperloglog<uint8_t, 1024, std::hash<std::string>> llc;
        llc += llc;

        Assert( llc.cardinality(), is_equal_to(0));
    })
    
    .Single("hashing", []
    {
        pds::hyperloglog<uint8_t, 1024, pds::H2> llc;

        std::mt19937 rand;

        for(int n = 0; n < 100000; n++)
        {
            llc(rand());
            std::cout << (n+1) << ": -> " << llc.cardinality() << std::endl;
        }
    })
    ;


auto g = Group("LogLog")

    .Single("simple", []
    {
        pds::loglog<uint8_t, 1024, std::hash<std::string>> llc;

        for(int n = 0; n < 10; n++)
        {
            llc("123ABC" + std::to_string(n));
            std::cout << (n+1) << ": -> " << llc.cardinality() << std::endl;
        }
    })

    .Single("merge", []
    {
        pds::loglog<uint8_t, 1024, std::hash<std::string>> llc;
        llc += llc;
    })
    ;


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}

