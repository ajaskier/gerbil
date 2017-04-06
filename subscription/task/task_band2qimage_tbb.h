#ifndef TASK_BAND2QIMAGE_TBB_H
#define TASK_BAND2QIMAGE_TBB_H

#include <task/tbb_task.h>

class TaskBand2QImageTbb : public TbbTask
{
public:
	explicit TaskBand2QImageTbb(QString sourceId, QString destId, size_t dim);
	virtual ~TaskBand2QImageTbb();

	virtual bool run() override;

private:
	size_t dim;
};


#endif // TASK_BAND2QIMAGE_TBB_H
