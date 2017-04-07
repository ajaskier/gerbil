#ifndef TASK_PCA_TBB_H
#define TASK_PCA_TBB_H

#include <QObject>
#include <task/tbb_task.h>

#include <opencv2/core/core.hpp>

class TaskPcaTbb : public TbbTask
{
public:
	explicit TaskPcaTbb(QString sourceId, QString destId,
	                    unsigned int components = 0, bool includecache = true);
	virtual ~TaskPcaTbb();

	virtual bool run() override;

private:

	unsigned int components;
	bool         includecache;
};


#endif // TASK_PCA_TBB_H
