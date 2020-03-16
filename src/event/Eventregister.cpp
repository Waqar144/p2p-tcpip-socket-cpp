#include "Eventregister.hpp"

void toLower(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c){ return std::tolower(c); });
}

std::shared_ptr<Event> EventRegister::on(std::string event)
{
    toLower(event);
    if (m_events.find(event) != m_events.end()) {
        return m_events[event];
    }

    std::shared_ptr<Event> ev = std::make_shared<Event>(this, event);
    m_events[event] = ev;
    return ev;
}

bool EventRegister::has(std::string event)
{
    toLower(event);
    return m_events.find(event) != m_events.end();
}

void EventRegister::clear(const std::shared_ptr<Event>& event)
{
    auto it = m_events.begin();
    while (it != m_events.end()) {
        if (it->second == event)
            it = m_events.erase(it);
        else
            ++it;
    }
}
