#ifndef SUBSCRIPTION_UTILS_H
#define SUBSCRIPTION_UTILS_H

#include <QObject>
#include <memory>
#include <functional>
#include <multitype.h>

class Model;

enum class Subscription {
    WILL_READ,
    DO_READ,
    WILL_WRITE,
    DO_WRITE
};

inline QString toString(Subscription s)
{
    switch(s)
    {
    case Subscription::WILL_READ:
        return "WILL READ";
    case Subscription::DO_READ:
        return "DO READ";
    case Subscription::WILL_WRITE:
        return "WILL WRITE";
    case Subscription::DO_WRITE:
        return "DO WRITE";
    }

    throw "!";
}

enum class State {
    READ,
    WRITE,
    VALID,
    INVALID
};

struct DataEntry {
    std::shared_ptr<MultiType> handle = nullptr;
    State state = State::INVALID;
    Model* creator;
    std::map<QObject*, Subscription> subscribers;
    int doReads = 0;
    bool doWrite = false;
};

#endif // SUBSCRIPTION_UTILS_H
