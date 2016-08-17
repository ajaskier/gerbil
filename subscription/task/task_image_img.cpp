#include "task_image_img.h"
#include "subscription.h"

#include "task_scope_image.h"
#include "task_rescale_tbb.h"
#include <QDebug>

TaskImageIMG::TaskImageIMG(cv::Rect roi, size_t bands, size_t roiBands,
                               bool includecache)
    : Task("image.IMG", {{"image", "source"}}), roi(roi), bands(bands),
      roiBands(roiBands), includecache(includecache)
{
}

TaskImageIMG::~TaskImageIMG()
{}

bool TaskImageIMG::run()
{
    auto sourceId = sub("source")->getDependency().dataId;
    auto destId = sub("dest")->getDependency().dataId;

    TaskScopeImage scopeImage(roi);
    scopeImage.setSubscription(sourceId, sub("source"));
    scopeImage.setSubscription(destId, sub("dest"));
    scopeImage.start();

    qDebug() << "halfway there";

    TaskRescaleTbb rescaleTbb(bands, roiBands, includecache);
    rescaleTbb.setSubscription(sourceId, sub("source"));
    rescaleTbb.setSubscription(destId, sub("dest"));
    rescaleTbb.start();

    return true;
}
