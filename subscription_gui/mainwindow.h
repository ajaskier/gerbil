#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <subscription_manager.h>

#include <model/model_a.h>
#include <model/model_b.h>
#include <model/model_d.h>
#include <imagemodel.h>
#include "model/representation.h"

class NormDock;

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

    ModelA* modelA;
    ModelB* modelB;
    ModelD* modelD;
    ImageModel* imageModel;

    SubscriptionManager sm;
    TaskScheduler* scheduler;

    std::unique_ptr<Subscription> imgSub;
    std::unique_ptr<Subscription> imgIMG_Sub;
    /*
    Subscription* imgSub;
    Subscription* imgIMG_Sub;
    */

    Subscription* dataASub;
    Subscription* dataBSub;
    Subscription* dataCSub;
    Subscription* dataDSub;

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

    void dockAVisibilityChanged(bool visible);
    void dockBVisibilityChanged(bool visible);
    void dockCVisibilityChanged(bool visible);

    void displayA();
    void displayB();
    void displayC();
    void displayD();
    void imgUpdated();
    void imgIMG_updated();

    void on_inputA_textChanged(const QString &arg1);
    void on_inputB_textChanged(const QString &arg1);
    void on_inputC_textChanged(const QString &arg1);
    void on_checkBoxA_toggled(bool checked);
    void on_checkBoxB_toggled(bool checked);
    void on_checkBoxC_toggled(bool checked);
    void on_computeAButton_clicked();
    void on_computeBButton_clicked();
    void on_computeCButton_clicked();
    void on_computeDButton_clicked();
    void on_inputD_textChanged(const QString &arg1);
    void on_imageModelButton_clicked();
    void on_imgButton_clicked();
    void on_normButton_clicked();
    void on_gradButton_clicked();
    void on_pcaButton_clicked();
    void on_bandSlider_sliderMoved(int position);

    void onNormalizationParametersChanged(
            representation::t type,
            multi_img::NormMode normMode,
            multi_img::Range targetRange
            );
    void on_gradpcaButton_clicked();
};

#endif // MAINWINDOW_H
