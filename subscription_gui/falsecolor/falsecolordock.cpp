#include <QVBoxLayout>
#include <QThread>
#include <QSettings>

#include <iostream>

#include "falsecolordock.h"
#include "model/falsecolor/falsecoloring.h"
#include "widgets/scaledview.h"
#include "widgets/autohideview.h"
#include "widgets/autohidewidget.h"
#include "similaritywidget.h"
#include <app/gerbilio.h>

#include "data_register.h"
#include "subscription.h"
#include "dependency.h"
#include "lock.h"

#include <memory>

#include "model/falsecolor_model.h"
#include "falsecolor_subscription_delegate.h"

//#define GGDBG_MODULE
#include "gerbil_gui_debug.h"

std::ostream & operator<<(std::ostream& os, const FalseColoringState::Type& state)
{
	if (state < 0 || state >= 4) {
		os << "INVALID";
		return os;
	}
	const char * const str[] = {
		"UNKNOWN",
		"FINISHED",
		"CALCULATING",
		"ABORTING"
	};
	os << str[state];
	return os;
}

static QStringList prettyFalseColorNames = QStringList()
                                           << "True Color (CIE XYZ)"
                                           << "Principle Component Analysis (PCA)"
                                           << "Spectral-gradient PCA"
                                           << "Self-organizing Map (SOM)"
                                           << "Spectral-gradient SOM";

FalseColorDock::FalseColorDock(QWidget *parent)
    : QDockWidget(parent)
{
	for (auto type : FalseColoring::all()) {
		FalseColorSubscriptionDelegate *delegate = new FalseColorSubscriptionDelegate(type, this);

		connect(delegate, SIGNAL(coloringUpdated(FalseColoring::Type)),
		        this, SLOT(coloringUpdated(FalseColoring::Type)));

		subs[type] = delegate;
	}

	setObjectName("FalseColorDock");
	/* setup our UI here as it is quite minimalistic */
	QWidget     *contents = new QWidget(this);
	QVBoxLayout *layout   = new QVBoxLayout(contents);
	view = new AutohideView(contents);
	view->setBaseSize(QSize(250, 300));
	view->setFrameShape(QFrame::NoFrame);
	view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	layout->addWidget(view);
	this->setWidget(contents);

	connect(QApplication::instance(), SIGNAL(lastWindowClosed()),
	        this, SLOT(saveState()));
	initUi();
	restoreState();
}

void FalseColorDock::processFalseColoringUpdate(FalseColoring::Type coloringType, QPixmap p)
{
	GGDBGM("enterState():" << endl);
	GGDBGM("receiving coloring " << coloringType << endl);
	enterState(coloringType, FalseColoringState::FINISHED);
	coloringUpToDate[coloringType] = true;
	updateTheButton();
	updateProgressBar();

	sw->doneAction()->setEnabled(false);
	if (coloringType == selectedColoring()) {
		//GGDBGM("updating " << coloringType << endl);
		view->setEnabled(true);
		scene->setPixmap(p);
		view->update();
		// good idea but seems distracting right now
		//view->setToolTip(prettyFalseColorNames[coloringType]);
		this->setWindowTitle(prettyFalseColorNames[coloringType]);
		lastShown = coloringType;
	}
}

void FalseColorDock::processComputationCancelled(FalseColoring::Type coloringType)
{
	coloringProgress[coloringType] = 0;
	GGDBGM("enterState():" << endl);
	enterState(coloringType, FalseColoringState::UNKNOWN);
	updateTheButton();
	updateProgressBar();

//	if (coloringState[coloringType] == FalseColoringState::ABORTING) {
//		coloringProgress[coloringType] = 0;
//		GGDBGM("enterState():" << endl);
//		enterState(coloringType, FalseColoringState::UNKNOWN);
//		updateTheButton();
//		updateProgressBar();
//	} else if (coloringState[coloringType] == FalseColoringState::CALCULATING) {
//		enterState(coloringType, FalseColoringState::UNKNOWN);
//		GGDBGM("restarting cancelled computation" << endl);
//		requestColoring(coloringType);
//	}
}

void FalseColorDock::setCalculationInProgress(FalseColoring::Type coloringType)
{
	if (selectedColoring() == coloringType) {
		enterState(selectedColoring(), FalseColoringState::CALCULATING);
		updateTheButton();
		updateProgressBar();
	}
}

void FalseColorDock::processSelectedColoring()
{
	lastColoring    = currentColoring;
	currentColoring = selectedColoring();

	//emit unsubscribeFalseColoring(this, lastShown);
	GGDBGM("requesting false color image " << selectedColoring() << endl);
	requestColoring(selectedColoring());
	updateTheButton();
	updateProgressBar();
}

void FalseColorDock::processApplyClicked()
{
	if (coloringState[selectedColoring()] == FalseColoringState::CALCULATING) {
		GGDBGM("enterState():" << endl);
		enterState(selectedColoring(), FalseColoringState::ABORTING);

		if (lastShown == selectedColoring()) {
			// Cancel after re-calc
			enterState(selectedColoring(), FalseColoringState::UNKNOWN);
			emit cancelComputation(selectedColoring());
			//emit unsubscribeFalseColoring(this, selectedColoring());
			updateProgressBar();
			updateTheButton();
		} else {
			//emit unsubscribeFalseColoring(this, selectedColoring());
			emit cancelComputation(selectedColoring());

			// go back to last shown coloring
			if (coloringUpToDate[lastShown]) {
				uisel->sourceBox->setCurrentIndex(int(lastShown));
				requestColoring(lastShown);
			} else { // or fall back to CMF, e.g. after ROI change
				uisel->sourceBox->setCurrentIndex(FalseColoring::CMF);
				requestColoring(FalseColoring::CMF);
			}
		}
	} else if (coloringState[selectedColoring()] == FalseColoringState::FINISHED ||
	           coloringState[selectedColoring()] == FalseColoringState::UNKNOWN) {
		requestColoring(selectedColoring(), /* recalc */ true);
	}
}

void FalseColorDock::initUi()
{
	// initialize scaled view
	view->installEventFilter(this); // needed for leave event
	view->init();
	scene = new ScaledView();
	view->setScene(scene);
	connect(scene, SIGNAL(newContentRect(QRect)),
	        view, SLOT(fitContentRect(QRect)));

	// initialize selection widget
	sel   = new AutohideWidget();
	uisel = new Ui::FalsecolorDockSelUI();
	uisel->setupUi(sel);
	scene->offTop = AutohideWidget::OutOffset;
	view->addWidget(AutohideWidget::TOP, sel);

	// setup source box
	uisel->sourceBox->setAHView(view);
	uisel->sourceBox->addItem(prettyFalseColorNames[FalseColoring::CMF],
	                          FalseColoring::CMF);
	uisel->sourceBox->addItem(prettyFalseColorNames[FalseColoring::PCA],
	                          FalseColoring::PCA);
	uisel->sourceBox->addItem(prettyFalseColorNames[FalseColoring::PCAGRAD],
	                          FalseColoring::PCAGRAD);

#ifdef WITH_SOM
	uisel->sourceBox->addItem(prettyFalseColorNames[FalseColoring::SOM],
	                          FalseColoring::SOM);
	uisel->sourceBox->addItem(prettyFalseColorNames[FalseColoring::SOMGRAD],
	                          FalseColoring::SOMGRAD);
#endif // WITH_SOM
	uisel->sourceBox->setCurrentIndex(0);

	updateTheButton();
	updateProgressBar();

	connect(scene, SIGNAL(newSizeHint(QSize)),
	        view, SLOT(updateSizeHint(QSize)));

	connect(scene, SIGNAL(updateScrolling(bool)),
	        view, SLOT(suppressScrolling(bool)));

	connect(scene, SIGNAL(requestCursor(Qt::CursorShape)),
	        view, SLOT(applyCursor(Qt::CursorShape)));

	connect(scene, SIGNAL(pixelOverlay(int, int)),
	        this, SIGNAL(pixelOverlay(int, int)));

	connect(scene, SIGNAL(requestSpecSim(int, int)),
	        this, SLOT(requestSpecSim(int, int)));

	connect(uisel->sourceBox, SIGNAL(currentIndexChanged(int)),
	        this, SLOT(processSelectedColoring()));

	connect(uisel->theButton, SIGNAL(clicked()),
	        this, SLOT(processApplyClicked()));

	connect(this, SIGNAL(visibilityChanged(bool)),
	        this, SLOT(processVisibilityChanged(bool)));

	connect(uisel->screenshotButton, SIGNAL(released()),
	        this, SLOT(screenshot()));

	// add similarity widget
	sw = new SimilarityWidget(view);
	scene->offBottom = AutohideWidget::OutOffset;
	view->addWidget(AutohideWidget::BOTTOM, sw);

	connect(sw->doneAction(), SIGNAL(triggered()),
	        this, SLOT(restoreFalseColorFunction()));
	connect(sw->doneAction(), SIGNAL(triggered()),
	        scene, SLOT(updateInputMode()));
	connect(sw->targetAction(), SIGNAL(triggered()),
	        scene, SLOT(updateInputMode()));
	scene->setActionTarget(sw->targetAction());
}

FalseColoring::Type FalseColorDock::selectedColoring()
{
	QComboBox *src    = uisel->sourceBox;
	QVariant  boxData = src->itemData(src->currentIndex());
	FalseColoring::Type coloringType = FalseColoring::Type(boxData.toInt());
	return coloringType;
}

void FalseColorDock::subscribeColoring(FalseColoring::Type coloringType)
{
	if (coloringState[lastColoring] != FalseColoringState::CALCULATING) {
		subs[lastColoring]->invalidate();
		//subs[lastColoring].reset(nullptr);
	}

	if (!subs[coloringType]->established()) {
		subs[coloringType]->establish();

//		QString dataSub = "falsecolor." + FalsecolorModel::coloringTypeToString(coloringType);
//		subs[coloringType].reset(
//		    DataRegister::subscribe(Dependency(dataSub, SubscriptionType::READ,
//		                                       AccessType::DEFERRED),
//		                            this, std::bind(&FalseColorDock::
//		                                            coloringUpdated, this)));


//		if (sub) {
//			if (sub->getDataId() == dataSub)
//				return;

//			sub.reset(nullptr);
//			//sub->deleteLater();
//		}
	}
}

void FalseColorDock::coloringUpdated(FalseColoring::Type coloringType)
{
	{
		Subscription *sub = subs[coloringType]->subscription();
		Subscription::Lock<QPixmap, FalseColoring::Type> lock(*sub);

		//FalseColoring::Type coloringType = *(lock.meta());
		QPixmap pix(*lock());

		qDebug() << "coloring updated" << FalsecolorModel::coloringTypeToString(coloringType);


		processFalseColoringUpdate(coloringType, pix);
	}

	if (coloringType != currentColoring) {
		subs[coloringType]->invalidate();
	}
}

void FalseColorDock::requestColoring(FalseColoring::Type coloringType, bool recalc)
{
	if (coloringState[coloringType] != FalseColoringState::FINISHED || recalc) {
		GGDBGM("enterState():" << endl);
		enterState(coloringType, FalseColoringState::CALCULATING);
		updateTheButton();
	}

	subscribeColoring(coloringType);

	if (recalc) {
		GGDBGM("recalc " << coloringType << endl);
		emit requestFalseColoring(coloringType, recalc);
		//emit subscribeFalseColoring(this, coloringType);
		emit falseColoringRecalcRequested(coloringType);
	} else {
		GGDBGM("subscribe " << coloringType << endl);
		//emit subscribeFalseColoring(this, coloringType);
		//emit requestFalseColoring(coloringType, recalc);
	}
	updateProgressBar();
	updateTheButton();
}

void FalseColorDock::updateProgressBar()
{
	if (coloringState[selectedColoring()] == FalseColoringState::CALCULATING) {
		int percent = coloringProgress[selectedColoring()];
		uisel->calcProgress->setValue(percent);
		uisel->calcProgress->setVisible(true);
		sel->adjust(); // grow accordingly
		// stay visible
		sel->scrollIn(true);
	} else if (coloringProgress[selectedColoring()] != 100) {
		enterState(selectedColoring(), FalseColoringState::CALCULATING);
		updateProgressBar();
	} else {
		//GGDBGM(selectedColoring() << " not CALCULATING" << endl);
		uisel->calcProgress->setValue(0);
		uisel->calcProgress->setVisible(false);
		sel->adjust(); // shrink accordingly
		// remove enforced visibility
		sel->scrollOut();
	}
}

void FalseColorDock::updateTheButton()
{
	switch (coloringState[selectedColoring()]) {
	case FalseColoringState::FINISHED:
		uisel->theButton->setIcon(QIcon::fromTheme("view-refresh"));
		uisel->theButton->setText("Recalculate");
		uisel->theButton->setToolTip("Run again with different initialization");
		uisel->theButton->setVisible(false);
		uisel->screenshotButton->setVisible(true);
		if (selectedColoring() == FalseColoring::SOM ||
		    selectedColoring() == FalseColoring::SOMGRAD) {
			uisel->theButton->setVisible(true);
		}
		break;
	case FalseColoringState::CALCULATING:
		uisel->theButton->setIcon(QIcon::fromTheme("process-stop"));
		uisel->theButton->setText("Cancel");
		uisel->theButton->setToolTip("Cancel current computation");
		uisel->theButton->setVisible(true);
		uisel->screenshotButton->setVisible(false);
		break;
	case FalseColoringState::ABORTING:
		uisel->theButton->setVisible(true);
		break;
	case FalseColoringState::UNKNOWN:
		uisel->theButton->setIcon(QIcon::fromTheme("view-refresh"));
		uisel->theButton->setText("Calculate");
		uisel->theButton->setToolTip("Start calculation");
		uisel->theButton->setVisible(true);
		break;
	default:
		throw std::runtime_error("FalseColorDock::updateTheButton(): bad coloringState");
		break;
	}
}

void FalseColorDock::enterState(FalseColoring::Type coloringType, FalseColoringState::Type state)
{
	GGDBGM(coloringType << " entering state " << state << endl);
	coloringState[coloringType] = state;

	if (coloringType == selectedColoring()
	    && state == FalseColoringState::CALCULATING) {
		//draw wait message
		scene->setPixmap(QPixmap(0, 0));
	}
}

void FalseColorDock::processVisibilityChanged(bool visible)
{
	//GGDBG_CALL();
	if (visible) {
		requestColoring(selectedColoring());
	} else {
		//emit unsubscribeFalseColoring(this, selectedColoring());
	}
}


void FalseColorDock::processCalculationProgressChanged(FalseColoring::Type coloringType,
                                                       int                 percent)
{
	//qDebug() << FalsecolorModel::coloringTypeToString(coloringType) << percent;
	coloringProgress[coloringType] = percent;

	if (coloringState[coloringType] != FalseColoringState::CALCULATING) {
		enterState(coloringType, FalseColoringState::CALCULATING);
		updateTheButton();
	}

	if (coloringType == selectedColoring())
		updateProgressBar();
}

void FalseColorDock::screenshot()
{
	QImage   img    = scene->getPixmap().toImage();
	cv::Mat  output = QImage2Mat(img);
	GerbilIO io(this, "False-coloring File", "false-color image");
	io.setFileSuffix(".png");
	io.setFileCategory("Screenshot");
	io.writeImage(output);
}

bool FalseColorDock::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::Leave) {
		scene->leaveEvent();
	}

	// continue with standard event processing
	return QObject::eventFilter(obj, event);
}

void FalseColorDock::requestSpecSim(int x, int y)
{
	similarity_measures::SMConfig conf = sw->config();
	emit requestComputeSpecSim(x, y, conf);
}

void FalseColorDock::processSpecSimUpdate(QPixmap result)
{
	scene->setPixmap(result);
	scene->update();
	sw->doneAction()->setEnabled(true);
	sw->targetAction()->setEnabled(true);
}

void FalseColorDock::restoreFalseColorFunction()
{
	sw->targetAction()->setEnabled(true);
	sw->doneAction()->setEnabled(false);
	processSelectedColoring();
}

void FalseColorDock::saveState()
{
	QSettings settings;
	QComboBox *src = uisel->sourceBox;
	settings.setValue("FalseColorDock/selectedColoringIndex", src->currentIndex());
	// TODO: specsim state?
}

void FalseColorDock::restoreState()
{
	QSettings settings;
	auto      selectedColoring = settings.value("FalseColorDock/selectedColoringIndex", 0);
	// TODO: specsim state?

	QComboBox *src = uisel->sourceBox;
	src->setCurrentIndex(selectedColoring.toInt());
	processSelectedColoring();
}
