#include "data_entry.h"

#include "subscription.h"

std::shared_ptr<boost::dynamic_any> DataEntry::read()
{
    std::unique_lock<std::mutex> lock(mu);
    not_writing.wait(lock, [this]() {
        return !doWrite;
    });
    return handle;
}

void DataEntry::endRead()
{
    //std::unique_lock<std::mutex> lock(mu);
    if(doReads == 0) not_reading.notify_one();
}

std::shared_ptr<boost::dynamic_any> DataEntry::write()
{
    std::unique_lock<std::mutex> lock(mu);
    not_reading.wait(lock, [this]() {
        return doReads == 0 && !doWrite;
    });

    return handle;
}

void DataEntry::endWrite()
{
    //std::unique_lock<std::mutex> lock(mu);
    if(!doWrite) not_writing.notify_all();
}
