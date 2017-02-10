#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <functional>

#include "subscription.h"
#include "task_scheduler.h"
#include "dependency.h"
#include "lock.h"
#include "subscription_factory.h"
#include "data_condition_informer.h"
#include "multi_img.h"

#include "model/img_model.h"
#include "model/dist_model.h"

#include "normdock.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    initCrucials();
   // initRest();
}

void MainWindow::initCrucials()
{
    scheduler = new TaskScheduler(sm);
    SubscriptionFactory::init(&sm);
    DataConditionInformer::init(&sm);

    imageModel = new ImgModel(false, sm, scheduler, this);
    distModel = new DistModel(sm, scheduler, this);
   // distModel = new DistModel(sm, scheduler, this);
    imageModel->setFilename("/home/olek/gerbil_data/peppers_descriptor.txt");
    imgSub = std::unique_ptr<Subscription>(SubscriptionFactory::create(Dependency("image", SubscriptionType::READ,
                                         AccessType::DEFERRED), this,
                                         std::bind(&MainWindow::imgUpdated, this)));


}

void MainWindow::initRest()
{
    ui->setupUi(this);

    normDock = new NormDock(this);

    connect(normDock, &NormDock::normalizationParametersChanged,
            this, &MainWindow::onNormalizationParametersChanged);
    connect(this, &MainWindow::normalizationParametersChanged,
            imageModel, &ImgModel::setNormalizationParameters);

    addDockWidget(Qt::RightDockWidgetArea, normDock);


    modelA = new ModelA(1, sm, scheduler, this);
    modelD = new ModelD(1, sm, scheduler, this);
    modelB = new ModelB(2, 3, sm, scheduler, this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayROI()
{

    Subscription::Lock<
            cv::Rect,
            std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>>
            > lock(*roiSub);


    int startX = lock()->x;
    int startY = lock()->y;
    int width = lock()->width;
    int height = lock()->height;

    int subSize = lock.meta()->first.size();
    int addSize = lock.meta()->second.size();

    ui->startX_input->setText(QString::number(startX));
    ui->startY_input->setText(QString::number(startY));
    ui->width_input->setText(QString::number(width));
    ui->height_input->setText(QString::number(height));

    ui->roiSub_label->setText(QString::number(subSize));
    ui->roiAdd_label->setText(QString::number(addSize));

}

void MainWindow::imgUpdated()
{
    roiSub = std::unique_ptr<Subscription>(SubscriptionFactory::create(Dependency("ROI", SubscriptionType::READ,
                                               AccessType::DEFERRED), this,
                                               std::bind(&MainWindow::displayROI, this)));

    {
        Subscription::Lock<multi_img> lock(*imgSub);
        multi_img* img = lock();
        imageModel->setROI(img->roi);
        maxBands = img->size();
        imageModel->setBandsCount(maxBands);
    }

    initRest();
}

cv::Rect MainWindow::getDimensions()
{
    //Subscription::Lock<multi_img_base> lock(imgSub);
    //multi_img_base& img = lock();

    //return cv::Rect(0, 0, img.width, img.height);
    return cv::Rect(0, 0, 0, 0);
}

void MainWindow::on_checkBoxA_toggled(bool visible)
{

    if (visible) {

        aWindow = new AWindow();

        connect(aWindow, SIGNAL(buttonAClicked()), this, SLOT(on_computeAButton_clicked()));
        connect(aWindow, SIGNAL(inputAChanged(int)), modelA, SLOT(setA(int)));

        aWindow->show();
        aWindow->raise();

    } else {
        aWindow->deleteLater();
    }
}

void MainWindow::onDistIMGRequest()
{
    distModel->delegateTask("dist.IMG");
}

void MainWindow::onImageIMGRequest()
{
    imageModel->delegateTask("image.IMG");
}

void MainWindow::on_checkBoxB_toggled(bool visible)
{
    if (visible) {
        bWindow = new BWindow();

        connect(bWindow, SIGNAL(buttonBClicked()), this, SLOT(on_computeBButton_clicked()));
        connect(bWindow, SIGNAL(buttonDClicked()), this, SLOT(on_computeDButton_clicked()));
        connect(bWindow, SIGNAL(inputBChanged(int)), modelB, SLOT(setB(int)));
        connect(bWindow, SIGNAL(inputDChanged(int)), modelD, SLOT(setD(int)));

        bWindow->show();
        bWindow->raise();


    } else {
        bWindow->deleteLater();
    }
}

void MainWindow::on_checkBoxC_toggled(bool visible)
{
    if (visible) {

        cWindow = new CWindow();

        connect(cWindow, SIGNAL(buttonCClicked()), this, SLOT(on_computeCButton_clicked()));
        connect(cWindow, SIGNAL(inputCChanged(int)), modelB, SLOT(setC(int)));

        cWindow->show();
    } else {
        cWindow->deleteLater();
    }
}

void MainWindow::on_computeAButton_clicked()
{
    modelA->delegateTask("DATA_A");
}

void MainWindow::on_computeBButton_clicked()
{
    modelB->delegateTask("DATA_B");
}

void MainWindow::on_computeCButton_clicked()
{
    modelB->delegateTask("DATA_C");
}

void MainWindow::on_computeDButton_clicked()
{
    modelD->delegateTask("DATA_D");
}


void MainWindow::onNormalizationParametersChanged(representation::t type,
                                                  multi_img::NormMode normMode,
                                                  multi_img_base::Range targetRange)
{
    emit normalizationParametersChanged(representation::fromStr(representation),
                                        normMode, targetRange);

}

void MainWindow::on_roiButton_clicked()
{

    int startX = ui->startX_input->text().toInt();
    int startY = ui->startY_input->text().toInt();
    int width = ui->width_input->text().toInt();
    int height = ui->height_input->text().toInt();

    imageModel->setROI(cv::Rect(startX, startY, width, height));
}


void MainWindow::on_bands_checkbox_toggled(bool visible)
{
    if (visible) {

        bandsWindow = new BandsWindow();
        connect(bandsWindow, &BandsWindow::normalizationParametersChanged,
                                    imageModel, &ImgModel::setNormalizationParameters);
        connect(normDock, &NormDock::normalizationParametersChanged,
                bandsWindow, &BandsWindow::onNormalizationParametersChanged);
        bandsWindow->setMax(maxBands-1);

        bandsWindow->show();


    } else {
        bandsWindow->deleteLater();
    }
}

void MainWindow::on_img_checkbox_toggled(bool visible)
{
    if (visible) {

        imgWindow = new ImgWindow();
        imgWindow->show();

        connect(imgWindow, &ImgWindow::calculateRequest,
                this, &MainWindow::onImageIMGRequest);

    } else {
        imgWindow->deleteLater();
    }
}

void MainWindow::on_dist_checkbox_toggled(bool visible)
{
    if (visible) {
        distWindow = new DistWindow();

        connect(distWindow, &DistWindow::calculateRequest,
                this, &MainWindow::onDistIMGRequest);

        distWindow->show();
    } else {
        distWindow->deleteLater();
    }
}
