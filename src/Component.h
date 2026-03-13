#pragma once

//#include "Commons.h"
#include <functional>
#include "Container.h"

class Component {

protected:
    Component(Container* _cont) {
        _container = _cont;
    }

    Container* _container;

    template<typename Event>
    using Callback = std::function<void(const Event&)>;

    template<typename Event>
    void Subscribe(Callback<Event> cb) {
        _container->Subscribe(cb);
    }

    template<typename Event>
    void Notify(const Event& event) const {
        _container->Notify(event);
    }

public:
    virtual ~Component() {}
    virtual void draw() {}
};