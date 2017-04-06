#include "falsecolor_task_delegate.h"

#include "data_register.h"
#include "data.h"
#include "lock.h"

#include <QVector>

FalsecolorTaskDelegate::FalsecolorTaskDelegate(FalseColoring::Type coloringType, QObject *parent)
    : QObject(parent), coloringType(coloringType)
{}

void FalsecolorTaskDelegate::setTask(std::shared_ptr<TaskRGBDisplay> task)
{
	this->task = task;

	connect(task.get(), &CommandTask::progressChanged,
	        this, &FalsecolorTaskDelegate::onTaskProgressChanged,
	        Qt::QueuedConnection);

	connect(this, &FalsecolorTaskDelegate::requestTaskAbort,
	        task.get(), &CommandTask::cancel);

	connect(task.get(), &CommandTask::taskFinished,
	        this, &FalsecolorTaskDelegate::onTaskFinished,
	        Qt::QueuedConnection);
}

void FalsecolorTaskDelegate::requestAbort()
{
	emit requestTaskAbort();
}

void FalsecolorTaskDelegate::onTaskFinished()
{
	task.reset();
	emit taskFinished(coloringType);
}

void FalsecolorTaskDelegate::onTaskProgressChanged(int percent)
{
	emit taskProgressChanged(coloringType, percent);
}

