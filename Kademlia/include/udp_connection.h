//
// Created by L on 04.08.2025.
//

#ifndef P2PMESSENGER_UDP_CONNECTION_H
#define P2PMESSENGER_UDP_CONNECTION_H

#include <QUdpSocket>
#include <QObject>

namespace kademlia {

    class UdpConnection : public QObject {
    private:
        QUdpSocket udp_socket;
    };

}

#endif //P2PMESSENGER_UDP_CONNECTION_H
