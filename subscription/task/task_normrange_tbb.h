#ifndef TASK_NORMRANGE_TBB_H
#define TASK_NORMRANGE_TBB_H

#include <task/task.h>
#include "model/representation.h"

#include "task_datarange_tbb.h"

class TaskNormRangeTbb : public TaskDataRangeTbb
{
public:
	explicit TaskNormRangeTbb(QString destId, multi_img::NormMode mode,
	                          multi_img::Value minval, multi_img::Value maxval,
	                          representation::t type,
	                          bool update);
	virtual ~TaskNormRangeTbb();

	virtual bool run() override;

private:

	multi_img::NormMode mode;
	multi_img::Value minval;
	multi_img::Value maxval;
	representation::t type;
	bool update;
};


#endif // TASK_NORMRANGE_TBB_H
