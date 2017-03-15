#include "Event.hpp"
#include "Reactor.hpp"

#include <assert.h>

Reactor::Reactor()    
    : currentThreadId_(std::this_thread::get_id())
{    
}

Reactor::~Reactor()
{    
}

void Reactor::run()
{
    assert(std::this_thread::get_id() == currentThreadId_);

    auto readyEvents = epoll_.pollEvents(1000);
    for (auto event: readyEvents)
    {
        event->handleEvent();
    }
}

void Reactor::insertEvent(EventPtr event)
{
    assert(std::this_thread::get_id() == currentThreadId_);
    
    int fd = event->fd();
    if (events_.find(fd) == events_.end())
    {
        events_[fd] = event;
        epoll_.addEvent(fd, event->interestedEvents(), event.get());
    }
    else
    {
        if (invalidFds_.find(fd) == invalidFds_.end())
        {
            invalidFds_.erase(fd);
            epoll_.addEvent(fd, event->interestedEvents(), event.get());
        }
        else
        {
            if (event->isActive()
            {
                epoll_.updateEvent(fd, event->interestedEvents(), event.get());
            }
            else
            {
                epoll_.deleteEvent(fd);
                invalidFds_.insert(fd);
            }
        }
    }
}
 
void Reactor::deleteEvent(EventPtr event)
{
    assert(std::this_thread::get_id() == currentThreadId_);
    
    int fd = event->fd();
    assert(events_.find(fd) != events_.end());

    events_.erase(fd);
    if (invalidFds_.find(fd) != invalidFds_.end())
    {
        invalidFds_.erase(fd);
    }
    else
    {
        epoll_.deleteEvent(fd);
    }
}
