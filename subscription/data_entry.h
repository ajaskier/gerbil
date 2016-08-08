#ifndef DATA_ENTRY_H
#define DATA_ENTRY_H

#include "state.h"

#include <vector>
#include <QString>
#include <mutex>
#include <condition_variable>
#include <map>
#include <boost/any.hpp>
#include <boost/dynamic_any.hpp>

class Model;
class Subscription;

class DataEntry {

public:

    std::shared_ptr<boost::dynamic_any> handle = nullptr;
    AccessState access = AccessState::NONE;
    ValidityState validity = ValidityState::INVALID;
    Model* creator = nullptr;
    std::vector<QString> dependants;
    std::map<int, Subscription*> currentSubs;

    int willReads = 0;
    bool willWrite = false;
    int doReads = 0;
    bool doWrite = false;
    bool upToDate = false;
    bool initialized = false;

    std::shared_ptr<boost::dynamic_any> read();
    void endRead();
    std::shared_ptr<boost::dynamic_any> write();
    void endWrite();

private:

    std::mutex mu;
    std::condition_variable not_reading;
    std::condition_variable not_writing;

};

#endif // DATA_ENTRY_H
