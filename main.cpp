#include <iostream>
#include <unistd.h>
#include <signal.h>

#include "P2PSocket.hpp"
#include "exceptions/P2PSocketException.hpp"
#include "peersmessages.h"

int main(int argc, char *argv[])
{
    std::vector<uint16_t> knownPorts = {
        6758,
    };

    std::unique_ptr<P2PSocket> sock = nullptr;

    try {
        sock = std::make_unique<P2PSocket>("127.0.0.1", 8890, 2);
    } catch(P2PSocketException &e) {
        std::cout << e.what() << std::endl;
    }

    sock->events()->onPeerConnect()->listen([](auto &&){ std::cout << "A new peer connected"<< std::endl; });

    int total = knownPorts.size();
    int connected = 0;
    signal (SIGPIPE, SIG_IGN);
    while (true) {
        sock->listen();
        std::cout << "sock->listening\n";
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
            int bytes = sock->peers()->broadcast("Broadcasting...\r\n");
            std::cout << "Sent: " << bytes << " msgs\n";
        }

        sleep(2);

        auto msgs = sock->peers()->read().all();
        for (const auto &msg : msgs) {
            std::cout << "Message: " << msg.message() << "\n";
        }
        std::cout << "iteration " << connected << "\n";
    }

    std::cout << "Done, exiting...\n";
    return 0;
}
