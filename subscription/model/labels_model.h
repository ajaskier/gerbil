#ifndef LABELS_MODEL_H
#define LABELS_MODEL_H

#include <QColor>
#include <QVector>

#include <model/representation.h>
#include <QObject>
#include <opencv2/core/core.hpp>
#include <model/model.h>
#include "subscription.h"

#include "labeling.h"

class Labels
{
public:
	Labels() {}

	cv::Mat1s       fullLabels;
	cv::Mat1s       scopedlabels;
	QVector<QColor> colors;
};

class LabelsMeta
{
public:
	LabelsMeta() {}

	cv::Mat1s oldLabels;
	cv::Mat1b mask;
};

class LabelsModel : public Model
{
	Q_OBJECT

public:
	explicit LabelsModel(TaskScheduler* scheduler, QObject *parent = nullptr);

	virtual void delegateTask(QString id, QString parentId = "") override;


public slots:

	void setLabels(const Labeling &labeling, bool full);
	void setLabels(const cv::Mat1s &labeling);

	void alterPixels(const cv::Mat1s &newLabels,
	                 const cv::Mat1b &mask);

	void addLabel();
	void mergeLabels(const QVector<int> mlabels);
	void deleteLabels(const QVector<int> mlabels);
	void consolidateLabels();

	void setImageSize(cv::Size imgSize);

	void setApplyROI(bool applyROI);
	void setIconsSize(QSize size);


private:

	void computeIcons();

	virtual void sendTask(std::shared_ptr<Task> t);

	// construct task of type T and send it right away
	template<typename T, typename ... A>
	void sendTask(A && ... args)
	{
		sendTask(std::make_shared<T>(std::forward<A>(args) ...));
	}

	void taskFinished(QString id, bool success);

	void labelsUpdated();

	bool  applyROI;
	QSize iconSize;

	Labeling  lastLabeling;
	cv::Mat1b lastMask;
	cv::Mat1s lastLabels;

	cv::Size originalImageSize;
	std::unique_ptr<Subscription> labelsSub;
};

#endif // LABELS_MODEL_H
