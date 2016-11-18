#include "data_entry.h"

#include "subscription.h"

handle_pair DataEntry::read()
{
    std::unique_lock<std::mutex> lock(mu);
    not_writing.wait(lock, [this]() {
        return !doWrite && initialized;
    });
    return handle_pair(data_handle, meta_handle/*, externalVersion*/);
}

void DataEntry::endRead()
{
    if (doReads == 0) not_reading.notify_one();
}

int& DataEntry::getMajorVersion()
{
    return majorVersion;
}

int DataEntry::getMinorVersion()
{
    return minorVersion;
}

handle_pair DataEntry::write()
{
    std::unique_lock<std::mutex> lock(mu);
    not_reading.wait(lock, [this]() {
        return doReads == 0 && !doWrite;
    });

    return handle_pair(data_handle, meta_handle/*, externalVersion*/);
}

void DataEntry::endWrite()
{
    //std::unique_lock<std::mutex> lock(mu);
    if (!doWrite) not_writing.notify_all();
}
