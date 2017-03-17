#ifndef TASK_IMAGE_NORM_H
#define TASK_IMAGE_NORM_H

#include <QObject>
#include <task/task.h>
#include <opencv2/core/core.hpp>
#include <multi_img.h>
#include "model/representation.h"

class TaskImageNORM : public Task
{
public:
	explicit TaskImageNORM(multi_img::NormMode normMode,
	                       multi_img_base::Range normRange,
	                       representation::t type, bool update);
	virtual ~TaskImageNORM();
	virtual bool run() override;

private:

	//normalization
	multi_img::NormMode normMode;
	multi_img_base::Range normRange;
	representation::t type;
	bool update;
};


#endif // TASK_IMAGE_NORM_H
