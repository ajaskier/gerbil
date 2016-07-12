#ifndef MULTI_TYPE_H
#define MULTI_TYPE_H

#include <QObject>

enum class DataId {
    DATA_A,
    DATA_B
};

inline QString toString(DataId d) {

    switch(d)
    {
    case DataId::DATA_A:
        return "DATA_A";
    case DataId::DATA_B:
        return "DATA_B";
    }
}

//dummiest possible example of struct
struct Data {
    int num;
};


class MultiType
{
public:
    MultiType(const DataId& id) : id(id) {}

private:
    DataId id;
};

template <class T>
class SpecificType : public MultiType
{
public:
    SpecificType(const DataId& id, const T& data)
        : MultiType(id) {}

    T& operator()() {
        return data;
    }

private:
    T data;
};



#endif // MULTI_TYPE_H
