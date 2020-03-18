#include "Peers.hpp"
#include "Peer.hpp"
#include "exceptions/peerexception.h"
#include "P2PSocket.hpp"
#include "peersmessages.h"
#include "SocketResource.hpp"

#include <iostream>
#include <unistd.h>
#include <string>
#include <algorithm>

Peers::Peers(P2PSocket* p2pSocket)
    : m_p2pSocket(p2pSocket),
      m_count(0)
{
}

int Peers::count() const
{
    return m_count;
}

std::map<std::string, std::shared_ptr<Peer> > Peers::all() const
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
    Socket peerSocket = ::accept(m_p2pSocket->socket().resource(), (struct sockaddr*)&addr, &len);
    if (peerSocket > 0) {
        std::shared_ptr<Peer> peer = std::make_shared<Peer>(m_p2pSocket, SocketResource(peerSocket), num);
        peerIsConnected(peer);
        return true;

    }
    return false;
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

    if (::inet_pton(AF_INET, remotePeerAddr.c_str(), &addr.sin_addr) <= 0) {
        std::cout << "Peers: Invalid IPv4 host address\n";
        return false;
    }

    if (::connect(socket.resource(), (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect fail");
        std::cout << "Peer connection to " << remotePeerAddr << " on port " << port << " failed";
        return false;
    }

    std::shared_ptr<Peer> peer = std::make_shared<Peer>(m_p2pSocket, SocketResource(socket), 0);
    peerIsConnected(peer);
    return true;
}

void Peers::remove(Peer *peer)
{
    std::map<std::string, std::shared_ptr<Peer>>::iterator pos = m_peers.find(peer->name());
    if (pos != m_peers.end()) {
        m_peers.erase(pos);
        --m_count;

        std::vector<int> ipPeers = ip2Peers(peer->ip());
        ipPeers.erase(std::remove(ipPeers.begin(), ipPeers.end(), peer->port()), ipPeers.end());
        m_ip2PeerMap[peer->ip()] = ipPeers;
    }
}

PeersMessages Peers::read(int length, std::function<void(std::shared_ptr<Peer>)> callback)
{
    PeersMessages messages;
    //TODO: Move this in the appropriate place once exceptions/error handling is in place
    (void)callback;
    for (const auto& [_, peer] : m_peers) {
        (void)_;
        try {
            std::string msg = peer->read(length);
            if (msg.length() > 0)
                messages.append(PeersReadMessage(peer, msg));
        } catch(const PeerException &e) {
            if (callback != nullptr) {
                callback(peer);
                continue;
            }
            throw e;
        }

    }

    return messages;
}

int Peers::broadcast(const std::string &message, std::function<void(std::shared_ptr<Peer>)> callback)
{
    int sent = 0;
    for (const auto &[_, peer] : m_peers) {
        try {
            peer->send(message);
            ++sent;
        } catch(const PeerException &e){
            if (callback != nullptr) {
                callback(peer);
                continue;
            }
            throw e;
        }
    }
    return sent;
}

void Peers::peerIsConnected(const std::shared_ptr<Peer>& peer)
{
    m_peers[peer->name()] = peer;
    ++m_count;

    //Map multiple ports from same IP
    auto ipPeers = ip2Peers(peer->ip());
    ipPeers.push_back((peer->port()));

    std::sort(ipPeers.begin(), ipPeers.end());
    ipPeers.erase(unique(ipPeers.begin(), ipPeers.end()), ipPeers.end());

    m_ip2PeerMap[peer->ip()] = ipPeers;

    m_p2pSocket->events()->onPeerConnect()->trigger(peer.get());
}
