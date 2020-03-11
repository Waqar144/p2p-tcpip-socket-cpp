#include "Event.hpp"
#include "Eventregister.hpp"
//Event::Event()
//{
//}

Event::Event(EventRegister *_register, std::string name)
    : m_eventRegister(_register), m_name{name}
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

Event Event::listen(std::function<void()> func)
{
    m_listeners.push_back(func);
    return *this;
}

int Event::trigger()
{
    if (m_listeners.empty())
        return 0;

    int count = 0;
    for (auto listener : m_listeners) {
        listener();
        ++count;
    }

    std::cout << "calling -> ";

    return count;
}
