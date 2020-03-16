#ifndef EVENT_H
#define EVENT_H

#include <vector>
#include <string>
#include <memory>
#include <functional>

class Peer;
class EventRegister;

struct Callable {
//    template<typename F>
//    explicit Callable(F f) : zero_(f), one_(std::move(f)) {}
    Callable(std::function<void()> f) : zero_(f) {}
    Callable(std::function<void(Peer*)> f) : one_(f) {}

    void operator()() { zero_(); }
    void operator()(Peer* peer) { one_(peer); }

    std::function<void()> zero_;
    std::function<void(Peer*)> one_;
};

class Event
{
public:
    Event(EventRegister *_register, std::string name);
    std::string name() const;
    std::shared_ptr<EventRegister> eventRegister();
    Event listen(const std::function<void(Peer*)>& func);
    Event listen(const std::function<void()>& func);

    template <typename ...Args>
    int trigger(Args ...args)
    {
        if (m_listeners.empty())
            return 0;

        int count = 0;
        for (auto& listener : m_listeners) {
            listener(args...);
            ++count;
        }

        return count;
    }
private:
    std::shared_ptr<EventRegister> m_eventRegister;
    std::string m_name;
    std::vector<Callable> m_listeners;
//    std::vector<std::function<void(std::shared_ptr<Peer>)> > m_listeners;
};

#endif // EVENT_H
