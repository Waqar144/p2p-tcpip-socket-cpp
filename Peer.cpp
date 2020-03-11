#include "Peer.hpp"
#include "P2PSocket.hpp"

#include <arpa/inet.h>
#include <unistd.h>

Peer::Peer(std::shared_ptr<P2PSocket> socket, SocketResource peer, int num) : m_master(socket), m_socket(peer) {
    struct sockaddr_in address;
    socklen_t len = sizeof(address);
    int ret = ::getpeername(peer.resource(), (struct sockaddr*)&address, &len);
    if (ret == -1)
        throw ("New peer connection " + std::to_string(num) + " failed.");
    m_port = ::ntohs(address.sin_port);
    m_ip = ::inet_ntoa(address.sin_addr);
    m_connected = true;
}

Peer::~Peer()
{
    ::close(m_socket.resource());
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

void Peer::send(const std::string &message)
{
    int ret = ::write(m_socket.resource(), message.c_str(), message.size());
    if (ret == -1) {
        perror("write ");
        std::cout << "Failed to write\n";
    } else if (ret == 0) {
        std::cout << "Peer has disconnected" << __func__ << "\n";
        m_connected = false;
        this->m_master->peers()->remove(this);
    }
}

std::string Peer::read(int length)
{
    char buf[length];
    int recv = ::recv(m_socket.resource(), buf, length, MSG_DONTWAIT);
    if (recv == -1) {
        perror("read ");
        std::cout << "Failed to write\n";
    }
    return buf;
}

bool Peer::status() const
{
    return m_connected;
}

void Peer::disconnect()
{
    auto shutdown = ::shutdown(m_socket.resource(), 2);
    if (shutdown == -1) {
        perror("shutdown ");
        std::cout << "failed to shutdown\n";
    }

    ::close(m_socket.resource());
    m_connected = false;
    this->m_master->peers()->remove(this);
}
