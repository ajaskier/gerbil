#include "task_image_fake.h"
#include "subscription_type.h"
#include "subscription.h"
#include <QThread>
#include "data.h"
#include <memory>
#include "lock.h"
#include <QDebug>

TaskImageFAKE::TaskImageFAKE(int a)
    : Task("image.FAKE", {{"ROI", "source"}}), a(a)
{
}

TaskImageFAKE::~TaskImageFAKE()
{
}

bool TaskImageFAKE::run()
{
    QThread::msleep(2000);

    Data data;
    data.num = a;

    Subscription::Lock<Data> source_lock(*sub("source"));
    int sourceVersion = source_lock.version();
    data.num += source_lock()->num;

    Subscription::Lock<Data> lock(*sub("dest"));
    lock.setVersion(sourceVersion);
    lock.swap(data);

    //qDebug() << "swapped";
    qDebug() << "task image.FAKE finished! with version" << sourceVersion;

    return true;
}
