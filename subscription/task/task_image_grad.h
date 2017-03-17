#ifndef TASK_IMAGE_GRAD_H
#define TASK_IMAGE_GRAD_H

#include <QObject>
#include <task/task.h>
#include <opencv2/core/core.hpp>
#include <multi_img.h>
#include "model/representation.h"

class TaskImageGRAD : public Task
{
public:
	explicit TaskImageGRAD(multi_img::NormMode normMode,
	                       multi_img_base::Range normRange,
	                       representation::t type, bool update,
	                       bool includecache = true);
	virtual ~TaskImageGRAD();
	virtual bool run() override;

private:

	bool includecache;

	//normalization
	multi_img::NormMode normMode;
	multi_img_base::Range normRange;
	representation::t type;
	bool update;
};


#endif // TASK_IMAGE_GRAD_H
