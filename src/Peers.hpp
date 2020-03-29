#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>

class P2PSocket;
class Peer;
class PeersMessages;

class Peers
{
  public:
    Peers() = default;
    Peers(P2PSocket *p2pSocket);
    int count() const;
    const std::map<std::string, std::unique_ptr<Peer>> &all() const;
    std::vector<int> ip2Peers(const std::string &ip) const;
    bool accept();
    bool connect(const std::string &remotePeerAddr, uint16_t port);
    void peerIsConnected(std::unique_ptr<Peer> peer);
    void remove(Peer *peer);

    PeersMessages read(int length = 1024,
                       std::function<void(const std::unique_ptr<Peer> &)> callback = nullptr);
    int broadcast(const std::string &message,
                  std::function<void(const std::unique_ptr<Peer> &)> callback = nullptr);

  private:
    std::map<std::string, std::unique_ptr<Peer>> m_peers;
    std::map<std::string, std::vector<int>> m_ip2PeerMap;
    P2PSocket *m_p2pSocket;
    int m_count;
};
