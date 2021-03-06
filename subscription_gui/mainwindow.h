#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <subscription_manager.h>

#include <model/model_a.h>
#include <model/model_b.h>
#include <model/model_d.h>

#include "model/representation.h"
#include "multi_img.h"
#include "graphseg_config.h"

#include "awindow.h"
#include "bwindow.h"
#include "cwindow.h"
#include "imgwindow.h"
#include "bandswindow.h"
#include "distwindow.h"
#include "dist/distviewgui2.h"

class NormDock;
class DistModel;
class ImgModel;
class LabelsModel;
class ClusterizationModel;
class FalsecolorModel;
class GraphSegModel;

class BandDock;
class LabelDock;
class RoiDock;
class ClusteringDock;
class FalseColorDock;

namespace Ui {
class MainWindow;
}

namespace seg_graphs
{
class GraphSegConfig;
}


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;

	AWindow    * aWindow;
	BWindow    * bWindow;
	CWindow    * cWindow;
	ImgWindow  * imgWindow;
	BandsWindow* bandsWindow;
	DistWindow * distWindow;

	ModelA      * modelA;
	ModelB      * modelB;
	ModelD      * modelD;
	ImgModel    * imageModel;
	DistModel   * distModel;
	LabelsModel * labelsModel;
	ClusterizationModel * clusterizationModel;
	FalsecolorModel     * falsecolorModel;
	GraphSegModel       * graphSegModel;

	SubscriptionManager sm;
	TaskScheduler       * scheduler;

	std::unique_ptr<Subscription> imgSub;
	std::unique_ptr<Subscription> roiSub;


	QString representation = "IMG";
	int     currentBand    = 0;
	size_t  maxBands       = 0;

	NormDock       * normDock;
	BandDock       * bandDock;
	LabelDock      * labelDock;
	RoiDock        * roiDock;
	ClusteringDock * clusteringDock;
	FalseColorDock * falsecolorDock;

	cv::Rect originalRoi;


signals:
	void normalizationParametersChanged(
	    representation::t   type,
	    multi_img::NormMode normMode,
	    multi_img::Range    targetRange
	    );

	void requestGraphsegBand(representation::t type, int bandId,
	                         cv::Mat1s seedMap,
	                         const seg_graphs::GraphSegConfig config,
	                         bool resetLabel);

	void requestGraphseg(representation::t                type,
	                     cv::Mat1s                        seedMap,
	                     const seg_graphs::GraphSegConfig config,
	                     bool                             resetLabel);

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

	void requestGraphsegCurBand(const seg_graphs::GraphSegConfig config,
	                            bool                             resetLabel);

	void requestGraphseg(representation::t                repr,
	                     const seg_graphs::GraphSegConfig config,
	                     bool                             resetLabel);

	void onNormalizationParametersChanged(
	    representation::t   type,
	    multi_img::NormMode normMode,
	    multi_img::Range    targetRange
	    );

	void on_roiButton_clicked();
	void on_bands_checkbox_toggled(bool visible);
	void on_img_checkbox_toggled(bool visible);
	void on_dist_checkbox_toggled(bool visible);
	void on_labels_checkbox_toggled(bool checked);
	void on_labels_icons_checkbox_toggled(bool checked);
	void on_roi_checkbox_toggled(bool checked);
	void on_clustering_checkbox_toggled(bool checked);
	void on_falsecolor_checkbox_toggled(bool checked);
};

#endif // MAINWINDOW_H
