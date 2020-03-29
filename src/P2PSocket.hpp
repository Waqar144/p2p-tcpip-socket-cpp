#pragma once

#include "Events.hpp"
#include "Peers.hpp"
#include "SocketResource.hpp"
#include <string>

class P2PSocket
{
  public:
    P2PSocket(int maxPeers, bool debug = false);
    ~P2PSocket();
    Events *events();
    void createServer(const std::string &ip, uint16_t port);
    void connect(const std::string &remotePeerAddress, int port);
    void listen(int queue = 0);
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
