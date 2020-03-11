#include <iostream>
#include <unistd.h>

#include "P2PSocket.hpp"
#include "P2PSocketException.hpp"
#include "peersmessages.h"

int main(int argc, char *argv[])
{
    std::vector<uint16_t> knownPorts = {
        6758,
    };

    P2PSocket *sock;

    try {
        sock = new P2PSocket("127.0.0.1", 8890, 5);
    } catch(P2PSocketException &e) {
        std::cout << e.what() << std::endl;
    }

    sock->events()->onPeerConnect()->listen([](){ std::cout << "A new peer connected"<< std::endl;; });

    int total = knownPorts.size();
    int connected = 0;
    while (true) {
        sock->listen();
        sleep(4);
        for (auto port : knownPorts) {
            if (connected != total) {
                sock->connect("127.0.0.1", port);
                ++connected;
            }
        }

        sleep(2);

        if (connected > 0) {
            sock->peers()->broadcast("Broadcasting...");
        }

        sleep(1);

        auto msgs = sock->peers()->read().all();
        for (const auto &msg : msgs) {
            std::cout << "Message: " << msg.message() << "\n";
        }

    }

    delete sock;

    std::cout << "Done, exiting...\n";
    return 0;
}
