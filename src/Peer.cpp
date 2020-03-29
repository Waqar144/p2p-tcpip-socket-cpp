#include "Peer.hpp"
#include "P2PSocket.hpp"
#include "exceptions/peerexception.h"

#include <iostream>
#include <unistd.h>

#include <cstring>
#include <utility>

Peer::Peer(P2PSocket *socket, SocketResource peer, int num)
    : m_master(socket), m_socket(peer), m_flags{0}
{
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof address);
    socklen_t len = sizeof(address);
    int ret = ::getpeername(peer.resource(), (struct sockaddr *)&address, &len);
    if (ret == -1)
        std::cout << "New peer connection " + std::to_string(num) + " failed." << std::endl;
    m_port = ntohs(address.sin_port);
    m_ip = ::inet_ntoa(address.sin_addr);
    m_name = m_ip + ":" + std::to_string(m_port);
    m_connected = true;
}

Peer::~Peer()
{
    m_socket.close();
}

std::string Peer::name() const
{
    return m_name;
}

std::string Peer::ip() const
{
    return m_ip;
}

uint16_t Peer::port() const
{
    return m_port;
}

SocketResource Peer::socket() const
{
    return m_socket;
}

int Peer::send(const std::string &message) const
{
#ifdef _WIN32
    /*write can be problematic on windows, using send() here */
    int ret = ::send(m_socket.resource(), message.c_str(), message.size(), 0);
#else
    int ret = ::write(m_socket.resource(), message.c_str(), message.size());
#endif
    if (ret == -1) {
        throw PeerException(std::string{"Failed to write to peer "} + m_name + " " +
                            std::strerror(errno) + "\n");
    }

    return ret;
}
std::string Peer::read(const int length)
{
    std::vector<char> buf(length);
    int recv = ::recv(m_socket.resource(), &buf[0], length, MSG_DONTWAIT);
    if (recv == -1) {
        throw PeerException(std::string{"Failed to read peer "} + m_name + " " +
                            std::strerror(errno) + "\n");
    }
    if (recv == 0) {
        // Connection is no longer valid, remote has been disconnected
        m_connected = false;
        this->m_master->events()->onPeerDisconnect()->trigger(this);
        this->m_master->peers()->remove(this);
        return std::string{};
    }
    buf.resize(recv);
    return std::string{buf.data()};
}

bool Peer::status() const
{
    return m_connected;
}

void Peer::disconnect(bool suppressException)
{
    int shutdown = ::shutdown(m_socket.resource(), SHUT_RDWR);
    if (shutdown == -1 && !suppressException) {
        throw PeerException(std::string{"Failed to shutdown socket to peer "} + m_name + " " +
                            std::strerror(errno) + "\n");
    }

    // Don't call close here, it will be triggered automatically when object gets destroyed
    m_connected = false;
    this->m_master->events()->onPeerDisconnect()->trigger(this);
    this->m_master->peers()->remove(this);
}

uint8_t Peer::flags() const
{
    return m_flags;
}

void Peer::setFlags(uint8_t flags)
{
    m_flags = flags;
}
