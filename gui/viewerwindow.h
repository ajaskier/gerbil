#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include "ui_viewerwindow.h"
#include <multi_img.h>
#include <illuminant.h>

#include <vector>
#include <QPixmap>
#include <QImage>
#include <QVector>
#include <QMenu>
#include <cv.h>

class ViewerWindow : public QMainWindow, private Ui::ViewerWindow {
    Q_OBJECT
public:
	ViewerWindow(multi_img *image, multi_img *gradient, QWidget *parent = 0);
	~ViewerWindow();

	const QPixmap* getBand(int dim, bool gradient);
	const inline Illuminant & getIlluminant(int temp);
	const inline std::vector<multi_img::Value> & getIlluminantC(int temp);

	static QIcon colorIcon(const QColor& color);

public slots:
	void reshapeDock(bool floating);
	void selectBand(int dim, bool gradient);
	void addToLabel()   { labelmask(false); }
	void remFromLabel() { labelmask(true); }
	void setActive(bool gradient);
	void newOverlay();
	void startGraphseg();
	void applyIlluminant();
	void setI1(int index);
	void setI1Visible(bool);

signals:
	void alterLabel(const multi_img::Mask &mask, bool negative);
	void drawOverlay(const multi_img::Mask &mask);

protected:
    void changeEvent(QEvent *e);

	// helper functions
	void createMarkers();
	void labelmask(bool negative);

	// multispectral image and gradient
	multi_img *image, *image_orig, *gradient;
	// bands from both image and gradient
	std::vector<QPixmap*> ibands, gbands;
	// pixel label holder
	cv::Mat_<uchar> labels;
	// label colors
	QVector<QColor> labelColors;

	// rgb pixmap
	QPixmap rgb;
	int activeViewer; // 0: IMG, 1: GRAD

private:
	void init();
	void initGraphsegUI();
	void initIlluminantUI();
	void updateRGB();
	void buildIlluminant(int temp);

	// cache for illumination coefficients
	typedef std::map<int, std::pair<
			Illuminant, std::vector<multi_img::Value> > > Illum_map;
	Illum_map illuminants;
};

#endif // VIEWERWINDOW_H