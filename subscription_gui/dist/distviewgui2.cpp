#include "distviewgui2.h"
#include "widgets/autohidewidget.h"

#include <QSettings>
#include <QDebug>

//#define GGDBG_MODULE
//#include <gerbil_gui_debug.h>

DistViewGUI2::DistViewGUI2(representation::t type)
	: type(type)
{	
	// setup frame and its UI
    frame = new QWidget(this);
	frame->setSizePolicy(QSizePolicy::Preferred, // hor
						 QSizePolicy::Expanding); // ver
    ui = new Ui::DistViewGUI2();
	ui->setupUi(frame);

	connect(QApplication::instance(), SIGNAL(lastWindowClosed()),
	        this, SLOT(saveState()));

	// create viewport
    qDebug() << "about to create viewport";
	initVP();
    qDebug() << "viewport created";

	// create controller widget that will reside inside viewport
	initVC(type);

	// connect and initialize topBar
	initTop();

	restoreState();
}

DistViewGUI2::~DistViewGUI2()
{
    vp->deleteLater();
}

void DistViewGUI2::initVP()
{
	// create target widget
	QGLWidget *target = ui->gv->init();

	// create viewport. The viewport is a GraphicsScene
    vp = new Viewport2(type, target);
	ui->gv->setScene(vp);
}

void DistViewGUI2::initVPActions()
{
	ui->gv->addAction(uivc->actionHq);
	uivc->hqButton->setAction(uivc->actionHq);
	vp->setDrawHQ(uivc->actionHq);
	connect(uivc->actionHq, SIGNAL(triggered()), vp, SLOT(toggleHQ()));

	ui->gv->addAction(uivc->actionLog);
	uivc->logButton->setAction(uivc->actionLog);
	vp->setDrawLog(uivc->actionLog);
	connect(uivc->actionLog, SIGNAL(triggered()), vp, SLOT(updateBuffers()));

	ui->gv->addAction(uivc->actionScr);
	uivc->screenshotButton->setAction(uivc->actionScr);
	connect(uivc->actionScr, SIGNAL(triggered()), vp, SLOT(screenshot()));

	ui->gv->addAction(uivc->actionBuff);
	connect(uivc->actionBuff, SIGNAL(triggered()), vp, SLOT(toggleBufferFormat()));
    connect(vp, SIGNAL(bufferFormatToggled(Viewport2::BufferFormat)),
            this, SLOT(updateBufferFormat(Viewport2::BufferFormat)));

	ui->gv->addAction(uivc->actionRgb);
	uivc->rgbButton->setAction(uivc->actionRgb);
	vp->setDrawRGB(uivc->actionRgb);
	connect(uivc->actionRgb, SIGNAL(triggered()), vp, SLOT(updateBuffers()));

	ui->gv->addAction(uivc->actionMeans);
	vp->setDrawMeans(uivc->actionMeans);
	connect(uivc->actionMeans, SIGNAL(triggered()), vp, SLOT(rebuild()));
}

void DistViewGUI2::initVC(representation::t type)
{
	/* create VC, apply UI to it, then add to GV */
	vc = new AutohideWidget();
	uivc = new Ui::ViewportControl();
	uivc->setupUi(vc);
	ui->gv->addWidget(AutohideWidget::LEFT, vc);
	// small hack to enable proximity trigger for said autohide widget
	ui->gv->fitContentRect(QRect(20, 0, 0, 0)); //!! change when adding widgets!

	/* we are ready to connect signals/slots */

	// let user see how many bins he will end up with
	connect(uivc->binSlider, SIGNAL(sliderMoved(int)),
	        this, SLOT(setBinLabel(int)));
	connect(uivc->binSlider, SIGNAL(valueChanged(int)),
	        this, SLOT(setBinCount(int)));
	connect(uivc->alphaSlider, SIGNAL(valueChanged(int)),
	        this, SLOT(setAlpha(int)));

	// low quality drawing while user works the slider
	connect(uivc->alphaSlider, SIGNAL(sliderPressed()),
	        vp, SLOT(startNoHQ()));
	connect(uivc->alphaSlider, SIGNAL(sliderReleased()),
	        vp, SLOT(endNoHQ()));

	connect(uivc->limiterButton, SIGNAL(toggled(bool)),
	        vp, SLOT(setLimitersMode(bool)));
	connect(uivc->limiterMenuButton, SIGNAL(clicked()),
	        this, SLOT(showLimiterMenu()));

	connect(uivc->formatButton, SIGNAL(released()),
	        this, SLOT(showFrameBufferMenu()));

	// default UI stuff
	if (type != representation::IMG)
		uivc->rgbButton->setVisible(false);

	setAlpha(uivc->alphaSlider->value());
	setBinCount(uivc->binSlider->value());

	createFrameBufferMenu();
	// initialize from VP as we only implicitely store it
	updateBufferFormat(vp->format());

	initVPActions();
}

void DistViewGUI2::initTop()
{
	// connect toggling trigger
	connect(ui->topBar, SIGNAL(toggleFold()),
	        this, SLOT(toggleFold()));

	// setup title in topBar
	setTitle(type);
}

void DistViewGUI2::initSignals(QObject *dvctrl)
{
	// signals from DistviewController
	connect(dvctrl, SIGNAL(pixelOverlayInvalid()),
	        vp, SLOT(removePixelOverlay()));
	connect(dvctrl, SIGNAL(toggleLabeled(bool)),
	        vp, SLOT(toggleLabeled(bool)));
	connect(dvctrl, SIGNAL(toggleUnlabeled(bool)),
	        vp, SLOT(toggleUnlabeled(bool)));

	connect(dvctrl, SIGNAL(labelSelected(int)),
	        vp, SLOT(toggleLabelHighlight(int)));

	// signals to controller
	connect(this, SIGNAL(requestBinCount(representation::t, int)),
	        dvctrl, SLOT(changeBinCount(representation::t, int)));


	//   viewport action
	connect(vp, SIGNAL(activated(representation::t)),
	        dvctrl, SLOT(setActiveViewer(representation::t)));
	connect(vp, SIGNAL(activated(representation::t)),
	        this, SIGNAL(activated()));
	connect(vp, SIGNAL(bandSelected(int)),
	        dvctrl, SLOT(propagateBandSelection(int)));
	connect(vp, SIGNAL(requestOverlay(int,int)),
	        dvctrl, SLOT(drawOverlay(int,int)));
	connect(vp, SIGNAL(requestOverlay(std::vector<std::pair<int,int> >,int)),
	        dvctrl, SLOT(drawOverlay(std::vector<std::pair<int,int> >,int)));

	connect(vp, SIGNAL(addSelectionRequested()),
	        dvctrl, SLOT(addHighlightToLabel()));
	connect(vp, SIGNAL(remSelectionRequested()),
	        dvctrl, SLOT(remHighlightFromLabel()));

	// illumination correction
	connect(this, SIGNAL(newIlluminantCurve(QVector<multi_img::Value>)),
	        vp, SLOT(changeIlluminantCurve(QVector<multi_img::Value>)));
	connect(this, SIGNAL(toggleIlluminationShown(bool)),
	        vp, SLOT(setIlluminationCurveShown(bool)));
	connect(this, SIGNAL(newIlluminantApplied(QVector<multi_img::Value>)),
	        vp, SLOT(setAppliedIlluminant(QVector<multi_img::Value>)));
}

void DistViewGUI2::initSubscriptions()
{
	if (!ui->gv->isHidden()) {
		emit needBinning(type);
		emit subscribeRepresentation(this, type);
	}
}

void DistViewGUI2::setEnabled(bool enabled)
{
	// TODO: maybe do this on frame
	ui->gv->setEnabled(enabled);
}

void DistViewGUI2::fold(bool folded)
{
	if (folded) { // fold
//		GGDBGM(type << " folding" << endl);

		// let the controller know we do not need our image representation
		emit unsubscribeRepresentation(this, type);

		// fold GUI and set size policy for proper arrangement
		ui->gv->setHidden(true);
		ui->topBar->fold();
		frame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

		// reset title
		setTitle(type);

		// TODO: send signals that will destroy unused data!

		// TODO: let viewport clean itself up!
		//(*binsets)->clear(); // clear the vector, not re-create shareddata!
		//viewport->shuffleIdx.clear();
		//viewport->vb.destroy();
		emit foldingStateChanged(type, true);

	} else { // unfold
//		GGDBGM(type << " unfolding" << endl);

		emit needBinning(type);
		// let the controller know we need our image representation
		emit subscribeRepresentation(this, type);

		// unfold GUI and set size policy for proper arrangement
		ui->gv->setVisible(true);
		ui->topBar->unfold();
		QSizePolicy pol(QSizePolicy::Preferred, QSizePolicy::Expanding);
		pol.setVerticalStretch(1);
		frame->setSizePolicy(pol);

		emit foldingStateChanged(type, false);

		// TODO: trigger calculation of data?
	}
}

void DistViewGUI2::toggleFold()
{
	fold(ui->gv->isHidden() ? false : true);
}

void DistViewGUI2::setTitle(representation::t type)
{
	QString title = QString("<b>%1</b>").arg(representation::prettyString(type));
	ui->topBar->setTitle(title);
}

void DistViewGUI2::setTitle(representation::t type,
                           multi_img::Value min, multi_img::Value max)
{
	QString title = QString("<b>%1</b>").arg(representation::prettyString(type));
	title = title.append(" [%1..%2]")
	        .arg(min, 0, 'f', 2).arg(max, 0, 'f', 2);
	ui->topBar->setTitle(title);
}

void DistViewGUI2::setAlpha(int alpha)
{
	float realalpha = (float)alpha/100.f;
	uivc->alphaLabel->setText(QString::fromUtf8("α: %1")
	                          .arg(realalpha, 0, 'f', 2));

	vp->setAlpha(realalpha);
}

void DistViewGUI2::setBinLabel(int n)
{
	uivc->binLabel->setText(QString("%1 bins").arg(n));
}

void DistViewGUI2::setBinCount(int n)
{
	setBinLabel(n);
	emit requestBinCount(type, n);
}

void DistViewGUI2::updateLabelColors(QVector<QColor> colors)
{
	labelColors = colors;

	/* we need to rebuild label color values
	 * initial menu creation is implicitely done as updateLabelColors is called
	 * at startup */
	createLimiterMenu();
}

void DistViewGUI2::createLimiterMenu()
{
	limiterMenu.clear();
	QAction *tmp;
	tmp = limiterMenu.addAction("No limits");
	tmp->setData(0);
	tmp = limiterMenu.addAction("Limit from current highlight");
	tmp->setData(-1);
	limiterMenu.addSeparator();
	for (int i = 1; i < labelColors.size(); ++i) {
		tmp = limiterMenu.addAction(colorIcon(labelColors.at(i)),
		                            "Limit by label");
		tmp->setData(i);
	}
}

void DistViewGUI2::showLimiterMenu()
{
	// map to scene coordinates
#ifdef _WIN32 // mapToGlobal() doesn't work correctly
	auto screenpoint = QCursor::pos();
#else
	auto screenpoint = uivc->limiterMenuButton->mapToGlobal(QPoint(0, 0));
#ifndef QT_BROKEN_MAPTOGLOBAL
	screenpoint = ui->gv->mapToGlobal(screenpoint);
#endif
#endif

	QAction *a = limiterMenu.exec(screenpoint);
	if (!a)
		return;

	int choice = a->data().toInt(); assert(choice < labelColors.size());
	vp->setLimiters(choice);
	if (!uivc->limiterButton->isChecked()) {
		uivc->limiterButton->toggle();	// changes button state AND viewport
	} else {
		vp->setLimitersMode(true);	// only viewport
	}
}

void DistViewGUI2::createFrameBufferMenu()
{
	QActionGroup *actionGroup = new QActionGroup(this);
	actionGroup->setExclusive(true);

	QAction* tmp;
	tmp = frameBufferMenu.addAction("RGBA8");
	tmp->setCheckable(true);
    tmp->setData(QVariant::fromValue(Viewport2::BufferFormat::RGBA8));
	actionGroup->addAction(tmp);

	tmp = frameBufferMenu.addAction("RGBA16F");
	tmp->setCheckable(true);
    tmp->setData(QVariant::fromValue(Viewport2::BufferFormat::RGBA16F));
	actionGroup->addAction(tmp);

	tmp = frameBufferMenu.addAction("RGBA32F");
	tmp->setCheckable(true);
    tmp->setData(QVariant::fromValue(Viewport2::BufferFormat::RGBA32F));
	actionGroup->addAction(tmp);
}

void DistViewGUI2::showFrameBufferMenu()
{
	// map to scene coordinates
#ifdef _WIN32 // mapToGlobal() doesn't work correctly
	auto screenpoint = QCursor::pos();
#else
	auto screenpoint = uivc->formatButton->mapToGlobal(QPoint(0, 0));
#ifndef QT_BROKEN_MAPTOGLOBAL
	screenpoint = ui->gv->mapToGlobal(screenpoint);
#endif
#endif

	QAction *a = frameBufferMenu.exec(screenpoint);
	if (!a)
		return;

    Viewport2::BufferFormat choice = a->data().value<Viewport2::BufferFormat>();
	vp->setBufferFormat(choice);
}

void DistViewGUI2::insertPixelOverlay(const QPolygonF &points)
{
	vp->insertPixelOverlay(points);
}

/** Return a 32x32px icon filled with color. */
QIcon DistViewGUI2::colorIcon(const QColor &color)
{
	QPixmap pm(32, 32);
	pm.fill(color);
	return QIcon(pm);
}

bool DistViewGUI2::isVisible()
{
	return !ui->gv->isHidden();
}

void DistViewGUI2::updateBufferFormat(Viewport2::BufferFormat format)
{
	QList<QAction*> list = frameBufferMenu.actions();
	for (QAction* act : list) {
        if (act->data().value<Viewport2::BufferFormat>() == format) {
			act->setChecked(true);
			return;
		}
	}
}

void DistViewGUI2::saveState()
{
	QSettings settings;
	settings.beginGroup("DistView_" + representation::str(type));
	settings.setValue("HQDrawing", uivc->actionHq->isChecked());
	settings.setValue("LogDrawing",uivc->actionLog->isChecked());
	settings.setValue("alphaModifier", uivc->alphaSlider->value());
	settings.setValue("NBins", uivc->binSlider->value());
	settings.endGroup();
}

void DistViewGUI2::restoreState()
{
	QSettings settings;

	settings.beginGroup("DistView_" + representation::str(type));
	auto hq = settings.value("HQDrawing", true);
	auto log = settings.value("LogDrawing", false);
	auto alpha = settings.value("alphaModifier", 50);
	auto nbins = settings.value("NBins", 64);
	settings.endGroup();

	uivc->actionHq->setChecked(hq.toBool());
	uivc->actionLog->setChecked(log.toBool());
	uivc->alphaSlider->setValue(alpha.toInt());
	uivc->binSlider->setValue(nbins.toInt());
}
