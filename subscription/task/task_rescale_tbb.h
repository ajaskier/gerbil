#ifndef TASK_RESCALE_TBB_H
#define TASK_RESCALE_TBB_H

#include <QObject>
#include <task/tbb_task.h>

#include <opencv2/core/core.hpp>
#include "multi_img.h"

class TaskRescaleTbb : public TbbTask
{
public:
	explicit TaskRescaleTbb(size_t bands, size_t roiBands, bool includecache = true);
	virtual ~TaskRescaleTbb();

	virtual bool run() override;

private:
	size_t bands;
	size_t roiBands;
	size_t newsize;
	bool   includecache;
};

#endif // TASK_RESCALE_TBB_H
