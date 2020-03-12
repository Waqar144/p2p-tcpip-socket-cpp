#include "P2PSocket.hpp"
#include "P2PSocketException.hpp"

#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>


P2PSocket::P2PSocket(std::string ip, int port, int maxPeers, bool debug)
    : m_debug(debug),
      m_maxPeers(maxPeers)
{
    if (maxPeers < 0x01 || maxPeers > 0xFF) {
        throw P2PSocketException("Max peers argument must be a valid single byte unsigned integer");
    }

    //validate port
    if (port < 0x3E8 || port > 0xFFFF) {
        throw P2PSocketException("Invalid port\n");
    }

    //save socket resource
    m_socket = SocketResource::create();
    if (m_socket.resource() == -1) {
        throw P2PSocketException("Failed to create socket\n");
    }

    //validate ip
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    if (::inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0) {
        throw P2PSocketException("Invalid IPv4 host address\n");
    }
    address.sin_port = htons(port); 

    if (::bind(m_socket.resource(), (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("Bind failed");
        throw P2PSocketException("Failed to bind\n");
    }

    if (::listen(m_socket.resource(), m_maxPeers) < 0) {
        perror("listen failure");
        throw P2PSocketException("Failed to start listener");
    }

    m_peers = Peers(this);
}

P2PSocket::~P2PSocket()
{
    ::close(m_socket.resource());
}

Events *P2PSocket::events()
{
    return &m_events;
}

void P2PSocket::connect(std::string remotePeerAddress, int port)
{
    this->m_peers.connect(remotePeerAddress, port);
}

void P2PSocket::listen()
{
    m_socket.setNonBlockingMode();
    int remain = m_maxPeers - m_peers.count();
    if (remain > 0) {
        for (int i = 0; i <= remain; ++i) {
            m_peers.accept();
        }
    }
    m_socket.setBlockMode();
}

SocketResource P2PSocket::socket() const
{
    return m_socket;
}

int P2PSocket::maxPeers() const
{
    return m_maxPeers;
}

Peers *P2PSocket::peers()
{
    return &m_peers;
}
