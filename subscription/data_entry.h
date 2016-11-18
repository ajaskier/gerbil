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
#include <queue>

class Model;
class Subscription;

using handle = std::shared_ptr<boost::dynamic_any>;
using handle_pair = std::pair<handle, handle/*, int&*/>;

class DataEntry {

public:

    handle data_handle = nullptr;
    handle meta_handle = nullptr;

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

    int minorVersion = 0;
    int majorVersion = 0;
    std::priority_queue<int, std::vector<int>, std::greater<int> > subscribedVersions;

    handle_pair read();
    void endRead();
    handle_pair write();
    void endWrite();

    int& getMajorVersion();
    int getMinorVersion();

private:

    std::mutex mu;
    std::condition_variable not_reading;
    std::condition_variable not_writing;

};

#endif // DATA_ENTRY_H
