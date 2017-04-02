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

class FalsecolorModel : public Model
{
	Q_OBJECT

public:
	explicit FalsecolorModel(TaskScheduler* scheduler, QObject *parent = nullptr);

	virtual void delegateTask(QString id, QString parentId = "") override;
	static QString coloringTypeToString(const FalseColoring::Type& coloringType);
	FalseColoring::Type stringToColoringType(QString coloringType);


signals:
	void progressChanged(int);
	void abort();
	void coloringCompleted();

public slots:
	void requestColoring(FalseColoring::Type coloringType, bool recalc = false);

private:

	void computeColoring(FalseColoring::Type coloringType);
	void setupConfig(rgb::RGBDisplay *cmd, FalseColoring::Type coloringType);

	void taskFinished(QString id, bool success);



//	using FalseColorModelPayloadMap = QMap<FalseColoring::Type, FalseColorModelPayload*>;
//	FalseColorModelPayloadMap payloads;
};

#endif // FALSECOLOR_MODEL_H
