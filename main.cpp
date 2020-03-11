#include <iostream>
#include <unistd.h>

#include "P2PSocket.hpp"
#include "P2PSocketException.hpp"

int main(int argc, char *argv[])
{
    P2PSocket *sock;

    try {
        sock = new P2PSocket("127.0.0.1", 8889, 5);
    } catch(P2PSocketException &e) {
        std::cout << e.what() << std::endl;
    }

    sock->events()->onPeerConnect()->listen([](){ std::cout << "A new peer connected"<< std::endl;; });

    while (true) {
        sock->listen();
    }

    std::cout << "Done, exiting...\n";
    return 0;
}
