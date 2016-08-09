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

void TaskImageRepresentationImg::setSubscription(QString id,
                                                 std::unique_ptr<Subscription> sub)
{
    this->sub = std::move(sub);
}
