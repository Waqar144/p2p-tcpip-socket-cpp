#include "Peers.hpp"
#include "P2PSocket.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <string.h>
#include <set>
#include <algorithm>
#include "peersmessages.h"

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

std::vector<int> Peers::ip2Peers(std::string ip) const
{
    auto retItr = m_ip2PeerMap.find(ip);
    if (retItr != m_ip2PeerMap.end())
        return m_ip2PeerMap.at(ip);
    return {};
}

void Peers::accept()
{
    int num = m_count + 1;
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int peerSocket = ::accept(m_p2pSocket->socket().resource(), (struct sockaddr*)&addr, &len);
    if (peerSocket > 0) {
        std::shared_ptr<Peer> peer = std::make_shared<Peer>(m_p2pSocket, SocketResource(peerSocket), num);
        peerIsConnected(peer);
    }
}

void Peers::connect(std::string remotePeerAddr, int port)
{
    if (port < 0x3E8 || port > 0xFFFF) {
        std::cout << "Invalid remote peer port\n";
    }
    auto socket = SocketResource::create();

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (::inet_pton(AF_INET, remotePeerAddr.c_str(), &addr.sin_addr) <= 0) {
        std::cout << "Peers: Invalid IPv4 host address\n";
    }

    if (::connect(socket.resource(), (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect fail");
        std::cout << "Peer connection to " << remotePeerAddr << " on port " << port << " failed";
    }

    std::shared_ptr<Peer> peer = std::make_shared<Peer>(m_p2pSocket, SocketResource(socket), 0);
    peerIsConnected(peer);
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

PeersMessages Peers::read(int length/*, ?callable $failPeerCallback = null*/)
{
    PeersMessages messages;
    for (const auto& [_, peer] : m_peers) {
        (void)_;
        std::string msg = peer->read(length);
        if (msg.length() > 0)
            messages.append(PeersReadMessage(peer, msg));

    }

    return messages;
}

int Peers::broadcast(std::string message/*, ?callable $failPeerCallback = null*/)
{
    int sent = 0;
    for (const auto &[_, peer] : m_peers) {
        peer->send(message);
        ++sent;
    }
    return sent;
}

void Peers::peerIsConnected(std::shared_ptr<Peer> peer)
{
    m_peers[peer->name()] = peer;
    ++m_count;

    //Map multiple ports from same IP
    auto ipPeers = ip2Peers(peer->ip());
    ipPeers.push_back((peer->port()));

    std::sort(ipPeers.begin(), ipPeers.end());
    ipPeers.erase(unique(ipPeers.begin(), ipPeers.end()), ipPeers.end());

    m_ip2PeerMap[peer->ip()] = ipPeers;

    std::cout << "Peer added to list " << m_count << "\n";

    m_p2pSocket->events()->onPeerConnect()->trigger();
}
