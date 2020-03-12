#include "SocketResource.hpp"

SocketResource::SocketResource(Socket sockFd) : m_sockFd(sockFd) {}

int SocketResource::init()
{
#ifdef _WIN32
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2,2), &wsaData);
#else
    return N0_ERROR;
#endif
}

int SocketResource::cleanUp()
{
#ifdef _WIN32
    return WSACleanup();
#else
    return N0_ERROR;
#endif
}

bool SocketResource::setNonBlockingMode()
{
    bool ret = true;

    const int flags = fcntl(m_sockFd, F_GETFL, 0);
    ret = 0 == fcntl(m_sockFd, F_SETFL, flags | O_NONBLOCK);

    return ret;
}

bool SocketResource::setBlockMode()
{
    bool ret = true;

    const int flags = fcntl(m_sockFd, F_GETFL, 0);
    ret = 0 == fcntl(m_sockFd, F_SETFL, flags & ~O_NONBLOCK);

    return ret;
}

SocketResource SocketResource::create()
{
    if (init() != N0_ERROR) {
        cleanUp();
        return SocketResource(INVALID_SOCKET);
    }

    Socket sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        return SocketResource(INVALID_SOCKET);
    }
    return SocketResource(sock);
}

int SocketResource::resource() const {
    return m_sockFd;
}
