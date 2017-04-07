#ifndef TASK_SPECSIM_TBB_H
#define TASK_SPECSIM_TBB_H

#include <QObject>
#include <task/tbb_task.h>

#include <opencv2/core/core.hpp>
#include "multi_img.h"
#include "sm_config.h"

class TaskSpecSimTbb : public TbbTask
{
public:
	explicit TaskSpecSimTbb(int x, int y, similarity_measures::SMConfig config);
	virtual ~TaskSpecSimTbb();

	virtual bool run() override;

private:
	int x;
	int y;
	similarity_measures::SMConfig config;
};

#endif // TASK_SPECSIM_TBB_H
