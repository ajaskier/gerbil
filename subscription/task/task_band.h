#ifndef TASK_BAND_H
#define TASK_BAND_H

#include <task/task.h>
#include "model/representation.h"


class TaskBand : public Task
{
public:
	explicit TaskBand(QString sourceId, QString destId,
	                  size_t dim, representation::t repr);
	virtual ~TaskBand();

	virtual bool run() override;

private:

	size_t dim;
	representation::t repr;
};


#endif // TASK_BAND_H
