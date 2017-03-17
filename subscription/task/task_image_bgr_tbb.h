#ifndef TASK_IMAGE_BGR_TBB_H
#define TASK_IMAGE_BGR_TBB_H

#include <QObject>
#include <task/task.h>

#include <tbb/task_group.h>
#include "tbb/tbb.h"

class TaskImageBgrTbb : public Task
{
public:
	explicit TaskImageBgrTbb(QString destId, QString sourceId);
	virtual bool run() override;

protected:
	virtual bool isCancelled() { return stopper.is_group_execution_cancelled(); }
	tbb::task_group_context stopper;
};


#endif // TASK_IMAGE_BGR_TBB_H
