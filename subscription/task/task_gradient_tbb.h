#ifndef TASK_GRADIENT_TBB_H
#define TASK_GRADIENT_TBB_H

#include <task/tbb_task.h>
#include <opencv2/core/core.hpp>

class TaskGradientTbb : public TbbTask
{
public:
	explicit TaskGradientTbb(bool includecache = true);
	virtual ~TaskGradientTbb();

	virtual bool run() override;

private:
	bool includecache;
};


#endif // TASK_GRADIENT_TBB_H
