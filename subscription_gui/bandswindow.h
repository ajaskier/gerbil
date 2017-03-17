#ifndef BANDSWINDOW_H
#define BANDSWINDOW_H

#include <QWidget>
#include <subscription_manager.h>

#include "multi_img.h"
#include "model/representation.h"

namespace Ui {
class BandsWindow;
}

class BandsWindow : public QWidget
{
	Q_OBJECT

public:
	explicit BandsWindow(QWidget *parent = 0);
	~BandsWindow();

	void setMax(int max);

private:
	Ui::BandsWindow *ui;
	Subscription* bandsSub;

	QString representation = "IMG";
	int currentBand        = 0;
	size_t maxBands        = 0;

	multi_img::NormMode current_normMode;
	multi_img_base::Range current_targetRange;

signals:
	void normalizationParametersChanged(
		representation::t   type,
		multi_img::NormMode normMode,
		multi_img::Range    targetRange
		);

public slots:
	void onNormalizationParametersChanged(representation::t     type,
	                                      multi_img::NormMode   normMode,
	                                      multi_img_base::Range targetRange);


private slots:
	void display();

	void on_imageModelButton_clicked();
	void on_imgButton_clicked();
	void on_normButton_clicked();
	void on_gradButton_clicked();
	void on_pcaButton_clicked();
	void on_gradpcaButton_clicked();
	void on_bandSlider_sliderMoved(int position);
};

#endif // BANDSWINDOW_H
