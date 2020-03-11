#include "peersmessages.h"

PeersMessages::PeersMessages() : m_pos{0}, m_count{0}
{
}

void PeersMessages::append(const PeersReadMessage &msg)
{
    m_messages.push_back(msg);
}

std::vector<PeersReadMessage> PeersMessages::all() const
{
    return m_messages;
}

int PeersMessages::count() const
{
    return m_count;
}

void PeersMessages::rewind()
{
    m_pos = 0;
}

PeersReadMessage PeersMessages::current()
{
    return m_messages.at(m_pos);
}

int PeersMessages::key() const
{
    return m_pos;
}

void PeersMessages::next()
{
    ++m_pos;
}

bool PeersMessages::valid() const
{
    std::size_t pos = m_pos;
    return pos < m_messages.size();
}
