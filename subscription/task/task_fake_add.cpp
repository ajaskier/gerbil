#include "task_fake_add.h"
#include "subscription.h"
#include "lock.h"

#include "data.h"
#include "QDebug"

TaskFakeAdd::TaskFakeAdd(QString destId, QString imageId)
    : Task(destId, {{imageId, "sourceImage"}})
{

}

TaskFakeAdd::TaskFakeAdd(QString destId, QString imageId, QString tempId)
    : Task(destId, {{imageId, "sourceImage"}, {tempId, "sourceTemp"}})
{

}

TaskFakeAdd::~TaskFakeAdd()
{

}

bool TaskFakeAdd::run()
{

    Data data;
    data.num = 0;

    if(subExists("sourceTemp")) {
        // no tempId
        Subscription::Lock<Data> lock(*sub("sourceTemp"));
        data.num += lock()->num;
    }

    Subscription::Lock<Data> lock(*sub("sourceImage"));
    data.num += lock()->num;
    int version = lock.version();

    Subscription::Lock<Data> destLock(*sub("dest"));
    destLock.setVersion(version);
    destLock.swap(data);

    qDebug() << "task Add finished! and the image.FAKE version is" << version;

    return true;

}
