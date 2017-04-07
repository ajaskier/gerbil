#ifndef TASK_IMAGE_BGR_TBB_H
#define TASK_IMAGE_BGR_TBB_H

#include <QObject>
#include <task/tbb_task.h>

class TaskImageBgrTbb : public TbbTask
{
public:
	explicit TaskImageBgrTbb(QString destId, QString sourceId);
	virtual bool run() override;
};


#endif // TASK_IMAGE_BGR_TBB_H
