#ifndef CLUSTERIZATION_MODEL_H
#define CLUSTERIZATION_MODEL_H

#include <QColor>
#include <QVector>

#include <model/clustering/clusteringrequest.h>
#include <model/representation.h>
#include <QObject>
#include <opencv2/core/core.hpp>
#include <model/model.h>
#include "subscription.h"


class ClusterizationModel : public Model
{
	Q_OBJECT

public:
	explicit ClusterizationModel(TaskScheduler* scheduler, QObject *parent = nullptr);
	virtual void delegateTask(QString id, QString parentId = "") override;

signals:
	void progressChanged(int);
	void abort();
	void segmentationCompleted();


public slots:
	void requestSegmentation(const ClusteringRequest &r);
	void taskFinished(QString id, bool success);

private:
};

#endif // CLUSTERIZATION_MODEL_H
