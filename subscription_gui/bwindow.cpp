#include "bwindow.h"
#include "ui_bwindow.h"

#include <QDebug>
#include <functional>

#include "subscription.h"
#include "dependency.h"
#include "data.h"
#include "lock.h"
#include "data_register.h"




BWindow::BWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BWindow)
{
    ui->setupUi(this);


    qDebug() << "displayB";
    dataBSub = DataRegister::subscribe(Dependency("DATA_B", SubscriptionType::READ,
                                           AccessType::DEFERRED), this,
                                           std::bind(&BWindow::displayB, this));
    dataDSub = DataRegister::subscribe(Dependency("DATA_D", SubscriptionType::READ,
                                           AccessType::DEFERRED), this,
                                           std::bind(&BWindow::displayD, this));

}

void BWindow::displayB()
{
    Subscription::Lock<Data> lock(*dataBSub);
    int num = lock()->num;

    ui->outputB->setText(QString::number(num));
}

void BWindow::displayD()
{
    qDebug() << "displayD";
    Subscription::Lock<Data> lock(*dataDSub);
    int num = lock()->num;

    ui->outputD->setText(QString::number(num));
}

BWindow::~BWindow()
{
    delete dataBSub;
    delete dataDSub;
}

void BWindow::on_computeBButton_clicked()
{
    emit buttonBClicked();
}

void BWindow::on_computeDButton_clicked()
{
    emit buttonDClicked();
}

void BWindow::on_inputD_textChanged(const QString &arg1)
{
    int num = arg1.toInt();
    emit inputDChanged(num);
}

void BWindow::on_inputB_textChanged(const QString &arg1)
{
    int num = arg1.toInt();
    emit inputBChanged(num);
}
