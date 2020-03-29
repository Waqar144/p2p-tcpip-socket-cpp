#include "Peers.hpp"
#include "P2PSocket.hpp"
#include "Peer.hpp"
#include "SocketResource.hpp"
#include "exceptions/peerexception.h"
#include "peersmessages.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <unistd.h>

Peers::Peers(P2PSocket *p2pSocket) : m_p2pSocket(p2pSocket), m_count(0)
{
}

int Peers::count() const
{
    return m_count;
}

const std::map<std::string, std::unique_ptr<Peer>> &Peers::all() const
{
    return m_peers;
}

std::vector<int> Peers::ip2Peers(const std::string &ip) const
{
    auto retItr = m_ip2PeerMap.find(ip);
    if (retItr != m_ip2PeerMap.end())
        return m_ip2PeerMap.at(ip);
    return {};
}

bool Peers::accept()
{
    int num = m_count + 1;
    sockaddr_in addr;
#ifdef _WIN32
    int len = sizeof(addr);
#else
    socklen_t len = sizeof(addr);
#endif
    Socket peerSocket = ::accept(m_p2pSocket->socket().resource(), (struct sockaddr *)&addr, &len);
    if (peerSocket == INVALID_SOCKET)
        return false;
    std::unique_ptr<Peer> peer =
        std::make_unique<Peer>(m_p2pSocket, SocketResource(peerSocket), num);
    peerIsConnected(std::move(peer));
    return true;
}

bool Peers::connect(const std::string &remotePeerAddr, uint16_t port)
{
    if (port < 0x3E8 || port > 0xFFFF) {
        std::cout << "Invalid remote peer port\n";
        return false;
    }
    auto socket = SocketResource::create();

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
#ifdef _WIN32
    auto ipAddr = ::inet_addr(remotePeerAddr.c_str());
    if (ipAddr == INADDR_NONE)
        return false;
    if (ipAddr == INADDR_ANY)
        return false;
    address.sin_addr.s_addr = ipAddr;
#else
    if (::inet_pton(AF_INET, remotePeerAddr.c_str(), &addr.sin_addr) <= 0) {
        std::cout << "Peers: Invalid IPv4 host address\n";
        return false;
    }
#endif
    if (::connect(socket.resource(), (struct sockaddr *)&addr, sizeof(addr)) != NO_ERROR) {
        std::cout << "Peer connection to " << remotePeerAddr << " on port " << port << " failed";
        return false;
    }

    std::unique_ptr<Peer> peer = std::make_unique<Peer>(m_p2pSocket, SocketResource(socket), 0);
    peerIsConnected(std::move(peer));
    return true;
}

void Peers::remove(Peer *peer)
{
    std::map<std::string, std::unique_ptr<Peer>>::iterator pos = m_peers.find(peer->name());
    std::string peerIp = peer->ip();
    int port = peer->port();
    if (pos != m_peers.end()) {
        m_peers.erase(pos);
        --m_count;
    }
    std::vector<int> ipPeers = ip2Peers(peerIp);
    ipPeers.erase(std::remove(ipPeers.begin(), ipPeers.end(), port), ipPeers.end());
    m_ip2PeerMap[peerIp] = ipPeers;
}

PeersMessages Peers::read(int length, std::function<void(const std::unique_ptr<Peer> &)> callback)
{
    PeersMessages messages;
    for (const auto &[_, peer] : m_peers) {
        (void)_;
        try {
            std::string msg = peer->read(length);
            if (!msg.empty())
                messages.append(PeersReadMessage(peer.get(), msg));
        } catch (const PeerException &e) {
            if (peer != nullptr && callback != nullptr) {
                callback(peer);
                continue;
            }
            throw e;
        }
        if (m_peers.empty())
            break;
    }

    return messages;
}

int Peers::broadcast(const std::string &message,
                     std::function<void(const std::unique_ptr<Peer> &)> callback)
{
    int sent = 0;
    for (const auto &[_, peer] : m_peers) {
        try {
            peer->send(message);
            ++sent;
        } catch (const PeerException &e) {
            if (callback != nullptr) {
                callback(peer);
                continue;
            }
            throw e;
        }
    }
    return sent;
}

void Peers::peerIsConnected(std::unique_ptr<Peer> peer)
{
    // Map multiple ports from same IP
    auto ipPeers = ip2Peers(peer->ip());
    ipPeers.push_back((peer->port()));

    std::sort(ipPeers.begin(), ipPeers.end());
    ipPeers.erase(unique(ipPeers.begin(), ipPeers.end()), ipPeers.end());
    m_ip2PeerMap[peer->ip()] = ipPeers;
    m_p2pSocket->events()->onPeerConnect()->trigger(peer.get());

    m_peers[peer->name()] = std::move(peer);
    ++m_count;
}
