/*
	Copyright(c) 2010 Johannes Jordan <johannes.jordan@cs.fau.de>.
	Copyright(c) 2012 Petr Koupy <petr.koupy@gmail.com>

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#include "viewport2.h"
#include <app/gerbilio.h>

#define GGDBG_MODULE
//#include "../gerbil_gui_debug.h"

#include <qtopencv.h>
#include <stopwatch.h>

#include <iostream>
#include <QApplication>
#include <QMessageBox>
#include <QGraphicsProxyWidget>
#include <QGraphicsItem>
#include <QPainter>
#include <QSignalMapper>
#include <QAction>
#include <QDebug>
#include <QSettings>
#include <boost/format.hpp>

#include "subscription.h"
#include "dependency.h"
#include "lock.h"
#include "data_register.h"


Viewport2::Viewport2(representation::t type, QGLWidget *target)
    : type(type), target(target), width(0), height(0),
      //ctx(new SharedData<ViewportCtx>(new ViewportCtx())),
      //sets(new SharedData<std::vector<BinSet> >(new std::vector<BinSet>())),
      selection(0), hover(-1), limiterMode(false),
      active(false), useralpha(1.f),
      showLabeled(true), showUnlabeled(true),
      overlayMode(false),
      illuminant_show(true),
      zoom(1.), holdSelection(false), activeLimiter(0),
      drawLog(nullptr), drawMeans(nullptr), drawRGB(nullptr), drawHQ(nullptr),
      bufferFormat(BufferFormat::RGBA16F),
      drawingState(HIGH_QUALITY), yaxisWidth(0), vb(QGLBuffer::VertexBuffer)
{
    //(*ctx)->wait = 1;
    //(*ctx)->reset = 1;
    //(*ctx)->ignoreLabels = false;

	restoreState();
	initTimers();

	connect(QApplication::instance(), SIGNAL(lastWindowClosed()),
	        this, SLOT(saveState()));

    sub = DataRegister::subscribe(Dependency("dist.IMG", SubscriptionType::READ,
                                                 AccessType::DEFERRED), this,
                                                 std::bind(&Viewport2::rebuild, this));
}

Viewport2::~Viewport2()
{
	for (int i = 0; i < 2; ++i) {
		delete buffers[i].fbo;
		delete buffers[i].blit;
	}

    delete sub;
}

/********* I N I T **************/

void Viewport2::initTimers()
{
	resizeTimer.setSingleShot(true);
	connect(&resizeTimer, SIGNAL(timeout()), this, SLOT(resizeScene()));

	QSignalMapper *mapper = new QSignalMapper();
	for (int i = 0; i < 2; ++i) {
		buffers[i].renderedLines = 0;
		buffers[i].renderTimer.setSingleShot(true);
		connect(&buffers[i].renderTimer, SIGNAL(timeout()),
		        mapper, SLOT(map()));
		mapper->setMapping(&buffers[i].renderTimer, i);
	}
	connect(mapper, SIGNAL(mapped(int)),
	        this, SLOT(continueDrawing(int)));
}

void Viewport2::initBuffers()
{
	/* (re)set framebuffers */
	target->makeCurrent();

	if (!tryInitBuffers()) {
		// tryInitBuffers() fails if the buffer format is not supported by the
		// GL driver. RGBA8 is the default and should be supported by all
		// drivers.

		/* debug output */
		const char* bfs;
		switch (bufferFormat) {
		case BufferFormat::RGBA8  : bfs = "RGBA8"; break;
		case BufferFormat::RGBA16F: bfs = "RGBA16F"; break;
		case BufferFormat::RGBA32F: bfs = "RGBA32F"; break;
		default:
			bfs = "UNKNOWN_BUFFER_FORMAT"; break;
		}
        std::cerr << "Viewport2: Warning: Failed to init framebuffer "
		          << "with format " << bfs << ". "
		          << "Falling back to RGBA8. Drawing quality reduced."
		          << std::endl;

		/* try fallback */
		bufferFormat = BufferFormat::RGBA8;
		// try again
		if (!tryInitBuffers()) {
            std::cerr << "Viewport2: Error: Failed to init framebuffer "
			          << "with format RGBA8. Aborting."
			          << std::endl;
			std::abort();
		}
	}
}

bool Viewport2::tryInitBuffers()
{
	for (int i = 0; i < 2; ++i) {
		// first: formats
		QGLFramebufferObjectFormat format_buf, format_blit;

		/* use floating point for better alpha accuracy in back buffer! */
		if (i == 0)
			format_blit.setInternalTextureFormat((GLenum)bufferFormat);

		// strict: format_buf must be format_blit + multisampling! OGL spec!
		format_buf = format_blit;

		/* multisampling. 0 deactivates. 2 or 4 should be reasonable values.
		 * TODO: make this configurable and/or test with cheap GPUs */
		format_buf.setSamples(4);

		// second: buffer
		delete buffers[i].fbo;
		buffers[i].fbo = new QGLFramebufferObject(width, height, format_buf);
        if(!buffers[i].fbo->isValid()) {
			return false;
		}
        buffers[i].dirty = true;

		// third: blit buffer
		delete buffers[i].blit;
		buffers[i].blit = new QGLFramebufferObject(width, height, format_blit);
		if(!buffers[i].blit->isValid()) {
			return false;
		}
	}
	return true;
}

/********* S T A T E ********/

void Viewport2::drawBackground(QPainter *painter, const QRectF &rect)
{
	// update geometry
	int nwidth = painter->device()->width();
	int nheight = painter->device()->height();
	if (nwidth != width || nheight != height) {
		width = nwidth;
		height = nheight;
		zoom = 1.f;

		updateYAxis();
		// update transformation (needed already for legend, axes drawing)
		updateModelview();

		// defer buffer update (do not hinder resizing with slow update)
		buffers[0].dirty = buffers[1].dirty = true;
		resizeTimer.start(150);
	}

	// draw
	drawScene(painter);
}

void Viewport2::resizeScene()
{
	// initialize buffers with new size
	initBuffers();

	// update buffers
	updateBuffers();
}

void Viewport2::reset()
{
	// reset values that would become inappropr.
	selection = 0;
	hover = -1;

	// reset limiters to most-lazy values
	setLimiters(0);

	// update y-axis (used by updateModelView())
	updateYAxis();

	// update coordinate system
	updateModelview(true);
}

void Viewport2::rebuild()
{
	// guess: we want the lock to carry over both methods..
//	SharedDataLock ctxlock(ctx->mutex);
//	SharedDataLock setslock(sets->mutex);

    qDebug() << "\nREBUILDDDDDDD\n";

	prepareLines(); // will also call reset() if indicated by ctx
	updateBuffers();
}

void Viewport2::prepareLines()
{
//	if (!ctx) {
//		GGDBGM("no ctx" << endl);
//		return;
//	}
//	if (!sets) {
//		GGDBGM("no sets" << endl);
//		return;
//	}

	// lock context and sets
    //SharedDataLock ctxlock(ctx->mutex);
    //SharedDataLock setslock(sets->mutex);

    Subscription::Lock<std::vector<BinSet>, ViewportCtx> lock(*sub);
    std::vector<BinSet>* sets = lock();
    ViewportCtx* ctx = lock.meta();

    ctx->wait.fetch_and_store(0); // set to zero: our data will be usable
    if (ctx->reset.fetch_and_store(0)) // is true if it was 1 before
		reset();

	// first step (cpu only)
    Compute::preparePolylines(*ctx, *sets, shuffleIdx);

	// second step (cpu -> gpu)
	target->makeCurrent();
    Compute::storeVertices(*ctx, *sets, shuffleIdx, vb,
	                       drawMeans->isChecked(), illuminantAppl);

}

void Viewport2::activate()
{
	if (!active) {
		active = true;
		emit activated(type);
		emit bandSelected(selection);

		// this is a sometimes redundant operation (matrix maybe still valid)
		if (limiterMode)
			emit requestOverlay(limiters, -1);
		else
			emit requestOverlay(selection, hover);
	}
}

void Viewport2::removePixelOverlay()
{
	overlayMode = false;
	update();
}

void Viewport2::insertPixelOverlay(const QPolygonF &points)
{
	overlayPoints = points;
	overlayMode = true;
	update();
}

void Viewport2::changeIlluminantCurve(QVector<multi_img::Value> illum)
{
	illuminantCurve = illum;
	if (illuminant_show)
		update();
}

void Viewport2::setIlluminationCurveShown(bool show)
{
	illuminant_show = show;
	update();
}

void Viewport2::setAppliedIlluminant(QVector<multi_img_base::Value> illum)
{
	//bool change = (applied != illuminant_apply);
	illuminantAppl = illum.toStdVector();
	/*	if (change) TODO: I assume this is already triggered by invalidated ROI
		rebuild();*/
}

void Viewport2::setLimiters(int label)
{
    Subscription::Lock<std::vector<BinSet>, ViewportCtx> lock(*sub);
    std::vector<BinSet>* sets = lock();
    ViewportCtx* ctx = lock.meta();

	if (label < 1) {	// not label
        //SharedDataLock ctxlock(ctx->mutex);


        limiters.assign(ctx->dimensionality,
                        std::make_pair(0, ctx->nbins-1));
		if (label == -1 && hover >= 0) {	// use hover data
			int b = selection;
			int h = hover;
			limiters[b] = std::make_pair(h, h);
		}
	} else {            // label holds data
        //SharedDataLock setslock(sets->mutex);
        if ((int)sets->size() > label && (*sets)[label].totalweight > 0) {
			// use range from this label
			const std::vector<std::pair<int, int> > &b =
                    (*sets)[label].boundary;
			limiters.assign(b.begin(), b.end());
		} else {
			setLimiters(0);
		}
	}
}

void Viewport2::toggleLabelHighlight(int index)
{
	if (highlightLabels.contains(index)) {
		int pos = highlightLabels.indexOf(index);
		highlightLabels.remove(pos, 1);
	}
	else {
		highlightLabels.push_back(index);
	}

    updateBuffers(Viewport2::RM_STEP,
	              (!highlightLabels.empty() ? RM_SKIP: RM_STEP));
}

void Viewport2::setAlpha(float alpha)
{
	useralpha = alpha;
    updateBuffers(Viewport2::RM_STEP, Viewport2::RM_SKIP);
}

void Viewport2::setLimitersMode(bool enabled)
{
	limiterMode = enabled;
    updateBuffers(Viewport2::RM_SKIP, Viewport2::RM_STEP);
	if (!active) {
		activate(); // will call requestOverlay()
	} else {
		// this is a sometimes redundant operation (matrix maybe still valid)
		if (limiterMode)
			emit requestOverlay(limiters, -1);
		else
			emit requestOverlay(selection, hover);
	}
}

void Viewport2::startNoHQ()
{
	drawingState = (drawingState == HIGH_QUALITY ? HIGH_QUALITY_QUICK : QUICK);
}

bool Viewport2::endNoHQ(RenderMode spectrum, RenderMode highlight)
{
	/* we enter this function after the display was in temporary low quality
	 * state. it may be "dirty", i.e. need to be redrawn in high quality now
	 * that is, if the global setting is not low quality anyways
	 */
	bool dirty = drawHQ->isChecked();
	if (drawingState == HIGH_QUALITY || drawingState == HIGH_QUALITY_QUICK)
		dirty = false;

	drawingState = (drawHQ->isChecked() ? HIGH_QUALITY : QUICK);
	if (dirty)
		updateBuffers(spectrum, highlight);
	return dirty;
}

bool Viewport2::updateLimiter(int dim, int bin)
{
	std::pair<int, int> &l = limiters[dim];
	int *target;
	if (l.first == l.second) { // both top and bottom are same
		target = (bin > l.first ? &l.second : &l.first);
	} else if (activeLimiter) {
		target = activeLimiter;
	} else { // choose closest between top and bottom
		target = (std::abs(l.first-bin) < std::abs(l.second-bin) ?
		              &l.first : &l.second);
	}
	if (*target == bin) // no change
		return false;

	*target = bin;
	activeLimiter = target;
	return true;
}

void Viewport2::toggleLabeled(bool enabled)
{
	showLabeled = enabled;
	updateBuffers();
}

void Viewport2::toggleUnlabeled(bool enabled)
{
	showUnlabeled = enabled;
	updateBuffers();
}

void Viewport2::screenshot()
{
	// ensure high quality
	drawingState = HIGH_QUALITY;
	updateBuffers(RM_FULL, RM_FULL);

	// render into our buffer
	QGLFramebufferObject b(width, height);
	QPainter p(&b);
	bool success = drawScene(&p, false);

	// reset drawing state
	endNoHQ();

	if (!success)
		return;

	QImage img = b.toImage();

	// write out
	cv::Mat output = QImage2Mat(img);
	GerbilIO io(target, "Screenshot File", "screenshot");
	io.setFileSuffix(".png");
	io.setFileCategory("Screenshot");
	io.writeImage(output);
}

void Viewport2::adjustBoundaries()
{
    Subscription::Lock<std::vector<BinSet>, ViewportCtx> lock(*sub);
    //std::vector<BinSet>* sets = lock();
    ViewportCtx* ctx = lock.meta();

	QPointF empty(0.f, 0.f);
	empty = modelview.map(empty);

	QPointF leftbound = empty;
	leftbound.setX(0.f);
	QPointF lb = modelview.map(leftbound);

	QPointF rightbound = empty;
//	SharedDataLock ctxlock(ctx->mutex);
    rightbound.setX(ctx->dimensionality - 1);
	QPointF rb = modelview.map(rightbound);

	QPointF bottombound = empty;
	bottombound.setY(0.f);
	QPointF bb = modelview.map(bottombound);

	QPointF topbound = empty;
    topbound.setY((float)(ctx->nbins));
	QPointF tb = modelview.map(topbound);

	qreal lbpos = yaxisWidth + 25;
	qreal rbpos = width - 15;
	qreal bbpos = height - boundaries.vp - boundaries.vtp;
	qreal tbpos = boundaries.vp;

	//    qDebug() << "LEFT BOUND " << lb
	//             << "RIGHT BOUND " << rb
	//             << "BOTTOM BOUND " << bb
	//             << "TOP BOUND " << tb;

	qreal xp = 0;
	qreal yp = 0;

	if (lb.x() > lbpos) {
		//qDebug() << "LEFT BOUND IS VISIBLE!";
		QPointF topleft(lbpos, 0.f);
		topleft = modelviewI.map(topleft);

		xp = topleft.x();
	} else if (rb.x() < rbpos) {
		//qDebug() << "RIGHT BOUND IS VISIBLE!";
		QPointF right(rbpos, 0.f);
		right = modelviewI.map(right);
		rb = modelviewI.map(rb);

		xp = right.x()-rb.x();
	}

	if (bb.y() < bbpos) {
		//qDebug() << "BOTTOM BOUND IS VISIBLE!";
		QPointF bottom(0.f, bbpos);
		bottom = modelviewI.map(bottom);
		bb = modelviewI.map(bb);

		yp = bottom.y()-bb.y();

	} else if (tb.y() > tbpos) {
		//qDebug() << "TOP BOUND IS VISIBLE!";
		QPointF top(0, tbpos);
		top = modelviewI.map(top);
		tb = modelviewI.map(tb);

		yp = top.y()-tb.y();
	}

	modelview.translate(xp, yp);
	modelviewI = modelview.inverted();
}

void Viewport2::toggleHQ()
{
	if (drawHQ->isChecked()) {
		// triggers drawing update
		endNoHQ();
	} else {
		startNoHQ();
		// deliberately make display worse for user to see effect
		updateBuffers();
	}
}

void Viewport2::setBufferFormat(BufferFormat format, bool propagate)
{
	bufferFormat = format;

	// initialize buffers with new format
	initBuffers();
	updateBuffers();

	// propagate change
	if (propagate)
		emit bufferFormatToggled(bufferFormat);
}

void Viewport2::toggleBufferFormat()
{
	BufferFormat format = BufferFormat::RGBA8;
	if (bufferFormat == BufferFormat::RGBA8)
		format = BufferFormat::RGBA16F;
	if (bufferFormat == BufferFormat::RGBA16F)
		format = BufferFormat::RGBA32F;
	setBufferFormat(format, true);
}

void Viewport2::saveState()
{
	QSettings settings;
	settings.beginGroup("DistView_" + representation::str(type));
	/* QVariant can only serialize GLenum, not BufferFormat */
	settings.setValue("BufferFormat", (GLenum)bufferFormat);
	settings.endGroup();
}

void Viewport2::restoreState()
{
	QSettings settings;
	settings.beginGroup("DistView_" + representation::str(type));
	auto format = settings.value("BufferFormat", (GLenum)bufferFormat);
	settings.endGroup();

	bufferFormat = (BufferFormat)format.value<GLenum>();
}
