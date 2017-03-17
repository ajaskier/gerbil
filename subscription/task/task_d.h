#ifndef TASK_D_H
#define TASK_D_H

#include <QObject>
#include <task/task.h>
#include "lock.h"

class TaskD : public Task
{
public:
	explicit TaskD(int d);
	virtual ~TaskD();

	virtual bool run() override;

private:

	int d;
};

#endif // TASK_D_H
