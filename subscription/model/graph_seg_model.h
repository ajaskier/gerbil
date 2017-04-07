#ifndef GRAPH_SEG_MODEL_H
#define GRAPH_SEG_MODEL_H

#include <QColor>
#include <QVector>

#include <model/representation.h>
#include <QObject>
#include <opencv2/core/core.hpp>
#include <model/model.h>
#include "subscription.h"
#include "graphseg_config.h"

namespace seg_graphs
{
class GraphSegConfig;
}

class GraphSegModel : public Model
{
	Q_OBJECT

public:
	explicit GraphSegModel(TaskScheduler* scheduler, QObject *parent = nullptr);

	virtual void delegateTask(QString id, QString parentId = "") override;

public slots:
	void setCurLabel(int curLabel);
	void runGraphseg(representation::t type, cv::Mat1s seedMap,
	                 const seg_graphs::GraphSegConfig config, bool resetLabel);
	void runGraphsegBand(representation::t type, int bandId, cv::Mat1s seedMap,
	                     const seg_graphs::GraphSegConfig config, bool resetLabel);
signals:
	void seedingDone();

private:

	void taskFinished(QString id, bool success);

	int       curLabel;
	cv::Mat1s lastSeedMap;
	seg_graphs::GraphSegConfig lastConfig;
};

#endif // GRAPH_SEG_MODEL_H
