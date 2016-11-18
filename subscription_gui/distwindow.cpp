#include "distwindow.h"
#include "ui_distwindow.h"

#include <QDebug>
#include <functional>

#include "subscription.h"
#include "dependency.h"
#include "data.h"
#include "lock.h"
#include "subscription_factory.h"

#include "distviewcompute_utils.h"


DistWindow::DistWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DistWindow)
{
    ui->setupUi(this);

    distSub = SubscriptionFactory::create(Dependency("dist.IMG", SubscriptionType::READ),
                                           SubscriberType::READER, this,
                                           std::bind(&DistWindow::displayDist, this));

}

void DistWindow::displayDist()
{
    qDebug() << "dist calculated";
    Subscription::Lock<std::vector<BinSet>, ViewportCtx> lock(*distSub);
    std::vector<BinSet>* sets = lock();

    qDebug() << "sets size:" << sets->size();
}

DistWindow::~DistWindow()
{
    delete distSub;
}

void DistWindow::on_pushButton_clicked()
{
    emit calculateRequest();
}
