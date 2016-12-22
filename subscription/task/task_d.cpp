#include "task_d.h"
#include "subscription_type.h"
#include "subscription.h"
#include <QThread>
#include "data.h"

TaskD::TaskD(int d)
    : Task("DATA_D", {{"source", {"DATA_A"}}}), d(d)
{
}

TaskD::~TaskD()
{
}

bool TaskD::run()
{

    Data data;
    {
        Subscription::Lock<Data> lockA(*sub("source"));
        data.num = d + lockA()->num + 1;
        QThread::msleep(2000);
    }

    Subscription::Lock<Data> lockD(*sub("dest"));
    lockD.swap(data);
    lockD.release();

    return true;
}
