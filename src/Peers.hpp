#pragma once

#include <vector>
#include <map>
#include <memory>
#include <functional>

class P2PSocket;
class Peer;
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
    void peerIsConnected(const std::shared_ptr<Peer>& peer);
    void remove(Peer *peer);

    PeersMessages read(int length = 1024, std::function<void(std::shared_ptr<Peer>)> callback = nullptr);
    int broadcast(const std::string &message, std::function<void(std::shared_ptr<Peer>)> callback = nullptr);

    private:
    std::shared_ptr<P2PSocket> m_p2pSocket;
    std::map<std::string, std::shared_ptr<Peer>> m_peers;
    std::map<std::string, std::vector<int>> m_ip2PeerMap;
    int m_count;
};
