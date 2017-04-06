#ifndef FALSECOLORDOCK_H
#define FALSECOLORDOCK_H

#include "ui_falsecolordock_sel.h"
#include <QDockWidget>
#include <shared_data.h>
#include "model/representation.h"
#include "model/falsecolor/falsecoloring.h"
#include "sm_config.h"

#include "subscription.h"
#include <memory>
#include <subscription_manager.h>

class AutohideView;
class ScaledView;
class AutohideWidget;
class SimilarityWidget;
class FalseColorSubscriptionDelegate;

struct FalseColoringState
{
	enum Type { UNKNOWN=0, FINISHED, CALCULATING, ABORTING };
};
std::ostream &operator<<(std::ostream& os, const FalseColoringState::Type& state);

class FalseColorDock : public QDockWidget {
	Q_OBJECT
public:
	explicit FalseColorDock(QWidget *parent = 0);

signals:
	/** Request re-calculation of non-determinitstic representation (e.g. SOM). */
	void falseColoringRecalcRequested(FalseColoring::Type coloringType);

	//void subscribeFalseColoring(QObject* subscriber, FalseColoring::Type coloringType);
	//void unsubscribeFalseColoring(QObject* subscriber, FalseColoring::Type coloringType);

	void requestFalseColoring(FalseColoring::Type coloringType, bool recalc);

	void pixelOverlay(int, int);
	void requestComputeSpecSim(int x, int y, similarity_measures::SMConfig conf);
	void cancelComputation(FalseColoring::Type);

public slots:
	void processVisibilityChanged(bool visible);
	void processCalculationProgressChanged(FalseColoring::Type coloringType, int percent);
	void processFalseColoringUpdate(FalseColoring::Type coloringType, QPixmap p);
	void processComputationCancelled(FalseColoring::Type coloringType);
	void restoreFalseColorFunction();

	/** Inform the FalseColorDock an update of the FalseColoring coloringType is pending.
	 *
	 * This triggers the display of a progress bar.
	 */
	void setCalculationInProgress(FalseColoring::Type coloringType);

	void processSpecSimUpdate(QPixmap result);

protected slots:
	void processSelectedColoring(); // the selection in the combo box changed
	void processApplyClicked();
	void screenshot();
	void requestSpecSim(int x, int y);

	void saveState();

	void coloringUpdated(FalseColoring::Type coloringType);

protected:
	void initUi();
	void restoreState();
	// event filter to intercept leave() on our view
	bool eventFilter(QObject *obj, QEvent *event);

	// the coloringType currently selected in the comboBox
	FalseColoring::Type selectedColoring();

	void requestColoring(FalseColoring::Type coloringType, bool recalc = false);
	// show/hide the progress bar depending on coloringState and update the value
	void updateProgressBar();
	// update/show/hide apply/cancel button according to state
	void updateTheButton();

	void enterState(FalseColoring::Type coloringType, FalseColoringState::Type state);

	void subscribeColoring(FalseColoring::Type coloringType);

	// State of the FalseColorings
	QMap<FalseColoring::Type, FalseColoringState::Type> coloringState;
	// Calculation progress percentage for progressbar for each FalseColoring
	QMap<FalseColoring::Type, int>  coloringProgress;
	QMap<FalseColoring::Type, bool> coloringUpToDate;

	FalseColoring::Type lastShown = FalseColoring::CMF;

	FalseColoring::Type currentColoring = FalseColoring::CMF;
	FalseColoring::Type lastColoring    = FalseColoring::CMF;

	//std::map<FalseColoring::Type, std::unique_ptr<Subscription> > subs;
	std::map<FalseColoring::Type, FalseColorSubscriptionDelegate*> subs;

	// viewport and scene
	AutohideView *view;
	ScaledView   *scene;
	// UI and widget for our button row
	Ui::FalsecolorDockSelUI *uisel;
	AutohideWidget          *sel;
	SimilarityWidget        *sw;

	//std::unique_ptr<Subscription> sub;
};

#endif // FALSECOLORDOCK_H
