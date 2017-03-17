#ifndef TASK_DATARANGE_TBB_H
#define TASK_DATARANGE_TBB_H

#include <task/task.h>
#include "model/representation.h"

#include "multi_img.h"
#include "tbb/tbb.h"

class TaskDataRangeTbb : public Task
{
public:
	explicit TaskDataRangeTbb(QString destId);
	virtual ~TaskDataRangeTbb();

	virtual bool run() override;

protected:

	virtual bool isCancelled() { return stopper.is_group_execution_cancelled(); }

	tbb::task_group_context stopper;
};


#endif // TASK_DATARANGE_TBB_H
