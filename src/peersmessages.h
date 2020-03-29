#ifndef PEERSMESSAGES_H
#define PEERSMESSAGES_H

#include <memory>
#include <string>
#include <vector>

class Peer;

class PeersReadMessage
{
  public:
    PeersReadMessage() = default;
    PeersReadMessage(Peer *peer, std::string message) : m_peer(peer), m_message(message)
    {
    }
    std::string message() const
    {
        return m_message;
    }
    Peer *peer() const
    {
        return m_peer;
    }

  private:
    Peer *m_peer;
    std::string m_message;
};

class PeersMessages
{
  public:
    PeersMessages();
    void append(const PeersReadMessage &msg);
    std::vector<PeersReadMessage> all() const;
    int count() const;
    void rewind();
    PeersReadMessage current();
    int key() const;
    void next();
    bool valid() const;

  private:
    int m_pos;
    int m_count;
    std::vector<PeersReadMessage> m_messages;
};

#endif // PEERSMESSAGES_H
