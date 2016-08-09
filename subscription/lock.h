#ifndef LOCK_H
#define LOCK_H

#include "subscription.h"
#include <functional>
#include "subscription_manager.h"

template <class T>
class Subscription::Lock
{
public:
    Lock(Subscription& sub) : sub(sub), data_ptr(sub.leaseData()) {}
    Lock(const Lock&) = delete;
    Lock(Lock&&) = delete;
    Lock& operator=(const Lock &) = delete;
    Lock& operator=(Lock&&) = delete;
    ~Lock() {
        if(!released) release();
    }

    T& operator()() {
        T& data = boost::dynamic_any_cast<T&>(*data_ptr);
        return data;
    }

    void swap(T& target) {
        boost::dynamic_any newany(target);
        //*data_ptr = boost::dynamic_any(target);
        std::swap(*data_ptr, newany);
    }

    void release() {
        sub.returnData();
        released = true;
    }

private:
    Subscription& sub;
    any_sptr data_ptr;
    bool released = false;
};

#endif // LOCK_H
