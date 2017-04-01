#ifndef TASK_MEANSHIFT_SP_H
#define TASK_MEANSHIFT_SP_H

#include <QObject>
#include "lock.h"

#include "command_task.h"
#include "meanshift_config.h"

namespace seg_meanshift {
class TaskMeanShiftSP : public CommandTask
{
public:
	explicit TaskMeanShiftSP(QString sourceId, QString sourceGradId);
	virtual ~TaskMeanShiftSP() {}

	virtual bool run() override;
};
}

#endif // TASK_MEANSHIFT_SP_H
