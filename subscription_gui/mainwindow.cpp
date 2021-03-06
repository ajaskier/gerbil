#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <functional>

#include "subscription.h"
#include "task_scheduler.h"
#include "dependency.h"
#include "lock.h"
#include "data_register.h"
#include "multi_img.h"

#include "model/img_model.h"
#include "model/dist_model.h"
#include "model/labels_model.h"
#include "model/clusterization_model.h"
#include "model/falsecolor_model.h"
#include "model/graph_seg_model.h"

#include "normdock.h"
#include "labels/banddock.h"
#include "labels/bandview.h"
#include "labels/graphsegwidget.h"

#include "labels/labeldock.h"
#include "roi/roidock.h"
#include "clustering/clusteringdock.h"
#include "falsecolor/falsecolordock.h"


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	initCrucials();
	// initRest();
}

void MainWindow::initCrucials()
{
	scheduler = new TaskScheduler(sm, this);
	DataRegister::init(&sm);

	imageModel          = new ImgModel(false, scheduler, this);
	labelsModel         = new LabelsModel(scheduler, this);
	distModel           = new DistModel(scheduler, this);
	clusterizationModel = new ClusterizationModel(scheduler, this);
	falsecolorModel     = new FalsecolorModel(scheduler, this);
	graphSegModel       = new GraphSegModel(scheduler, this);
	imageModel->setFilename("/home/ocieslak/gerbil_data/fake_and_real_peppers_ms.txt");
	imgSub =
	    std::unique_ptr<Subscription>(DataRegister::subscribe(Dependency("image",
	                                                                     SubscriptionType::READ,
	                                                                     AccessType::DEFERRED),
	                                                          this,
	                                                          std::bind(&MainWindow::imgUpdated,
	                                                                    this)));
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


	modelA = new ModelA(1, scheduler, this);
	modelD = new ModelD(1, scheduler, this);
	modelB = new ModelB(2, 3, scheduler, this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::displayROI()
{
	Subscription::Lock<
	    cv::Rect, ROIMeta> lock(*roiSub);


	int startX = lock()->x;
	int startY = lock()->y;
	int width  = lock()->width;
	int height = lock()->height;

	int subSize = lock.meta()->sub.size();
	int addSize = lock.meta()->add.size();

	ui->startX_input->setText(QString::number(startX));
	ui->startY_input->setText(QString::number(startY));
	ui->width_input->setText(QString::number(width));
	ui->height_input->setText(QString::number(height));

	ui->roiSub_label->setText(QString::number(subSize));
	ui->roiAdd_label->setText(QString::number(addSize));
}

void MainWindow::imgUpdated()
{
	roiSub =
	    std::unique_ptr<Subscription>(DataRegister::subscribe(Dependency("ROI",
	                                                                     SubscriptionType::READ,
	                                                                     AccessType::DEFERRED),
	                                                          this,
	                                                          std::bind(&MainWindow::displayROI,
	                                                                    this)));

	{
		Subscription::Lock<multi_img> lock(*imgSub);
		multi_img* img = lock();

		originalRoi = img->roi;
		imageModel->setROI(img->roi);
		maxBands = img->size();
		imageModel->setBandsCount(maxBands);

		labelsModel->setImageSize(cv::Size(originalRoi.width,
		                                   originalRoi.height));
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


void MainWindow::onNormalizationParametersChanged(representation::t     type,
                                                  multi_img::NormMode   normMode,
                                                  multi_img_base::Range targetRange)
{
	emit normalizationParametersChanged(representation::fromStr(representation),
	                                    normMode, targetRange);
}

void MainWindow::on_roiButton_clicked()
{
	int startX = ui->startX_input->text().toInt();
	int startY = ui->startY_input->text().toInt();
	int width  = ui->width_input->text().toInt();
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
		bandsWindow->setMax(maxBands - 1);

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

void MainWindow::on_labels_checkbox_toggled(bool checked)
{
	if (checked) {
		bandDock = new BandDock(originalRoi, this);
		addDockWidget(Qt::TopDockWidgetArea, bandDock);

		connect(bandDock, &BandDock::newLabelRequested,
		        labelsModel, &LabelsModel::addLabel);

		connect(bandDock->bandView(), &BandView::alteredLabels,
		        labelsModel, &LabelsModel::alterPixels);

		connect(bandDock->bandView(), SIGNAL(newLabeling(cv::Mat1s)),
		        labelsModel, SLOT(setLabels(const cv::Mat1s &)));

		connect(bandDock->graphSegWidget(),
		        SIGNAL(requestGraphseg(representation::t, seg_graphs::GraphSegConfig, bool)),
		        this, SLOT(requestGraphseg(representation::t, seg_graphs::GraphSegConfig, bool)));

		connect(this,
		        SIGNAL(requestGraphseg(representation::t, cv::Mat1s, seg_graphs::GraphSegConfig,
		                               bool)),
		        graphSegModel,
		        SLOT(runGraphseg(representation::t, cv::Mat1s, seg_graphs::GraphSegConfig, bool)));

		connect(bandDock->graphSegWidget(),
		        SIGNAL(requestGraphsegCurBand(seg_graphs::GraphSegConfig, bool)),
		        this, SLOT(requestGraphsegCurBand(seg_graphs::GraphSegConfig, bool)));

		connect(this,
		        SIGNAL(requestGraphsegBand(representation::t, int, cv::Mat1s,
		                                   seg_graphs::GraphSegConfig,
		                                   bool)),
		        graphSegModel,
		        SLOT(runGraphsegBand(representation::t, int, cv::Mat1s,
		                             seg_graphs::GraphSegConfig,
		                             bool)));

		connect(bandDock, SIGNAL(currentLabelChanged(int)),
		        graphSegModel, SLOT(setCurLabel(int)));

		connect(graphSegModel, SIGNAL(seedingDone()),
		        bandDock->graphSegWidget(), SLOT(processSeedingDone()));
	} else {
		removeDockWidget(bandDock);
		bandDock->deleteLater();
	}
}

void MainWindow::requestGraphsegCurBand(const seg_graphs::GraphSegConfig config, bool resetLabel)
{
	representation::t repr = bandDock->getCurRepresentation();
	int       bandId       = bandDock->getCurBandId();
	cv::Mat1s seedMap      = bandDock->bandView()->getSeedMap();

	emit requestGraphsegBand(repr, bandId, seedMap, config, resetLabel);
}

void MainWindow::requestGraphseg(representation::t repr, const seg_graphs::GraphSegConfig config,
                                 bool resetLabel)
{
	cv::Mat1s seedMap = bandDock->bandView()->getSeedMap();
	emit      requestGraphseg(repr, seedMap, config, resetLabel);
}

void MainWindow::on_labels_icons_checkbox_toggled(bool checked)
{
	if (checked) {
		labelDock = new LabelDock(this);
		labelDock->setImageSize(cv::Size(originalRoi.width,
		                                 originalRoi.height));
		addDockWidget(Qt::TopDockWidgetArea, labelDock);

		connect(labelDock, &LabelDock::applyROIChanged,
		        labelsModel, &LabelsModel::setApplyROI);

		connect(labelDock, &LabelDock::labelMaskIconsRequested,
		        labelsModel, &LabelsModel::setIconsSize);

		connect(labelDock, &LabelDock::mergeLabelsRequested,
		        labelsModel, &LabelsModel::mergeLabels);

		connect(labelDock, &LabelDock::deleteLabelsRequested,
		        labelsModel, &LabelsModel::deleteLabels);

		connect(labelDock, &LabelDock::consolidateLabelsRequested,
		        labelsModel, &LabelsModel::consolidateLabels);
	} else {
		removeDockWidget(labelDock);
		labelDock->deleteLater();
	}
}

void MainWindow::on_roi_checkbox_toggled(bool checked)
{
	if (checked) {
		roiDock = new RoiDock(this);
		addDockWidget(Qt::TopDockWidgetArea, roiDock);

		connect(roiDock, &RoiDock::roiRequested,
		        imageModel, &ImgModel::setROI);
	} else {
		removeDockWidget(roiDock);
		roiDock->deleteLater();
	}
}

void MainWindow::on_clustering_checkbox_toggled(bool checked)
{
	if (checked) {
		clusteringDock = new ClusteringDock(this);
		addDockWidget(Qt::TopDockWidgetArea, clusteringDock);

		connect(clusteringDock, SIGNAL(segmentationRequested(ClusteringRequest)),
		        clusterizationModel, SLOT(requestSegmentation(ClusteringRequest)));

		connect(clusteringDock, SIGNAL(cancelSegmentationRequested()),
		        clusterizationModel, SIGNAL(abort()));

		connect(clusterizationModel, SIGNAL(progressChanged(int)),
		        clusteringDock, SLOT(updateProgress(int)));

		connect(clusterizationModel, SIGNAL(segmentationCompleted()),
		        clusteringDock, SLOT(processSegmentationCompleted()));
	} else {
		removeDockWidget(clusteringDock);
		clusteringDock->deleteLater();
	}
}

void MainWindow::on_falsecolor_checkbox_toggled(bool checked)
{
	if (checked) {
		falsecolorDock = new FalseColorDock(this);
		addDockWidget(Qt::TopDockWidgetArea, falsecolorDock);

		connect(falsecolorDock, SIGNAL(requestFalseColoring(FalseColoring::Type, bool)),
		        falsecolorModel, SLOT(requestColoring(FalseColoring::Type, bool)));

		connect(falsecolorModel, SIGNAL(progressChanged(FalseColoring::Type, int)),
		        falsecolorDock, SLOT(processCalculationProgressChanged(FalseColoring::Type, int)));

		connect(falsecolorModel, SIGNAL(coloringCancelled(FalseColoring::Type)),
		        falsecolorDock, SLOT(processComputationCancelled(FalseColoring::Type)));

		connect(falsecolorDock, SIGNAL(cancelComputation(FalseColoring::Type)),
		        falsecolorModel, SLOT(requestAbort(FalseColoring::Type)));

		connect(falsecolorDock,
		        SIGNAL(requestComputeSpecSim(int, int, similarity_measures::SMConfig)),
		        falsecolorModel, SLOT(computeSpecSim(int, int, similarity_measures::SMConfig)));
	} else {
		removeDockWidget(falsecolorDock);
		falsecolorDock->deleteLater();
	}
}
