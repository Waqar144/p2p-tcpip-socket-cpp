#ifndef EVENT_H
#define EVENT_H

#include <vector>
#include <string>
#include <functional>

//#include "Eventregister.hpp"
#include "Peer.hpp"

class EventRegister;

class Event
{
public:
    Event(EventRegister *_register, std::string name);
    std::string name() const;
    std::shared_ptr<EventRegister> eventRegister();
    Event listen(const std::function<void()>& func);
    int trigger();
private:
    std::shared_ptr<EventRegister> m_eventRegister;
    std::string m_name;
    std::vector<std::function<void()>> m_listeners;
};

#endif // EVENT_H
