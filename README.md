# Peer-to-Peer TCP Socket Library

## Build

```sh
mkdir build && cd build
cmake ..
make
```

## Usage

```cpp
auto socket = std::make_unique<P2PSocket>(1 /*no of max peers*/);
socket->createServer("ip" /*leave blank to bind to all available addresses*/, 12345 /*port*/)
//start accepting connections
while(true)
	socket->listen();

//connect to other peers
socket->connect("ip", 12334);

//events
    sock->events()->onPeerConnect()->listen(
        [](auto &&) {
				std::cout << "A new peer connected" << std::endl;
			}
		);
```
