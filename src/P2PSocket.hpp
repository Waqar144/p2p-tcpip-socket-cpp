#pragma once

#include <string>
#include "SocketResource.hpp"
#include "Peers.hpp"
#include "Events.hpp"

class P2PSocket {

public:
    P2PSocket(const std::string& ip, int port, int maxPeers, bool debug = false);
    ~P2PSocket();
    Events *events();
    void connect(const std::string& remotePeerAddress, int port);
    void listen();
    SocketResource socket() const;
    int maxPeers() const;
    bool m_debug;

    Peers *peers();

private:
    SocketResource m_socket;
    Peers m_peers;
    Events m_events;
    int m_maxPeers;
};
