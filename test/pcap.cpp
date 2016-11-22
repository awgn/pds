#include "pds/tuple.hpp"

#include "pds/hash.hpp"
#include "pds/range.hpp"
#include "pds/sketch.hpp"
#include "pds/reversible.hpp"
#include "pds/cartesian.hpp"
#include "pds/mangling.hpp"
#include "pds/hyperloglog.hpp"
#include "pds/loglog.hpp"

#include <pcap/pcap.h>

#include <iostream>
#include <random>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <tuple>
#include <cmath>

#include <yats.hpp>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <cat/show.hpp>
#include <cat/functor.hpp>


using namespace yats;
using namespace pds;


#define TEST_DEBUG_LOGLOG


struct HashTriple
{
	uint32_t operator()(int );
	uint32_t operator()(std::tuple<uint32_t, uint32_t, uint32_t> v) const
	{
		pds::Wang7 h;
		return h(std::get<0>(v) ^ std::get<1>(v) ^ std::get<2>(v));
	}

};


using loglog_t = pds::hyperloglog                                                                               
    <  uint8_t                               
    ,  64 
    ,  HashTriple 
    >;                                                                                                                 
      

pds::sketch< loglog_t                      
    , (1 << 16)
    , pds::ModularHash<BIT_8(H1), BIT_8(H1)>   // IP components...
    , pds::ModularHash<BIT_8(H2), BIT_8(H2)> 
    , pds::ModularHash<BIT_8(H3), BIT_8(H3)> 
    , pds::ModularHash<BIT_8(H4), BIT_8(H4)> 
    , pds::ModularHash<BIT_8(H5), BIT_8(H5)> 
    > test_sketch;


#ifdef TEST_DEBUG_LOGLOG

pds::sketch< std::set<std::tuple<uint32_t, uint32_t, uint32_t>> 
    , (1 << 16)
    , pds::ModularHash<BIT_8(H1), BIT_8(H1)>   // IP components...
    , pds::ModularHash<BIT_8(H2), BIT_8(H2)> 
    , pds::ModularHash<BIT_8(H3), BIT_8(H3)> 
    , pds::ModularHash<BIT_8(H4), BIT_8(H4)> 
    , pds::ModularHash<BIT_8(H5), BIT_8(H5)> 
    > test_sketch_set;

#endif

std::unordered_map<uint32_t, std::set<std::tuple<uint32_t, uint32_t, uint32_t> > > test_map;


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
        auto src_ip   = ip->saddr;
        auto src_port = ntohs(tcp->source);
        auto dst_port = ntohs(tcp->dest);

	test_map[ip->daddr]
		.insert(std::make_tuple(src_ip, src_port, dst_port));

        test_sketch.foreach_bucket(ip2tuple<8191>(ip->daddr), [&](auto &hllc) 
        {
            hllc(std::make_tuple(pds::mangling<8191>(src_ip), src_port, dst_port));
        });

#ifdef TEST_DEBUG_LOGLOG
        test_sketch_set.foreach_bucket(ip2tuple<8191>(ip->daddr), [&](auto &hllc) 
        {
            hllc.insert(std::make_tuple(pds::mangling<8191>(src_ip), src_port, dst_port));
        });
#endif

    } break;

    case 17: { // UDP

        if (h->caplen < (14 + ip->ihl * 4))
            return;

        auto udp = reinterpret_cast<const udphdr *>(payload + 14 + ip->ihl * 4);

        auto src_ip   = ip->saddr;
        auto src_port = ntohs(udp->source);
        auto dst_port = ntohs(udp->dest);

	test_map[ip->daddr]
		.insert(std::make_tuple(src_ip, src_port, dst_port));

        test_sketch.foreach_bucket(ip2tuple<8191>(ip->daddr), [&](auto &hllc) 
        {
            hllc(std::make_tuple(pds::mangling<8191>(src_ip), src_port, dst_port));
        });
       
#ifdef TEST_DEBUG_LOGLOG 
	test_sketch_set.foreach_bucket(ip2tuple<8191>(ip->daddr), [&](auto &hllc) 
        {
            hllc.insert(std::make_tuple(pds::mangling<8191>(src_ip), src_port, dst_port));
        });
#endif
    } break;

    }
}


auto g = Group("PCAP")

    .Main("flow-count",  [] (int argc, char *argv[]) {


        std::map<uint32_t, uint32_t> top_hitter;
        std::map<uint32_t, uint32_t> top_candidate;


        char errbuf[PCAP_ERRBUF_SIZE];

        if (argc < 3)
            throw std::runtime_error("usage: pcap:FILE number_injected_flows:INT perc:DOUBLE");

        auto mem = test_sketch.size().first * test_sketch.size().second * loglog_t{}.size();
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
            auto src_ip   = rand();
            auto src_port = rand();
            auto dst_port = rand();

            test_sketch.foreach_bucket(ip2tuple<8191>(dst_ip), [&](auto &hllc) 
            {
                hllc(std::make_tuple(src_ip, src_port, dst_port));
            });
        }

        auto perc = atof(argv[2]);
	
	//
	// get real number of hitter...
	//
	
	size_t total = 0, cnt = 0;
	for(auto &elem : test_map)
	{
	    total+= elem.second.size();
	}

	std::cout << std::endl;
	std::cout << "Total Map/Flow size: " << total << std::endl;

	for(auto &elem : test_map)
	{
	    if (((100.0 * elem.second.size())/total) > perc) {
		cnt++;
            	std::cout << "  hitter -> " << inet_ntoa({elem.first}) << " (" << elem.second.size() << ")" << std::endl;
            	top_hitter[elem.first] = elem.second.size();
	    }
	}

        std::cout << "Total hitter: " << cnt << std::endl;

	//
 	// estimation
	//	

	// dump buckets...
#ifdef TEST_DEBUG_LOGLOG
#if 0
	auto colum = test_sketch.size().second;
	for(size_t x = 0; x < colum; x++) {
		auto exact = eval(test_sketch_set(0, x));
		auto estimate = eval(test_sketch(0, x));
		std::cout << estimate << " " << exact << " "  << "\terror:" << (double)(exact-estimate)/exact << std::endl;
	}
#endif
#endif

	auto minsum = test_sketch.minsum();
#ifdef TEST_DEBUG_LOGLOG
	auto real   = test_sketch_set.minsum();
#endif
	std::cout << "Total minsum: " << minsum;

#ifdef TEST_DEBUG_LOGLOG
	std::cout  << " realsum: " << real;
#endif
	std::cout  << std::endl;

#ifdef TEST_DEBUG_LOGLOG
	// Sketch<std::set> 
	{
		auto idx = test_sketch_set.indexes([=](auto &b, auto) { 
				bool ret = (100.0 * b.size() / minsum) >= perc;	
				return ret;
			   });

		std::cout << "+ reversing sketch<std::set>..." << std::endl;

		auto start = std::chrono::system_clock::now();
		auto res = pds::reverse_sketch<uint16_t, uint16_t>(test_sketch_set, idx); 
		auto end = std::chrono::system_clock::now();

		size_t false_positive = 0;

		for(auto & t: res) {

		    auto it = test_map.find(tuple2ip<8191>(t.value));
		    if (it != test_map.end()) { 

			auto v = test_sketch_set.buckets(t.info);  
			std::vector<size_t> buckets;
			std::transform(v.begin(), v.end(), std::back_inserter(buckets), [](auto &b) { return b.size(); });

			std::cout << "  candidate (SET) -> " << inet_ntoa({tuple2ip<8191>(t.value)}) << " " << *std::min_element(buckets.begin(), buckets.end()) << std::endl;
		    }
		    else
			false_positive++;
		}
		
		std::cout << "Candidates found (" << (res.size() - false_positive) << " found in the map)!" << std::endl;
		std::cout << "Reversing done in " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " usec" << std::endl;
	}	
#endif

	// Sketch<HLL>
	{
		auto idx = test_sketch.indexes([=](auto &b, auto) { 
				bool ret = (100.0 * b.cardinality() / minsum) >= perc;	
				return ret;
			   });

		std::cout << "+ reversing sketch<HLL>..." << std::endl;

		auto start = std::chrono::system_clock::now();
		auto res = pds::reverse_sketch<uint16_t, uint16_t>(test_sketch, idx); 
		auto end = std::chrono::system_clock::now();

		size_t false_positive = 0;

		for(auto & t: res) {

		    auto it = test_map.find(tuple2ip<8191>(t.value));
		    if (it != test_map.end()) { 

			auto v = test_sketch.buckets(t.info);  
			std::vector<size_t> buckets;
			std::transform(v.begin(), v.end(), std::back_inserter(buckets), [](auto &b) { return b.cardinality(); });

			std::cout << "  candidate (HLL) -> " << inet_ntoa({tuple2ip<8191>(t.value)}) << " " << *std::min_element(buckets.begin(), buckets.end()) << std::endl;
            top_candidate[tuple2ip<8191>(t.value)] = *std::min_element(buckets.begin(), buckets.end());
		    }
		    else
			false_positive++;
		}
		
		std::cout << "Candidates found (" << (res.size() - false_positive) << " found in the map)!" << std::endl;
		std::cout << "Reversing done in " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " usec" << std::endl;
	}	


    // err^2
    //
    //
    long double err = 0;

    for(auto hit : top_hitter)
    {
        auto cand = top_candidate.find(hit.first);
        if ( cand == top_candidate.end())
        {
            std::cout << "could not find hitter " << inet_ntoa({hit.first}) << " among candidates..." << std::endl; 
        }
        else
        {
            err += std::pow(hit.second - cand->second, 2)*hit.second;
        }
    }

    std::cout << "Err^2 = " << (err / total) << std::endl;

    })
    
    ;


int
main(int argc, char *argv[])
{
    return yats::run(argc,argv);
}

