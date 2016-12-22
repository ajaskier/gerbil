#include "task_fake_sub.h"
#include "subscription.h"
#include "lock.h"

#include "data.h"
#include "QDebug"

TaskFakeSub::TaskFakeSub(QString sourceId)
    : Task("taskSub", "dist.tmp.FAKE", {{"source", sourceId}})
{
}

TaskFakeSub::~TaskFakeSub()
{

}

bool TaskFakeSub::run()
{

    Data data;
    data.num = 0;

    Subscription::Lock<Data> lock(*sub("source"));
    data.num += lock()->num - 10;
    int version = lock.version();

    Subscription::Lock<Data> destLock(*sub("dest"));
    destLock.setVersion(version);
    destLock.swap(data);

    qDebug() << "task Sub finished! and the image.FAKE version is" << version;

    return true;


}
