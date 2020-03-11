#ifndef EVENTS_HPP
#define EVENTS_HPP

#include "Eventregister.hpp"

class Events : public EventRegister
{
public:
    Events() = default;
    std::shared_ptr<Event> onPeerConnect();
    std::shared_ptr<Event> onPeerDisconnect();
};

#endif // EVENTS_HPP
