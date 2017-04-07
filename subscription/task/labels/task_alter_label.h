#ifndef TASK_ALTER_LABEL_H
#define TASK_ALTER_LABEL_H

#include <QObject>
#include <task/task.h>

#include "labeling.h"
#include "model/labels_model.h"

class TaskAlterLabel : public Task
{
public:
	explicit TaskAlterLabel(short index, cv::Mat1b mask, bool negative);
	virtual bool run() override;

protected:

	short     index;
	cv::Mat1b mask;
	bool      negative;
};

#endif // TASK_ALTER_LABEL_H
