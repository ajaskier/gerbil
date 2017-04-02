#ifndef SIMILARITYWIDGET_H
#define SIMILARITYWIDGET_H

#include "ui_similaritywidget.h"
#include "widgets/autohidewidget.h"
#include "widgets/scaledview.h"
#include "sm_config.h"

class AutohideView;

class SimilarityWidget : public AutohideWidget, private Ui::SimilarityWidget
{
	Q_OBJECT

public:
	explicit SimilarityWidget(AutohideView* view);
	~SimilarityWidget();

	QAction* targetAction() { return actionTarget; }
	QAction* doneAction() { return actionDone; }
	similarity_measures::SMConfig config();

private:
	void initUi(AutohideView *view);
};

#endif // SIMILARITYWIDGET_H
