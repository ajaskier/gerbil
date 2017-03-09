#include "bandswindow.h"
#include "ui_bandswindow.h"

#include <QDebug>
#include <functional>

#include "subscription.h"
#include "dependency.h"
#include "lock.h"
#include "data_register.h"
#include "multi_img.h"


#include "distviewcompute_utils.h"

BandsWindow::BandsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BandsWindow)
{
    ui->setupUi(this);
}

BandsWindow::~BandsWindow()
{
    if (bandsSub) delete bandsSub;
}

void BandsWindow::display()
{
    qDebug() << "did I crash?!";
    Subscription::Lock<std::pair<QImage, QString>> lock(*bandsSub);
    //multi_img* img = lock();
    QImage img = lock()->first;
    QString desc = lock()->second;
    qDebug() << desc;

    //QPixmap pix = QPixmap::fromImage(img->export_qt(1));
    QPixmap pix = QPixmap::fromImage(img);
    ui->imageLabel->setPixmap(pix);
}

void BandsWindow::on_imageModelButton_clicked()
{
    QString image = "bands." + representation + "." + QString::number(currentBand);

    bandsSub = DataRegister::subscribe(Dependency(image,
                                                        SubscriptionType::READ,
                                             AccessType::DEFERRED), this,
                                             std::bind(&BandsWindow::display, this));
}

void BandsWindow::on_imgButton_clicked()
{
    representation = "IMG";
    emit normalizationParametersChanged(representation::fromStr(representation), current_normMode, current_targetRange);
}

void BandsWindow::on_normButton_clicked()
{
    representation = "NORM";
    emit normalizationParametersChanged(representation::fromStr(representation), current_normMode, current_targetRange);
}

void BandsWindow::on_gradButton_clicked()
{
    representation = "GRAD";
    emit normalizationParametersChanged(representation::fromStr(representation), current_normMode, current_targetRange);
}

void BandsWindow::on_pcaButton_clicked()
{
    representation = "IMGPCA";
    emit normalizationParametersChanged(representation::fromStr(representation), current_normMode, current_targetRange);
}

void BandsWindow::on_gradpcaButton_clicked()
{
    representation = "GRADPCA";
    emit onNormalizationParametersChanged(representation::fromStr(representation), current_normMode, current_targetRange);
}

void BandsWindow::on_bandSlider_sliderMoved(int position)
{
    currentBand = position;
}

void BandsWindow::onNormalizationParametersChanged(representation::t type,
                                                   multi_img::NormMode normMode,
                                                   multi_img_base::Range targetRange)
{
    current_normMode = normMode;
    current_targetRange = targetRange;
}

void BandsWindow::setMax(int max)
{
    ui->bandSlider->setMaximum(max);
}
