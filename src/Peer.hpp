#pragma once

#include "SocketResource.hpp"
#include <string>
#include <memory>

class P2PSocket;

class Peer {
public:
    Peer() = default;
    Peer(std::shared_ptr<P2PSocket> socket, SocketResource peer, int num);
    ~Peer();
    std::string name() const;
    std::string ip () const;
    uint16_t port() const;
    SocketResource socket() const;

    int send(const std::string &message) const;
    std::string read(int length = 1024);

    bool status() const;

    void disconnect();

private:
    std::shared_ptr<P2PSocket> m_master;
    std::string m_ip;
    uint16_t m_port;
    std::string m_name;
    SocketResource m_socket;
    bool m_connected;
};
