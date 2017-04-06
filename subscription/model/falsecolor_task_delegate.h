#ifndef FALSECOLOR_TASK_DELEGATE_H
#define FALSECOLOR_TASK_DELEGATE_H

#include <QColor>
#include <QVector>
#include <QObject>

#include <model/representation.h>
#include <QObject>
#include <opencv2/core/core.hpp>
#include <model/model.h>
#include "subscription.h"

#include "model/falsecolor/falsecoloring.h"
#include "model/falsecolor/falsecoloringcacheitem.h"
#include "rgb.h"

#include "task/task_rgb_display.h"

class FalsecolorTaskDelegate : public QObject
{
	Q_OBJECT

public:
	explicit FalsecolorTaskDelegate(FalseColoring::Type coloringType,
	                                QObject             *parent = nullptr);
	void setTask(std::shared_ptr<TaskRGBDisplay> task);

	void requestAbort();

signals:
	void taskProgressChanged(FalseColoring::Type, int);
	void taskFinished(FalseColoring::Type);
	void requestTaskAbort();

private slots:
	void onTaskProgressChanged(int percent);
	void onTaskFinished();

private:
	std::shared_ptr<TaskRGBDisplay> task;
	FalseColoring::Type coloringType;
};

#endif // FALSECOLOR_TASK_DELEGATE_H
