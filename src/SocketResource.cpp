#include "SocketResource.hpp"

SocketResource::SocketResource(int sockFd) : m_sockFd(sockFd) {}

bool SocketResource::setNonBlockingMode()
{
    bool ret = true;

    const int flags = fcntl(m_sockFd, F_GETFL, 0);
    ret = 0 == fcntl(m_sockFd, F_SETFL, O_NONBLOCK);

    return ret;
}
bool SocketResource::setBlockMode()
{
    bool ret = true;

    const int flags = fcntl(m_sockFd, F_GETFL, 0);
    ret = 0 == fcntl(m_sockFd, F_SETFL, flags & (~O_NONBLOCK));

    return ret;
}
SocketResource SocketResource::create(bool debug)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        return SocketResource(-1);
    }
    return SocketResource(sockfd);
}
int SocketResource::resource() const {
    return m_sockFd;
}
