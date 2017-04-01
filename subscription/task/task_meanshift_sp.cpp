#include "task_meanshift_sp.h"
#include "subscription_type.h"
#include "subscription.h"
#include "lock.h"
#include <QThread>

#include <QDebug>

#include "data.h"
#include "multi_img.h"
#include "model/labels_model.h"
#include "labels/task_set_labels.h"

namespace seg_meanshift {
TaskMeanShiftSP::TaskMeanShiftSP(QString sourceId, QString sourceGradId)
    : CommandTask("meanshift", "labels",
                  { { "source", { sourceId } }, { "sourceGrad", { sourceGradId } },
                    { "ROI", { "ROI" } } })
{}


bool TaskMeanShiftSP::run()
{
	std::map<std::string, boost::any> output;
	{
		std::map<std::string, boost::any> input;

		Subscription::Lock<multi_img> lock(*sub("source"));
		const multi_img* const        source = lock();

		Subscription::Lock<multi_img> grad_lock(*sub("sourceGrad"));
		const multi_img* sourceGrad = grad_lock();

		input["multi_img"]  = source;
		input["multi_grad"] = sourceGrad;

		emit progressChanged(0);
		try {
			output = cmd->execute(input, this);
		} catch (std::exception& e) {
			qDebug() << e.what();
			emit exception(std::current_exception(), false);
			abort();
		}
	}

	if (isAborted()) {
		emit failure();
		return false;
	} else
		emit progressChanged(100);


	if (output.count("labels")) {
		boost::shared_ptr<cv::Mat1s> labelMask = boost::any_cast< boost::shared_ptr<cv::Mat1s> >(
		    output["labels"]);
		cv::Mat1s mask = labelMask->clone();

		Subscription::Lock<multi_img> lock(*sub("source"));
		const multi_img* source = lock();

		Labeling      vl(mask, false);
		TaskSetLabels task(vl, cv::Size(source->width, source->height), false, true);
		task.importSubscription(sub("dest"));
		task.importSubscription(sub("ROI"));

		auto success = task.start();
	}

	return true;
}
}
