#ifndef TASK_SET_LABELS_H
#define TASK_SET_LABELS_H

#include <QObject>
#include <task/task.h>

#include "labeling.h"
#include "model/labels_model.h"

class TaskSetLabels : public Task
{
public:
	explicit TaskSetLabels(const Labeling &labeling, const cv::Size originalImageSize,
	                       bool full, bool recoloring = false);

	virtual bool run() override;

protected:

	Labels getLabels(cv::Mat1s m, Labels *dst);
	void setColors(const std::vector<cv::Vec3b> &newColors, Labels &l);

	const Labeling &labeling;
	const cv::Size  originalImageSize;
	bool full;
	bool recoloring;
};

#endif // TASK_SET_LABELS_H
