#include "event.hpp"
#include "spdlog/spdlog.h"
#include <cstdio>
#include <memory>
#include <optional>
#include <queue>
#include <vector>

namespace tiny_cherno {

Event::Event(EventType type) : m_type(type) {};

EventType Event::getType() { return m_type; }

EventDispatcher::EventDispatcher()
    : m_events(std::queue<std::shared_ptr<Event>>()) {}

std::optional<std::shared_ptr<Event>> EventDispatcher::nextEvent() {
    if (m_events.empty()) {
        return std::nullopt;
    }

    std::shared_ptr<Event> &e = m_events.front();
    m_events.pop();
    return e;
}

void EventDispatcher::Dispatch(std::shared_ptr<Event> e) {
    m_events.push(e);
}

void EventDispatcher::ProcessQueue() {
    while (!IsDone()) {
        std::optional<std::shared_ptr<Event>> e = nextEvent();
        if (!e.has_value())
            break;

        if (m_listeners.find(e->get()->getType()) == m_listeners.end()) {
            spdlog::info("No listeners for the event.");
            continue;
        }

        for (EventListener listener : m_listeners[e->get()->getType()]) {
            listener(*e->get());
        }
    }
}

void EventDispatcher::RegisterListener(EventType type, EventListener listener) {
    auto it = m_listeners.find(type);
    if (it == m_listeners.end()) {
        m_listeners[type] = std::vector<EventListener>();
    }
    m_listeners[type].push_back(listener);
}

void EventDispatcher::ClearQueue() {
    while (!m_events.empty()) {
        m_events.pop();
    }
}

bool EventDispatcher::IsDone() { return m_events.empty(); }

} // namespace tiny_cherno
