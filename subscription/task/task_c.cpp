#include "task_c.h"
#include "subscription_type.h"
#include "subscription.h"
#include <QThread>
#include "data.h"

TaskC::TaskC(int c)
    : Task("DATA_C", {{"DATA_A", "sourceA"}, {"DATA_B", "sourceB"}}), c(c)
{
}

TaskC::~TaskC()
{
}

bool TaskC::run()
{

    Data data;
    {
        Subscription::Lock<Data> lockA(*sub("sourceA"));
        Subscription::Lock<Data> lockB(*sub("sourceB"));
        //QThread::sleep(3);
        data.num = c + lockA()->num + lockB()->num;
    }

    Subscription::Lock<Data> lockC(*sub("dest"));
    lockC.swap(data);

    return true;
}
