#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <iostream>

class SocketResource
{
private:
    int m_sockFd;

public:
    SocketResource(int sockFd = -1);
    bool setNonBlockingMode();
    bool setBlockMode();
    static SocketResource create(bool debug = false);
    int resource() const;
};
