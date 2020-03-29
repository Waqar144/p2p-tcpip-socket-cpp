#pragma once

#include "SocketResource.hpp"
#include <memory>
#include <string>

class P2PSocket;

class Peer
{
  public:
    Peer() = default;
    Peer(P2PSocket *socket, SocketResource peer, int num);
    ~Peer();
    std::string name() const;
    std::string ip() const;
    uint16_t port() const;
    SocketResource socket() const;

    int send(const std::string &message) const;
    std::string read(const int length = 1024);

    bool status() const;

    void disconnect(bool suppressException = false);

    uint8_t flags() const;
    void setFlags(uint8_t flags);

  private:
    P2PSocket *m_master;
    std::string m_name;
    std::string m_ip;
    SocketResource m_socket;
    uint16_t m_port;
    uint8_t m_flags;
    bool m_connected;
};
