#include "task_image_lim.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "multi_img/multi_img_offloaded.h"
#include "imginput.h"

TaskImageLim::TaskImageLim(const QString &filename, bool limitedMode, QObject *parent)
    : Task("image", parent), filename(filename), limitedMode(limitedMode)
{
    dependencies = {Dependency("image", SubscriptionType::WRITE)};
}

TaskImageLim::~TaskImageLim()
{

}

void TaskImageLim::run()
{
    std::string fn = filename.toLocal8Bit().constData();
    if (limitedMode) {
        std::pair<std::vector<std::string>, std::vector<multi_img::BandDesc>>
                filelist = multi_img::parse_filelist(fn);
        multi_img_offloaded offloaded = multi_img_offloaded(filelist.first,
                                                            filelist.second);

        Subscription::Lock<multi_img_offloaded> lock(sub);
        lock.swap(offloaded);
    } else {
        imginput::ImgInputConfig inputConfig;
        inputConfig.file = fn;
        multi_img::ptr img = imginput::ImgInput(inputConfig).execute();

        Subscription::Lock<multi_img> lock(sub);
        lock.swap(*img);
    }
}

void TaskImageLim::setSubscription(QString id, Subscription *sub)
{
    this->sub = sub;
}

void TaskImageLim::endSubscriptions()
{
    sub->end();
}
