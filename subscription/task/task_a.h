#ifndef TASK_A_H
#define TASK_A_H

#include <QObject>
#include <task/task.h>

class TaskA : public Task
{
public:
	explicit TaskA(int a);
	virtual ~TaskA();

	virtual bool run() override;

private:

	int a;
};

#endif // TASK_A_H
