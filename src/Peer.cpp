#include "Peer.hpp"
#include "P2PSocket.hpp"
#include "exceptions/peerexception.h"

#include <unistd.h>
#include <iostream>

#include <cstring>
#include <utility> 

Peer::Peer(std::shared_ptr<P2PSocket> socket, SocketResource peer, int num) : m_master(std::move(socket)), m_socket(peer)
{
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof address);
    socklen_t len = sizeof(address);
    int ret = ::getpeername(peer.resource(), (struct sockaddr*)&address, &len);
    if (ret == -1)
        std::cout<< "New peer connection " + std::to_string(num) + " failed." << std::endl;
    m_port = ::ntohs(address.sin_port);
    m_ip = ::inet_ntoa(address.sin_addr);
    m_name = m_ip + ":" + std::to_string(m_port);
    m_connected = true;
}

Peer::~Peer()
{
    m_socket.close();
}

std::string Peer::name() const {
    return m_name;
}

std::string Peer::ip () const {
    return m_ip;
}

uint16_t Peer::port() const {
    return m_port;
}

SocketResource Peer::socket() const {
    return m_socket;
}

int Peer::send(const std::string &message) const
{
#ifdef _WIN32
    /*write can be problematic on windows, using send() here */
    ::send(m_socket.resource(), message.c_str(), message.size(), 0);
#else
    int ret = ::write(m_socket.resource(), message.c_str(), message.size());
#endif
    if (ret == -1) {
        throw PeerException(std::string{"Failed to write to peer "} + m_name + " " + std::strerror(errno) + "\n");
    }

    return ret;
}

std::string Peer::read(int length)
{
    char buf[length];
    std::memset(buf, 0, sizeof buf);
    int recv = ::recv(m_socket.resource(), buf, length, MSG_DONTWAIT);
    if (recv == -1) {
        throw PeerException(std::string{"Failed to read peer "} + m_name + " " + std::strerror(errno) + "\n");
     }
    if (recv == 0) {
        //Connection is no longer valid, remote has been disconnected
        m_connected = false;
        this->m_master->peers()->remove(this);
        this->m_master->events()->onPeerDisconnect()->trigger(this);
    }
    return buf;
}

bool Peer::status() const
{
    return m_connected;
}

void Peer::disconnect()
{
    int shutdown = ::shutdown(m_socket.resource(), SHUT_RDWR);
    if (shutdown == -1) {
        throw PeerException(std::string{"Failed to shutdown socket to peer "} + m_name + " " + std::strerror(errno) + "\n");
    }

    //Don't call close here, it will be triggered automatically when object gets destroyed
    m_connected = false;
    this->m_master->peers()->remove(this);
    this->m_master->events()->onPeerDisconnect()->trigger(this);
}
