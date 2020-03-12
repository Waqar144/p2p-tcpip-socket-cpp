#pragma once

#ifdef _WIN32
    /* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0501  /* Windows XP. */
    #endif
    #include <winsock2.h>
    #include <Ws2tcpip.h>

    #define SHUT_RDWR SD_BOTH
#else
  /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>

constexpr int N0_ERROR = 0;
constexpr int INVALID_SOCKET = -1;

#endif

using Socket = decltype(socket(0, 0, 0));

class SocketResource
{
private:
    Socket m_socket;

public:
    SocketResource(Socket sockFd = -1);
    static int init();
    static int cleanUp(void);
    static int close(Socket sock);
    bool setNonBlockingMode();
    bool setBlockMode();
    static SocketResource create();
    Socket resource() const;
};
