#include "labels_model.h"

#include "task_scheduler.h"
#include "task/task.h"

#include "data_register.h"
#include "data.h"
#include "lock.h"

#include "qtopencv.h"

#include <QVector>

#include "task/labels/task_set_labels.h"
#include "task/labels/task_labels_alter_pixels.h"
#include "task/labels/task_add_label.h"
#include "task/labels/task_labels_icons.h"
#include "task/labels/task_merge_labels.h"
#include "task/labels/task_labels_consolidate.h"

LabelsModel::LabelsModel(TaskScheduler *scheduler, QObject *parent)
	: Model(scheduler, parent), iconSize(32, 32)
{
	registerData("labels", { "ROI" });
	registerData("labels.icons", { "labels" });
}

void LabelsModel::delegateTask(QString id, QString parentId)
{
	if (id == "labels") {
		setLabels(lastLabeling, true);
	} else if (id == "labels.icons") {
		computeIcons();
	}
}

void LabelsModel::setImageSize(cv::Size imgSize)
{
	originalImageSize = imgSize;
}

void LabelsModel::setLabels(const cv::Mat1s &labeling)
{
	Labeling vl(labeling, false);
	setLabels(vl, false);
}

void LabelsModel::sendTask(std::shared_ptr<Task> t)
{
	Model::sendTask(t);

	labelsSub = std::unique_ptr<Subscription>(DataRegister::subscribe(Dependency("labels",
	                                                                             SubscriptionType::
	                                                                             READ,
	                                                                             AccessType::
	                                                                             DEFERRED), this,
	                                                                  std::bind(&LabelsModel::
	                                                                            labelsUpdated, this)));
}

void LabelsModel::labelsUpdated()
{
	Labels l;
	{
		Subscription::Lock<Labels> lock(*labelsSub);
		l = *lock();
	}

	lastLabeling = Labeling(l.scopedlabels.clone(), false);
	labelsSub.reset(nullptr);
}

void LabelsModel::taskFinished(QString id, bool success)
{
	Model::taskFinished(id, success);
}

void LabelsModel::setLabels(const Labeling &labeling, bool full)
{
	lastLabeling = labeling;
	sendTask<TaskSetLabels>(lastLabeling, originalImageSize, full);
}

void LabelsModel::alterPixels(const cv::Mat1s &newLabels, const cv::Mat1b &mask)
{
	lastMask = mask.clone(); //looks tricky, but it's the best I can for now

	bool profitable = (size_t(2 * cv::countNonZero(mask)) < mask.total());
	if (profitable) {
		sendTask<TaskLabelsAlterPixels>(newLabels, lastMask);
	} else {
		setLabels(newLabels);
	}
}

void LabelsModel::addLabel()
{
	sendTask<TaskAddLabel>();
}

void LabelsModel::computeIcons()
{
	sendTask<TaskLabelsIcons>(iconSize, applyROI);
}

void LabelsModel::setApplyROI(bool applyROI)
{
	this->applyROI = applyROI;
	computeIcons();
}

void LabelsModel::setIconsSize(QSize size)
{
	this->iconSize = size;
	computeIcons();
}

void LabelsModel::mergeLabels(const QVector<int> mlabels)
{
	sendTask<TaskMergeLabels>(mlabels);
}

void LabelsModel::deleteLabels(const QVector<int> mlabels)
{
	QVector<int> tmp = mlabels;
	tmp.append(0);

	sendTask<TaskMergeLabels>(tmp);
}

void LabelsModel::consolidateLabels()
{
	sendTask<TaskLabelsConsolidate>(originalImageSize);
}
