#ifndef P2PMESSENGER_NODE_ENTRY_H
#define P2PMESSENGER_NODE_ENTRY_H

#include <array>
#include <QHostAddress>

struct NodeEntry {
    uint16_t port_;
    QHostAddress ip_address_;
    std::array<unsigned char, 16> node_id_;
};
#endif //P2PMESSENGER_NODE_ENTRY_H
