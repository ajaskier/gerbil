#ifndef TBB_TASK_H
#define TBB_TASK_H

#include <task/task.h>

#include <tbb/blocked_range2d.h>
#include <tbb/task_group.h>

class TbbTask : public Task
{
public:
	explicit TbbTask(QString target, std::map<QString, SourceDeclaration> sources);
	explicit TbbTask(QString id, QString target, std::map<QString, SourceDeclaration> sources);
	virtual ~TbbTask() {}

	virtual void cancel() { stopper.cancel_group_execution(); }

protected:
	virtual bool run() = 0;
	virtual bool isCancelled() { return stopper.is_group_execution_cancelled(); }
	tbb::task_group_context stopper;
};


#endif // TBB_TASK_H
