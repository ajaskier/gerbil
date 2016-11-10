#include "cwindow.h"
#include "ui_cwindow.h"

#include <QDebug>
#include <functional>

#include "subscription.h"
#include "dependency.h"
#include "data.h"
#include "lock.h"
#include "subscription_factory.h"




CWindow::CWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWindow)
{
    ui->setupUi(this);


    qDebug() << "displayC";
    dataCSub = SubscriptionFactory::create(Dependency("DATA_C", SubscriptionType::READ),
                                           SubscriberType::READER, this,
                                           std::bind(&CWindow::displayC, this));

}

void CWindow::displayC()
{
    Subscription::Lock<Data> lock(*dataCSub);
    int num = lock()->num;

    ui->outputC->setText(QString::number(num));
}

CWindow::~CWindow()
{
    delete dataCSub;
}

void CWindow::on_computeCButton_clicked()
{
    emit buttonCClicked();
}

void CWindow::on_inputC_textChanged(const QString &arg1)
{
    int num = arg1.toInt();
    emit inputCChanged(num);
}
