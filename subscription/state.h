#ifndef STATE_H
#define STATE_H

#include <QString>

enum class AccessState {
    READ,
    WRITE,
    NONE
};

enum class ValidityState {
    VALID,
    INVALID
};

inline QString toString(AccessState s)
{
    switch(s)
    {
    case AccessState::READ:
        return "READ";
    case AccessState::WRITE:
        return "WRITE";
    case  AccessState::NONE:
        return "NONE";
    }

    throw "!";
}

inline QString toString(ValidityState s)
{
    switch(s)
    {
    case ValidityState::VALID:
        return "VALID";
    case ValidityState::INVALID:
        return "INVALID";
    }

    throw "!";
}

#endif // STATE_H
