#pragma once

//#include "Commons.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <memory>

/*
* Container base class - based on mediator pattern
*/
class Container {
public:

    using SubscriptionId = int;

    template<typename Event>
    using Callback = std::function<void(const Event&)>;

    Container() : nextId(0) {}

    template<typename Event>
    SubscriptionId Subscribe(Callback<Event> cb) {
        auto& subs = GetSubscribers<Event>();
        int id = nextId++;
        subs[id] = std::move(cb);
        return id;
    }

    // Unsubscribe using ID
    template<typename Event>
    void Unsubscribe(SubscriptionId id) {
        auto& subs = GetSubscribers<Event>();
        subs.erase(id);
    }

    template<typename Event>
    void Notify(const Event& event) const {
        const auto& subs = GetSubscribers<Event>();
        for (const auto& [id, cb] : subs) {
            cb(event);
        }
    }


private:

    struct ISubscriberList {
        virtual ~ISubscriberList() = default;
    };

    template<typename Event>
    struct SubscriberList : ISubscriberList {
        std::unordered_map<SubscriptionId, Callback<Event>> callbacks;
    };

    template<typename Event>
    std::unordered_map<SubscriptionId, Callback<Event>>& GetSubscribers() const {
        auto type = std::type_index(typeid(Event));
        auto it = subscribers.find(type);
        if (it == subscribers.end()) {
            auto list = std::make_unique<SubscriberList<Event>>();
            auto* ptr = list.get();
            subscribers[type] = std::move(list);
            return ptr->callbacks;
        }
        else {
            return static_cast<SubscriberList<Event>*>(it->second.get())->callbacks;
        }
    }

    mutable std::unordered_map<std::type_index, std::unique_ptr<ISubscriberList>> subscribers;
    std::atomic<SubscriptionId> nextId;
};
