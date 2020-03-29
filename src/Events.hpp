#ifndef EVENTS_HPP
#define EVENTS_HPP

#include "event/Eventregister.hpp"

class Events : public EventRegister
{
public:
    Events() = default;
    const std::unique_ptr<Event> &onPeerConnect();
    const std::unique_ptr<Event> &onPeerDisconnect();
};

#endif // EVENTS_HPP
