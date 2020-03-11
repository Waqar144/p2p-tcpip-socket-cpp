#include <iostream>

#include "P2PSocket.hpp"
#include "P2PSocketException.hpp"

int main(int argc, char *argv[])
{
    P2PSocket *sock;

    try {
        sock = new P2PSocket("127.0.0.1", 8870, 10);
    } catch(P2PSocketException &e) {
        std::cout << e.what() << std::endl;
    }

    sock->connect("127.0.0.1", 8889);

    std::cout << "Done, exiting...\n";
    return 0;
}
