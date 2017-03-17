#include "imgwindow.h"
#include "ui_imgwindow.h"

#include <QDebug>
#include <functional>

#include "subscription.h"
#include "task_scheduler.h"
#include "dependency.h"
#include "lock.h"
#include "data_register.h"
#include "multi_img.h"


#include "distviewcompute_utils.h"

ImgWindow::ImgWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ImgWindow)
{
	ui->setupUi(this);
	sub = DataRegister::subscribe(Dependency("image.IMG", SubscriptionType::READ,
	                                         AccessType::DEFERRED), this,
	                              std::bind(&ImgWindow::display, this));
}

ImgWindow::~ImgWindow()
{
	delete sub;
}

void ImgWindow::display()
{
	Subscription::Lock<multi_img> lock(*sub);
	multi_img* img = lock();
	qDebug() << "height:" << img->height << "width:" << img->width;

	QPixmap pix = QPixmap::fromImage(img->export_qt(1));
	ui->label->setPixmap(pix);
	ui->label->resize(pix.width(), pix.height());
	int margin = 15;
	this->resize(pix.width() + margin, pix.height() + margin);
}

void ImgWindow::on_pushButton_clicked()
{
	emit calculateRequest();
}
