#include "graph_seg_model.h"

#include "task_scheduler.h"
#include "task/task.h"

#include "data_register.h"
#include "data.h"
#include "lock.h"

#include "task/task_graph_seg.h"

#include "qtopencv.h"

GraphSegModel::GraphSegModel(TaskScheduler *scheduler, QObject *parent)
    : Model(scheduler, parent)
{}

void GraphSegModel::delegateTask(QString id, QString parentId)
{}


void GraphSegModel::runGraphseg(representation::t type, cv::Mat1s seedMap,
                                const seg_graphs::GraphSegConfig config,
                                bool resetLabel)
{
	lastSeedMap = seedMap.clone();
	lastConfig  = config;
	QString dataId = "image." + representation::str(type);
	sendTask<TaskGraphSeg>(dataId, lastConfig, lastSeedMap, resetLabel, curLabel);
}

void GraphSegModel::runGraphsegBand(representation::t type, int bandId,
                                    cv::Mat1s seedMap,
                                    const seg_graphs::GraphSegConfig config,
                                    bool resetLabel)
{
	lastSeedMap = seedMap.clone();
	lastConfig  = config;
	QString dataId = "bands." + representation::str(type) + "." + QString::number(bandId);
	sendTask<TaskGraphSeg>(dataId, lastConfig, lastSeedMap, resetLabel, curLabel);
}

void GraphSegModel::setCurLabel(int curLabel)
{
	this->curLabel = curLabel;
}

void GraphSegModel::taskFinished(QString id, bool success)
{
	Model::taskFinished(id, success);
	emit seedingDone();
}
