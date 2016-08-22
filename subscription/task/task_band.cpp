#include "task_band.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "task_band2qimage_tbb.h"

#include <QImage>

TaskBand::TaskBand(QString sourceId, QString destId, size_t dim,
                   representation::t repr)
    : Task(destId, {{sourceId, "source"}}), dim(dim), repr(repr)
{
}

TaskBand::~TaskBand()
{
}

bool TaskBand::run()
{
    {
        Subscription::Lock<multi_img> source_lock(*sub("source"));
        multi_img* source = source_lock();

        int size = source->size();
        if (dim >= size)
            dim = 0;

    }

    auto sourceId = sub("source")->getDependency().dataId;
    auto destId = sub("dest")->getDependency().dataId;

    TaskBand2QImageTbb taskConvert(sourceId, destId, dim);
    taskConvert.setSubscription(sourceId, sub("source"));
    taskConvert.setSubscription(destId, sub("dest"));
    auto success = taskConvert.start();

    if(success) {

        Subscription::Lock<multi_img> source_lock(*sub("source"));
        multi_img* source = source_lock();

        std::string banddesc;
        if(source->meta.size() > 0) {
            banddesc = source->meta[dim].str();
        }
        QString typestr = representation::prettyString(repr);
        QString desc;

        if(banddesc.empty()) {
            desc = QString("%1 Band #%2").arg(typestr).arg(dim+1);
        } else {
            desc = QString("%1 Band %2").arg(typestr).arg(banddesc.c_str());
        }

        Subscription::Lock<std::pair<QImage, QString>> dest_lock(*sub("dest"));
        dest_lock()->second = desc;
    }

    return success;
}
