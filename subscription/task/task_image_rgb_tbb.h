#ifndef TASK_IMAGE_RGB_TBB_H
#define TASK_IMAGE_RGB_TBB_H

#include <QObject>
#include <task/task.h>

#include <tbb/blocked_range2d.h>
#include <tbb/task_group.h>

#include "task_image_bgr_tbb.h"

class TaskImageRgbTbb : public TbbTask
{
public:
	explicit TaskImageRgbTbb(QString destId, QString sourceId);
	virtual bool run() override;
};


#endif // TASK_IMAGE_RGB_TBB_H
