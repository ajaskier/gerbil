#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <subscription_manager.h>

#include <model/model_a.h>
#include <model/model_b.h>
#include <model/model_d.h>
//#include <imagemodel.h>
//#include <fakemodel.h>

#include "model/representation.h"
#include "multi_img.h"

#include "awindow.h"
#include "bwindow.h"
#include "cwindow.h"
#include "imgwindow.h"
#include "bandswindow.h"
#include "distwindow.h"

class NormDock;
//class DistModel;
class ImgModel;
class FakeModel;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    AWindow* aWindow;
    BWindow* bWindow;
    CWindow* cWindow;
    ImgWindow* imgWindow;
    BandsWindow* bandsWindow;
    DistWindow* distWindow;

    ModelA* modelA;
    ModelB* modelB;
    ModelD* modelD;
    ImgModel* imageModel;
    FakeModel* fakeModel;
    //DistModel* distModel;

    SubscriptionManager sm;
    TaskScheduler* scheduler;

    std::unique_ptr<Subscription> imgSub;
    std::unique_ptr<Subscription> roiSub;

    QString representation = "IMG";
    int currentBand = 0;
    size_t maxBands = 0;

    NormDock* normDock;


signals:
    void normalizationParametersChanged(
            representation::t type,
            multi_img::NormMode normMode,
            multi_img::Range targetRange
            );

private slots:

    void initCrucials();
    void initRest();

    cv::Rect getDimensions();

    void displayROI();
    void imgUpdated();

    void on_checkBoxA_toggled(bool visible);
    void on_checkBoxB_toggled(bool visible);
    void on_checkBoxC_toggled(bool visible);
    void on_computeAButton_clicked();
    void on_computeBButton_clicked();
    void on_computeCButton_clicked();
    void on_computeDButton_clicked();
    void onDistIMGRequest();
    void onImageIMGRequest();

    void onNormalizationParametersChanged(
            representation::t type,
            multi_img::NormMode normMode,
            multi_img::Range targetRange
            );

    void on_roiButton_clicked();
    void on_bands_checkbox_toggled(bool visible);
    void on_img_checkbox_toggled(bool visible);
    void on_dist_checkbox_toggled(bool visible);
};

#endif // MAINWINDOW_H
