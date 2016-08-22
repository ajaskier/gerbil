#ifndef LOCK_H
#define LOCK_H

#include "subscription.h"
#include <functional>
#include "subscription_manager.h"

template <class T1, class T2>
class Subscription::Lock
{
public:
    Lock(Subscription& sub) : sub(sub), handles(sub.leaseData()) {}
    Lock(const Lock&) = delete;
    Lock(Lock&&) = delete;
    Lock& operator=(const Lock &) = delete;
    Lock& operator=(Lock&&) = delete;
    ~Lock() {
        if (!released) release();
    }

    T1* operator()() {
        try {
            T1& data = boost::dynamic_any_cast<T1&>(*get_data());
            return &data;
        }
        catch (...) {
            return nullptr;
        }
    }

    void swap(T1& target) {
        boost::dynamic_any newany(target);
        //*data_ptr = boost::dynamic_any(target);
        std::swap(*get_data(), newany);
    }

    T2* meta() {
        try {
            T2& meta = boost::dynamic_any_cast<T2&>(*get_meta());
            return &meta;
        }
        catch (...) {
            return nullptr;
        }
    }

    void swapMeta(T2& target) {
        boost::dynamic_any newany(target);
        std::swap(*get_meta(), newany);
    }

    void release() {
        sub.returnData();
        released = true;
    }

private:

    handle get_data() { return handles.first; }
    handle get_meta() { return handles.second; }

    Subscription& sub;
    handle_pair handles;
    bool released = false;
};

#endif // LOCK_H
