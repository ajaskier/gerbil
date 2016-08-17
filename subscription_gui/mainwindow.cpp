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
#include "multi_img.h"

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

    imageModel = new ImageModel(false, sm, scheduler, this);
    imageModel->setFilename("/home/olek/gerbil_data/peppers_descriptor.txt");
    imgSub = std::unique_ptr<Subscription>(SubscriptionFactory::create(Dependency("image", SubscriptionType::READ),
                                         SubscriberType::READER, this,
                                         std::bind(&MainWindow::imgUpdated, this)));
}

void MainWindow::initRest()
{
    ui->setupUi(this);
    ui->bandSlider->setMaximum(maxBands-1);


    {
        Subscription::Lock<multi_img> lock(*imgSub);
        multi_img* img = lock();
        imageModel->setROI(img->roi);

        QPixmap pix = QPixmap::fromImage(img->export_qt(1));
        ui->imageLabel->setPixmap(pix);
    }


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
        dataASub = SubscriptionFactory::create(Dependency("DATA_A", SubscriptionType::READ),
                                               SubscriberType::READER, this,
                                               std::bind(&MainWindow::displayA, this));
    } else {
        //dataASub->end();
        delete dataASub;
    }

}

void MainWindow::displayA()
{
    qDebug() << "displayA";

    Subscription::Lock<Data> lock(*dataASub);
    int num = lock()->num;

    ui->outputA->setText(QString::number(num));
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

    imgIMG_Sub = std::unique_ptr<Subscription>(SubscriptionFactory::create(Dependency(image,
                                                        SubscriptionType::READ),
                                             SubscriberType::READER, this,
                                             std::bind(&MainWindow::imgIMG_updated, this)));
}

void MainWindow::imgIMG_updated()
{
    qDebug() << "did I crash?!";
    Subscription::Lock<std::pair<QImage, QString>> lock(*imgIMG_Sub);
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
}

void MainWindow::on_normButton_clicked()
{
    representation = "NORM";
}

void MainWindow::on_gradButton_clicked()
{
    representation = "GRAD";
}

void MainWindow::on_pcaButton_clicked()
{
    representation = "IMGPCA";
}

void MainWindow::on_bandSlider_sliderMoved(int position)
{
    currentBand = position;
}
