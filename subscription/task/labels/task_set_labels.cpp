#include "task_set_labels.h"

#include "subscription.h"
#include "lock.h"
#include "data_register.h"

#include "data.h"
#include "QDebug"

#include "labeling.h"
#include "qtopencv.h"

#define REUSE_THRESHOLD    0.1

TaskSetLabels::TaskSetLabels(const Labeling &labeling, const cv::Size originalImageSize,
                             bool full)
    : Task("setLabels", "labels", { { "ROI", { "ROI" } } }),
    labeling(labeling), originalImageSize(originalImageSize),
    full(full)
{}

bool TaskSetLabels::run()
{
	Labels* dst = nullptr;
	Subscription::Lock<Labels, LabelsMeta> dest_lock(*sub("dest"));
	if (DataRegister::isInitialized("labels")) {
		dst = dest_lock();
	}

	Labels l = getLabels(labeling(), dst);

	dest_lock.swap(l);

	LabelsMeta lMeta;
	lMeta.mask = cv::Mat1b().clone();
	dest_lock.swapMeta(lMeta);

	Subscription::Lock<cv::Rect> roi_lock(*sub("ROI"));
	dest_lock.setVersion(roi_lock.version());

	return true;
}

Labels TaskSetLabels::getLabels(cv::Mat1s m, Labels* dst)
{
	Labels l;
	if (dst) {
		l = *dst;

		Subscription::Lock<cv::Rect> roi_lock(*sub("ROI"));
		cv::Rect roi = *roi_lock();


		int version = DataRegister::majorVersion("labels");
		if (roi_lock.version() > version) {
			l.scopedlabels = cv::Mat1s(l.fullLabels, roi);
			return l;
		} else {
			full = full ||
			       (m.cols == l.fullLabels.cols
			        && m.rows == l.fullLabels.rows);

			if (full) {
				m.copyTo(l.fullLabels);
				//l.scopedlabels = cv::Mat1s(l.fullLabels, roi);
			} else {
				m.copyTo(l.scopedlabels);
			}
		}
	} else {
		//initial
		Subscription::Lock<cv::Rect> roi_lock(*sub("ROI"));
		cv::Rect roi = *roi_lock();

		l.fullLabels = cv::Mat1s(originalImageSize.height,
		                         originalImageSize.width,
		                         (short)0);
		l.scopedlabels = cv::Mat1s(l.fullLabels, roi);
	}

	if (labeling.colors().size() < 2) {
		setColors(Labeling::colors(2, true), l);
	} else {
		setColors(labeling.colors(), l);
	}

	return l;
}

void TaskSetLabels::setColors(const std::vector<cv::Vec3b> &newColors, Labels& l)
{
	QVector<QColor> col = Vec2QColor(newColors);
	col[0] = Qt::white; // override black for label 0

	l.colors.swap(col);
}
