#include "distwindow.h"
#include "ui_distwindow.h"

#include <QDebug>
#include <functional>

#include "subscription.h"
#include "dependency.h"
#include "lock.h"
#include "subscription_factory.h"

#include "distviewcompute_utils.h"


DistWindow::DistWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DistWindow)
{
    ui->setupUi(this);

    distSub = SubscriptionFactory::create(Dependency("dist.IMG", SubscriptionType::READ,
                                           AccessType::DEFERRED), this,
                                           std::bind(&DistWindow::displayDist, this));

}

void DistWindow::displayDist()
{
    if (!guiCreated) {
        gui = new DistViewGUI2(representation::t::IMG);
        QWidget *frame = gui->getFrame();
        ui->gridLayout->addWidget(frame, 1, 1);

        gui->setActive();

        guiCreated = true;
    }
}

DistWindow::~DistWindow()
{
    delete distSub;
    gui->deleteLater();
}

void DistWindow::on_pushButton_clicked()
{
    emit calculateRequest();
}
