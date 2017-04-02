#ifndef TASK_RGB_DISPLAY_H
#define TASK_RGB_DISPLAY_H

#include <QObject>
#include "lock.h"

#include "command_task.h"
#include "meanshift_config.h"

#include "model/falsecolor_model.h"


class TaskRGBDisplay : public CommandTask
{
public:
	explicit TaskRGBDisplay(QString destId, QString sourceId, FalseColoring::Type coloringType);
	virtual ~TaskRGBDisplay() {}

	virtual bool run() override;

private:

	FalseColoring::Type coloringType;
};


#endif // TASK_RGB_DISPLAY_H
