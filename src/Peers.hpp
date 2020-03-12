#pragma once

#include "Peer.hpp"
#include <vector>
#include <map>
#include <memory>

class P2PSocket;
class PeersMessages;

class Peers {
    public:
    Peers() = default;
    Peers(P2PSocket* p2pSocket);
    int count() const;
    std::map<std::string, std::shared_ptr<Peer>> all() const;
    std::vector<int> ip2Peers(const std::string &ip) const;
    void accept();
    bool connect(const std::string &remotePeerAddr, uint16_t port);
    void peerIsConnected(std::shared_ptr<Peer> peer);
    void remove(Peer *peer);

    PeersMessages read(int length = 1024/*, ?callable $failPeerCallback = null*/);
    int broadcast(const std::string &message/*, ?callable $failPeerCallback = null*/);

    private:
    std::shared_ptr<P2PSocket> m_p2pSocket;
    int m_count;
    std::map<std::string, std::shared_ptr<Peer>> m_peers;
    std::map<std::string, std::vector<int>> m_ip2PeerMap;
};
