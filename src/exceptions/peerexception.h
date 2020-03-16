#ifndef PEEREXCEPTION_H
#define PEEREXCEPTION_H

#include <string>

class PeerException
{
private:
    std::string m_description;

public:
    PeerException(std::string &&desc) : m_description{std::move(desc)} {}
    std::string what() const {return m_description;}
};


#endif // PEEREXCEPTION_H
