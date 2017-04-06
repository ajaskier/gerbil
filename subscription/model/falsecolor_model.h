#ifndef FALSECOLOR_MODEL_H
#define FALSECOLOR_MODEL_H

#include <QColor>
#include <QVector>

#include <model/representation.h>
#include <QObject>
#include <opencv2/core/core.hpp>
#include <model/model.h>
#include "subscription.h"

#include "model/falsecolor/falsecoloring.h"
#include "model/falsecolor/falsecoloringcacheitem.h"
#include "rgb.h"

class FalsecolorTaskDelegate;

class FalsecolorModel : public Model
{
	Q_OBJECT

public:
	explicit FalsecolorModel(TaskScheduler* scheduler, QObject *parent = nullptr);

	virtual void delegateTask(QString id, QString parentId = "") override;
	static QString coloringTypeToString(const FalseColoring::Type& coloringType);
	FalseColoring::Type stringToColoringType(QString coloringType);

signals:
	void progressChanged(FalseColoring::Type, int);
	void coloringCompleted(FalseColoring::Type);
	void coloringCancelled(FalseColoring::Type);
	//void abort(FalseColoring::Type);

public slots:
	void requestColoring(FalseColoring::Type coloringType, bool recalc = false);
	void requestAbort(FalseColoring::Type type);

private:
	void computeColoring(FalseColoring::Type coloringType);
	void setupConfig(rgb::RGBDisplay *cmd, FalseColoring::Type coloringType);

	void setupDelegates();
	void taskFinished(QString id, bool success);

	std::map<FalseColoring::Type, FalsecolorTaskDelegate*> delegates;



//	using FalseColorModelPayloadMap = QMap<FalseColoring::Type, FalseColorModelPayload*>;
//	FalseColorModelPayloadMap payloads;
};

#endif // FALSECOLOR_MODEL_H
