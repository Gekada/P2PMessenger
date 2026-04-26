//
// Created by L on 09.10.2025.
//

#ifndef P2PMESSENGER_CONSTANTS_H
#define P2PMESSENGER_CONSTANTS_H
namespace kademlia::constants {
    constexpr unsigned short kNodeIdSize = 128;
    // The K coefficient
    constexpr unsigned short kBucketLimit = 5;
    // a parameter
    constexpr unsigned short kParallelismLimit = 3;
    // Node id bites
    constexpr unsigned int kBucketsSize = kNodeIdSize * 8;

    constexpr uint16_t kDefaultUdpPort = 1337;

    constexpr char confPath[] = "conf/conf.json";
}
#endif //P2PMESSENGER_CONSTANTS_H
