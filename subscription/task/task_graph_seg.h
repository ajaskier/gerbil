#ifndef TASK_GRAPH_SEG_H
#define TASK_GRAPH_SEG_H

#include <QObject>
#include <task/task.h>

#include <opencv2/core/core.hpp>
#include "multi_img.h"
#include "graphseg.h"

class TaskGraphSeg : public Task
{
public:
	explicit TaskGraphSeg(QString sourceId, const seg_graphs::GraphSegConfig config,
	                      const cv::Mat1s & seedMap,
	                      bool resetLabel, int label);
	virtual ~TaskGraphSeg();

	virtual bool run() override;

private:
	const seg_graphs::GraphSegConfig config;
	const cv::Mat1s& seedMap;
	bool resetLabel;
	int  label = -1;
};

#endif // TASK_GRAPH_SEG_H
