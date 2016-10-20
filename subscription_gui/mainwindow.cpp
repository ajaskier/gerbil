#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <functional>

#include "subscription.h"
#include "task_scheduler.h"
#include "dependency.h"
#include "data.h"
#include "lock.h"
#include "subscription_factory.h"
#include "data_condition_informer.h"
#include "multi_img.h"

#include <imagemodel.h>
#include <fakemodel.h>
#include <distmodel.h>

#include "normdock.h"
#include "distviewcompute_utils.h"

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
    fakeModel = new FakeModel(sm, scheduler, this);
   // distModel = new DistModel(sm, scheduler, this);
    imageModel->setFilename("/home/olek/gerbil_data/peppers_descriptor.txt");
    imgSub = std::unique_ptr<Subscription>(SubscriptionFactory::create(Dependency("image", SubscriptionType::READ),
                                         SubscriberType::READER, this,
                                         std::bind(&MainWindow::imgUpdated, this)));


}

void MainWindow::initRest()
{
    ui->setupUi(this);
    ui->bandSlider->setMaximum(maxBands-1);

    normDock = new NormDock(this);
//    connect(normDock, &NormDock::normalizationParametersChanged,
//            imageModel, &ImageModel::setNormalizationParameters);

    connect(normDock, &NormDock::normalizationParametersChanged,
            this, &MainWindow::onNormalizationParametersChanged);
    connect(this, &MainWindow::normalizationParametersChanged,
            imageModel, &ImgModel::setNormalizationParameters);

    addDockWidget(Qt::RightDockWidgetArea, normDock);
//    ui->normDock = new NormDock(this);
//    ui->normDock->initUi();


    {
        Subscription::Lock<multi_img> lock(*imgSub);
        multi_img* img = lock();
        //imageModel->setROI(img->roi);

        QPixmap pix = QPixmap::fromImage(img->export_qt(1));
        ui->imageLabel->setPixmap(pix);
    }

    imageIMGSub = std::unique_ptr<Subscription>(SubscriptionFactory::create(Dependency("image.IMG", SubscriptionType::READ),
                                               SubscriberType::READER, this,
                                               std::bind(&MainWindow::displayImageIMG, this)));


    modelA = new ModelA(1, sm, scheduler, this);
    modelD = new ModelD(1, sm, scheduler, this);
    modelB = new ModelB(2, 3, sm, scheduler, this);

    ui->dockA->setVisible(false);
    ui->dockB->setVisible(false);
    ui->dockC->setVisible(false);

    connect(ui->dockA, SIGNAL(visibilityChanged(bool)), this,
            SLOT(dockAVisibilityChanged(bool)));
    connect(ui->dockB, SIGNAL(visibilityChanged(bool)), this,
            SLOT(dockBVisibilityChanged(bool)));
    connect(ui->dockC, SIGNAL(visibilityChanged(bool)), this,
            SLOT(dockCVisibilityChanged(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
    //delete imgIMG_Sub;
    //imgIMG_Sub->end();
}

void MainWindow::dockAVisibilityChanged(bool visible)
{
    if (visible) {

        scheduler->stop();
//        dataASub = SubscriptionFactory::create(Dependency("DATA_A", SubscriptionType::READ),
//                                               SubscriberType::READER, this,
//                                               std::bind(&MainWindow::displayA, this));


//        roiSub = SubscriptionFactory::create(Dependency("ROI.diff", SubscriptionType::READ),
//                                             SubscriberType::READER);

//        imgIMG_Sub = std::unique_ptr<Subscription>(SubscriptionFactory::create(Dependency("image.IMG", SubscriptionType::READ),
//                                                                               SubscriberType::READER));

        roiSub = SubscriptionFactory::create(Dependency("ROI.diff", SubscriptionType::READ),
                                                       SubscriberType::READER);

        distIMGSub = SubscriptionFactory::create(Dependency("dist.IMG", SubscriptionType::READ),
                                                 SubscriberType::READER, this,
                                                 std::bind(&MainWindow::displayDist, this));

//        imageIMGSub = SubscriptionFactory::create(Dependency("image.IMG", SubscriptionType::READ),
//                                                   SubscriberType::READER, this,
//                                                   std::bind(&MainWindow::displayFake, this));

//        distFAKESub = SubscriptionFactory::create(Dependency("dist.FAKE", SubscriptionType::READ),
//                                                   SubscriberType::READER, this,
//                                                   std::bind(&MainWindow::displayDistFake, this));




        scheduler->resume();

    } else {
        //dataASub->end();
       // delete dataASub;
        delete distIMGSub;
//        delete imageFAKESub;
//        delete distFAKESub;
        delete roiSub;
    }
}

void MainWindow::displayA()
{
    qDebug() << "displayA";

    Subscription::Lock<Data> lock(*dataASub);
    int num = lock()->num;

    ui->outputA->setText(QString::number(num));
}

void MainWindow::displayImageIMG()
{

    qDebug() << "image.IMG computed";

    qDebug() << "did I crash?!";
    Subscription::Lock<multi_img> lock(*imageIMGSub);
    multi_img* img = lock();
    qDebug() << "height:" << img->height << "width:" << img->width;

   // QPixmap pix = QPixmap::fromImage(img->export_qt(1));
   // ui->imageIMGLabel->setPixmap(pix);


//    Subscription::Lock<Data> lock(*imageFAKESub);
//    int num = lock()->num;

//    ui->imageFakeLabel->setText(QString::number(num));
}

void MainWindow::displayDist()
{

    qDebug() << "dist calculated";
    Subscription::Lock<std::vector<BinSet>, ViewportCtx> lock(*distIMGSub);
    std::vector<BinSet>* sets = lock();

    qDebug() << "alright!";
    //int num = lock()->num;

    //ui->distFakeLabel->setText(QString::number(num));
}

void MainWindow::displayROI()
{
//    Subscription::Lock<Data> lock(*roiSub);
//    int num = lock()->num;

//    ui->roiLabel->setText(QString::number(num));
}

void MainWindow::imgUpdated()
{
    //qDebug() << "img updated";

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

void MainWindow::dockBVisibilityChanged(bool visible)
{
    if (visible) {
        dataBSub = SubscriptionFactory::create(Dependency("DATA_B",SubscriptionType::READ),
                                               SubscriberType::READER, this,
                                               std::bind(&MainWindow::displayB, this));

        dataDSub = SubscriptionFactory::create(Dependency("DATA_D", SubscriptionType::READ),
                                               SubscriberType::READER, this,
                                               std::bind(&MainWindow::displayD, this));

    } else {
        //dataBSub->end();
        //dataDSub->end();
        delete dataBSub;
        delete dataDSub;
    }
}

void MainWindow::displayB()
{
    qDebug() << "displayB";

    Subscription::Lock<Data> lock(*dataBSub);
    int num = lock()->num;

    ui->outputB->setText(QString::number(num));
}

void MainWindow::dockCVisibilityChanged(bool visible)
{
    if (visible) {
        dataCSub = SubscriptionFactory::create(Dependency("DATA_C", SubscriptionType::READ),
                                               SubscriberType::READER, this,
                                               std::bind(&MainWindow::displayC, this));

    } else {
       // dataCSub->end();
        delete dataCSub;
    }
}

void MainWindow::displayC()
{
    qDebug() << "displayC";

    Subscription::Lock<Data> lock(*dataCSub);
    int num = lock()->num;

    ui->outputC->setText(QString::number(num));
}

void MainWindow::displayD()
{
    qDebug() << "displayD";
    Subscription::Lock<Data> lock(*dataDSub);
    int num = lock()->num;

    ui->outputD->setText(QString::number(num));
}

void MainWindow::on_inputA_textChanged(const QString &arg1)
{
    int num = arg1.toInt();
    modelA->setA(num);
}

void MainWindow::on_inputB_textChanged(const QString &arg1)
{
    int num = arg1.toInt();
    modelB->setB(num);
}

void MainWindow::on_inputC_textChanged(const QString &arg1)
{
    int num = arg1.toInt();
    modelB->setC(num);
}

void MainWindow::on_inputD_textChanged(const QString &arg1)
{
    int num = arg1.toInt();
    modelD->setD(num);
}

void MainWindow::on_checkBoxA_toggled(bool checked)
{
    ui->dockA->setVisible(checked);
}

void MainWindow::on_checkBoxB_toggled(bool checked)
{
    ui->dockB->setVisible(checked);
}

void MainWindow::on_checkBoxC_toggled(bool checked)
{
    ui->dockC->setVisible(checked);
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

void MainWindow::on_imageModelButton_clicked()
{
    QString image = "bands." + representation + "." + QString::number(currentBand);

    bandsSub = std::unique_ptr<Subscription>(SubscriptionFactory::create(Dependency(image,
                                                        SubscriptionType::READ),
                                             SubscriberType::READER, this,
                                             std::bind(&MainWindow::imgIMG_updated, this)));
}

void MainWindow::imgIMG_updated()
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

void MainWindow::on_imgButton_clicked()
{
    representation = "IMG";
    emit normalizationParametersChanged(representation::fromStr(representation), current_normMode, current_targetRange);
}

void MainWindow::on_normButton_clicked()
{
    representation = "NORM";
    emit normalizationParametersChanged(representation::fromStr(representation), current_normMode, current_targetRange);
}

void MainWindow::on_gradButton_clicked()
{
    representation = "GRAD";
    emit normalizationParametersChanged(representation::fromStr(representation), current_normMode, current_targetRange);
}

void MainWindow::on_pcaButton_clicked()
{
    representation = "IMGPCA";
    emit normalizationParametersChanged(representation::fromStr(representation), current_normMode, current_targetRange);
}

void MainWindow::on_bandSlider_sliderMoved(int position)
{
    currentBand = position;
}

void MainWindow::onNormalizationParametersChanged(representation::t type,
                                                  multi_img::NormMode normMode,
                                                  multi_img_base::Range targetRange)
{
    current_normMode = normMode;
    current_targetRange = targetRange;
    emit normalizationParametersChanged(representation::fromStr(representation),
                                        normMode, targetRange);


}

void MainWindow::on_gradpcaButton_clicked()
{
    representation = "GRADPCA";
    emit onNormalizationParametersChanged(representation::fromStr(representation), current_normMode, current_targetRange);
}

void MainWindow::on_fakeButton_clicked()
{
    scheduler->stop();
    imageModel->runImg();
    scheduler->resume();
}

void MainWindow::on_roiButton_clicked()
{
//    roiSub = SubscriptionFactory::create(Dependency("ROI.diff", SubscriptionType::READ),
//                                         SubscriberType::READER);


   // scheduler->stop();
    imageModel->setROI(cv::Rect(5, 5, 50, 50), false);
    imageModel->calculateROIdiff();
    imageModel->runImg();
 //   scheduler->resume();
}
