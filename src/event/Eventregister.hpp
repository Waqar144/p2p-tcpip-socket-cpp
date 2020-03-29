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
    EventRegister() = default;
    const std::unique_ptr<Event> &on(std::string event);
    bool has(std::string event);
    void clear(const std::unique_ptr<Event> &event);

  private:
    std::map<std::string, std::unique_ptr<Event>> m_events;
};

#endif // EVENTREGISTER_H
