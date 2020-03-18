#include "P2PSocket.hpp"
#include "exceptions/P2PSocketException.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>


P2PSocket::P2PSocket(int maxPeers, bool debug) : m_debug(debug), m_maxPeers(maxPeers)
{
    if (maxPeers < 0x01 || maxPeers > 0xFF) {
        throw P2PSocketException("Max peers argument must be a valid single byte unsigned integer");
    }

    m_peers = Peers(this);
}

P2PSocket::~P2PSocket()
{
    SocketResource::close(m_socket.resource());
    SocketResource::cleanUp();
}

Events* P2PSocket::events()
{
    return &m_events;
}

void P2PSocket::createServer(const std::string& ip, uint16_t port)
{
    if (m_socket.resource() != -1) {
        throw P2PSocketException("Socket server was already created\n");
    }

    // validate port
    if (port < 0x3E8 || port > 0xFFFF) {
        throw P2PSocketException("Invalid port\n");
    }

    sockaddr_in address;
    std::memset(&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    // save socket resource
    m_socket = SocketResource::create();
    if (m_socket.resource() == -1) {
        throw P2PSocketException(std::string{"Failed to create socket\n"} + std::strerror(errno));
    }

    // validate ip
    if (ip == "") {
        address.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (::inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0)
            throw P2PSocketException(
                std::string{"Invalid IPv4 host address\n"} + std::strerror(errno));
    }

    if (::bind(m_socket.resource(), (struct sockaddr*)&address, sizeof(address)) == -1) {
        throw P2PSocketException(std::string{"Failed to bind\n"} + std::strerror(errno));
    }

    if (::listen(m_socket.resource(), m_maxPeers) < 0) {
        throw P2PSocketException(std::string{"Failed to start listener"} + std::strerror(errno));
    }
}

void P2PSocket::connect(const std::string& remotePeerAddress, int port)
{
    if (!this->m_peers.connect(remotePeerAddress, port)) {
        throw P2PSocketException(
            std::string{"Failed to connect to: " + remotePeerAddress + " " + std::to_string(port) +
                        " "} +
            std::strerror(errno));
    }
}

void P2PSocket::listen()
{
    m_socket.setNonBlockingMode();
    int remain = m_maxPeers - m_peers.count();
    if (remain > 0) {
        for (int i = 0; i <= remain; ++i) {
            if (!m_peers.accept())
                throw P2PSocketException(std::string{"Accept failure "} + std::strerror(errno));
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

Peers* P2PSocket::peers()
{
    return &m_peers;
}
