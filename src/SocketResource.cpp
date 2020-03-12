#include "SocketResource.hpp"

SocketResource::SocketResource(Socket sockFd) : m_socket(sockFd) {}

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
#ifdef _WIN32
    u_long iMode = 1;
    const bool ret = N0_ERROR == ioctlsocket(m_socket, FIONBIO, &iMode);
#endif
    const int flags = fcntl(m_socket, F_GETFL, 0);
    const bool ret = N0_ERROR == fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);

    return ret;
}

bool SocketResource::setBlockMode()
{
#ifdef _WIN32
    u_long iMode = 0;
    const bool ret = N0_ERROR == ioctlsocket(m_socket, FIONBIO, &iMode);
#endif

    const int flags = fcntl(m_socket, F_GETFL, 0);
    const bool ret = N0_ERROR == fcntl(m_socket, F_SETFL, flags & ~O_NONBLOCK);

    return ret;
}

SocketResource SocketResource::create()
{
    if (init() != N0_ERROR) {
        cleanUp();
        return SocketResource(INVALID_SOCKET);
    }

    Socket sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        return SocketResource(INVALID_SOCKET);
    }
    return SocketResource(sock);
}

int SocketResource::resource() const {
    return m_socket;
}
