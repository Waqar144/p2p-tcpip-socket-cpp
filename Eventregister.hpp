#ifndef EVENTREGISTER_H
#define EVENTREGISTER_H

#include <string>
#include <vector>
#include <map>
#include "Event.hpp"

class Event;

class EventRegister
{
public:
    EventRegister();
    std::shared_ptr<Event> on(std::string event);
    bool has(std::string event);
    void clear(std::shared_ptr<Event> event);

private:
    std::map<std::string, std::shared_ptr<Event>> m_events;
};

#endif // EVENTREGISTER_H
