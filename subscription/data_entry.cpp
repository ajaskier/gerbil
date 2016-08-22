#include "data_entry.h"

#include "subscription.h"

handle_pair DataEntry::read()
{
    std::unique_lock<std::mutex> lock(mu);
    not_writing.wait(lock, [this]() {
        return !doWrite;
    });
    return handle_pair(data_handle, meta_handle);
}

void DataEntry::endRead()
{
    //std::unique_lock<std::mutex> lock(mu);
    if (doReads == 0) not_reading.notify_one();
}

handle_pair DataEntry::write()
{
    std::unique_lock<std::mutex> lock(mu);
    not_reading.wait(lock, [this]() {
        return doReads == 0 && !doWrite;
    });

    return handle_pair(data_handle, meta_handle);
}

void DataEntry::endWrite()
{
    //std::unique_lock<std::mutex> lock(mu);
    if (!doWrite) not_writing.notify_all();
}
