#include <iostream>

#include "P2PSocket.hpp"
#include "exceptions/P2PSocketException.hpp"
#include "exceptions/peerexception.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>

#include "peersmessages.h"

int main(int argc, char *argv[])
{
    std::vector<uint16_t> knownPorts = {
        8890,
    };

    P2PSocket *sock;

    try {
        sock = new P2PSocket(2);
        sock->createServer("127.0.0.1", 6758);
    } catch(P2PSocketException &e) {
        std::cout << e.what() << std::endl;
    }

    sock->events()->onPeerConnect()->listen([](auto &&){ std::cout << "A new peer connected"<< std::endl; });

    int total = knownPorts.size();
    int connected = 0;
    signal (SIGPIPE, SIG_IGN);
    while (true) {
        try {
            sock->listen();
            std::cout << "sock->listening\n";
        } catch (const P2PSocketException &e) {
            std::cout << e.what() << std::endl;
        }
        sleep(4);
        for (auto port : knownPorts) {
            if (connected < total) {
                try {
                    sock->connect("127.0.0.1", port);
                } catch(const P2PSocketException &e) {
                    std::cout << e.what();
                }
                ++connected;
            }
        }

        sleep(2);

        if (connected > 0) {
            try {
                int bytes = sock->peers()->broadcast("Broadcasting...\r\n");
                std::cout << "Sent: " << bytes << " msgs\n";
            } catch (const PeerException &e) {\
                std::cout << e.what();
            }
        }

        sleep(2);
        try {
            auto msgs = sock->peers()->read().all();
            for (const auto &msg : msgs) {
                std::cout << "Recieved message: " << msg.message() << "\n";
            }
        } catch (const PeerException &e) {
            std::cout << e.what();
        }
        std::cout << "iteration " << connected << "\n";
    }

    std::cout << "Done, exiting...\n";
    return 0;
}
