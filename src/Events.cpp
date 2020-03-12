#include "Events.hpp"

std::shared_ptr<Event> Events::onPeerConnect()
{
    return this->on("onPeerConnect");
}

std::shared_ptr<Event> Events::onPeerDisconnect()
{
    return this->on("onPeerDisconnect");
}
