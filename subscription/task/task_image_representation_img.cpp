#include "task_image_representation_img.h"
#include "subscription.h"
#include "lock.h"

TaskImageRepresentationImg::TaskImageRepresentationImg(QObject *parent)
    : Task("IMG", parent)
{

}

void TaskImageRepresentationImg::run()
{

}

void TaskImageRepresentationImg::setSubscription(QString id, Subscription *sub)
{
    this->sub = sub;
}

void TaskImageRepresentationImg::endSubscriptions()
{
    sub->end();
}
