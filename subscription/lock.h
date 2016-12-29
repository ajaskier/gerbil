#ifndef LOCK_H
#define LOCK_H

#include <functional>
#include "subscription.h"
#include "subscription_manager.h"

template <class T1, class T2>
class Subscription::Lock
{
public:
    Lock(Subscription& sub) : sub(sub)/*, handles(sub.leaseData())*/ {}
    Lock(const Lock&) = delete;
    Lock(Lock&&) = delete;
    Lock& operator=(const Lock &) = delete;
    Lock& operator=(Lock&&) = delete;
    ~Lock() {
        if (!released) release();
    }

    T1* operator()() {

        if (!initialized) initialize();

        try {
            T1& data = boost::dynamic_any_cast<T1&>(*get_data());
            return &data;
        }
        catch (...) {
            return nullptr;
        }
    }

    void swap(T1& target) {

        if (!initialized) initialize();

        boost::dynamic_any newany(target);
        //*data_ptr = boost::dynamic_any(target);
        std::swap(*get_data(), newany);
    }

    T2* meta() {

        if (!initialized) initialize();

        try {
            T2& meta = boost::dynamic_any_cast<T2&>(*get_meta());
            return &meta;
        }
        catch (...) {
            return nullptr;
        }
    }

    void swapMeta(T2& target) {

        if (!initialized) initialize();

        boost::dynamic_any newany(target);
        std::swap(*get_meta(), newany);
    }

    int version() {
        return get_version();
    }

    void setVersion(int newVersion) {
        //get_version() = newVersion;
        sub.setVersion(newVersion);
    }

    void release() {
        if (initialized) sub.returnData();
       // sub.returnData();
        released = true;
    }

    void initialize() {
        handles = sub.leaseData();
        initialized = true;
    }

private:

    handle get_data() { return std::get<0>(handles); }
    handle get_meta() { return std::get<1>(handles); }
    int get_version() { return sub.version(); }

    Subscription& sub;
    handle_pair handles;

    bool initialized = false;
    bool released = false;
};

#endif // LOCK_H
