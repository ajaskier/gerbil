#ifndef TASK_NORML2TBB_H
#define TASK_NORML2TBB_H

#include <QObject>
#include <task/tbb_task.h>


class TaskNormL2Tbb : public TbbTask
{
public:
	explicit TaskNormL2Tbb();
	virtual ~TaskNormL2Tbb();
	virtual bool run() override;
};

#endif // TASK_NORML2TBB_H
