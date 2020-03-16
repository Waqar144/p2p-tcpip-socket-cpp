#include "Event.hpp"
#include <utility>
#include "Eventregister.hpp"
#include "../Peer.hpp"

Event::Event(EventRegister *_register, std::string name)
    : m_eventRegister(_register),
      m_name{std::move(name)}
{
}

std::string Event::name() const
{
    return m_name;
}

std::shared_ptr<EventRegister> Event::eventRegister()
{
    return m_eventRegister;
}

Event Event::listen(const std::function<void(Peer*)>& func)
{
    m_listeners.emplace_back(Callable(func));
    return *this;
}

Event Event::listen(const std::function<void ()> &func)
{
    m_listeners.emplace_back(Callable(func));
    return *this;
}
