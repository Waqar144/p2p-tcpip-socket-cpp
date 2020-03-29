#include "SocketResource.hpp"

SocketResource::SocketResource(Socket sockFd) : m_socket(sockFd)
{
}

int SocketResource::init()
{
#ifdef _WIN32
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
    return NO_ERROR;
#endif
}

int SocketResource::cleanUp()
{
#ifdef _WIN32
    return WSACleanup();
#else
    return NO_ERROR;
#endif
}

bool SocketResource::close()
{
    if (m_socket == INVALID_SOCKET)
        return false;

#ifdef _WIN32
    int status = closesocket(m_socket);
#else
    int status = ::close(m_socket);
#endif

    return status != SOCKET_ERROR;
}

bool SocketResource::setNonBlockingMode()
{
#ifdef _WIN32
    u_long iMode = 1;
    const bool ret = N0_ERROR == ioctlsocket(m_socket, FIONBIO, &iMode);
#else
    const int flags = fcntl(m_socket, F_GETFL, 0);
    const bool ret = NO_ERROR == fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);

#endif
    return ret;
}

bool SocketResource::setBlockMode()
{
#ifdef _WIN32
    u_long iMode = 0;
    const bool ret = N0_ERROR == ioctlsocket(m_socket, FIONBIO, &iMode);
#else
    const int flags = fcntl(m_socket, F_GETFL, 0);
    const bool ret = NO_ERROR == fcntl(m_socket, F_SETFL, flags & ~O_NONBLOCK);
#endif

    return ret;
}

bool SocketResource::SetSocketNoDelay()
{
    int set = 1;
    int rc = setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&set, sizeof(int));
    return rc == 0;
}

SocketResource SocketResource::create()
{
    if (init() != NO_ERROR) {
        cleanUp();
        return SocketResource(INVALID_SOCKET);
    }

    Socket sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        return SocketResource(INVALID_SOCKET);
    }
    SocketResource rsc(sock);
    rsc.SetSocketNoDelay();
    return rsc;
}

Socket SocketResource::resource() const
{
    return m_socket;
}
