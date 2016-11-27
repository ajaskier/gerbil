#include "awindow.h"
#include "ui_awindow.h"

#include <QDebug>
#include <functional>

#include "subscription.h"
#include "dependency.h"
#include "data.h"
#include "lock.h"
#include "subscription_factory.h"




AWindow::AWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AWindow)
{
    ui->setupUi(this);


    qDebug() << "displayA";
    dataASub = SubscriptionFactory::create(Dependency("DATA_A", SubscriptionType::READ),
                                           AccessType::DEFERRED, this,
                                           std::bind(&AWindow::displayA, this));

}

void AWindow::displayA()
{
    Subscription::Lock<Data> lock(*dataASub);
    int num = lock()->num;

    ui->outputA->setText(QString::number(num));
}

AWindow::~AWindow()
{
    delete dataASub;
}

void AWindow::on_computeAButton_clicked()
{
    emit buttonAClicked();
}

void AWindow::on_inputA_textChanged(const QString &arg1)
{
    int num = arg1.toInt();
    emit inputAChanged(num);
}
