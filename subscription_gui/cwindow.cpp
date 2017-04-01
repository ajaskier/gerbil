#include "cwindow.h"
#include "ui_cwindow.h"

#include <QDebug>
#include <functional>

#include "subscription.h"
#include "dependency.h"
#include "data.h"
#include "lock.h"
#include "data_register.h"




CWindow::CWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CWindow)
{
	ui->setupUi(this);


	qDebug() << "displayC";
	dataCSub = DataRegister::subscribe(Dependency("DATA_C", SubscriptionType::READ,
	                                              AccessType::DEFERRED), this,
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
	int  num = arg1.toInt();
	emit inputCChanged(num);
}
