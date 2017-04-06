#ifndef TASK_DATARANGE_TBB_H
#define TASK_DATARANGE_TBB_H

#include <task/tbb_task.h>
#include "model/representation.h"

#include "multi_img.h"

class TaskDataRangeTbb : public TbbTask
{
public:
	explicit TaskDataRangeTbb(QString destId);
	virtual ~TaskDataRangeTbb();
	virtual bool run() override;
};


#endif // TASK_DATARANGE_TBB_H
