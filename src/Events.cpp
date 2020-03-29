#include "Events.hpp"

const std::unique_ptr<Event> &Events::onPeerConnect()
{
    return this->on("onPeerConnect");
}

const std::unique_ptr<Event> &Events::onPeerDisconnect()
{
    return this->on("onPeerDisconnect");
}
