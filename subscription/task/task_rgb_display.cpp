#include "task_rgb_display.h"
#include "subscription_type.h"
#include "subscription.h"
#include "lock.h"
#include <QThread>

#include <QDebug>

#include "multi_img.h"

#include <QPixmap>
#include <qtopencv.h>
#include <rgb.h>

TaskRGBDisplay::TaskRGBDisplay(QString destId, QString sourceId, FalseColoring::Type coloringType)
    : CommandTask(destId, destId, { { "source", { sourceId } } }), coloringType(coloringType)
{}

bool TaskRGBDisplay::run()
{
	std::map<std::string, boost::any> output;

	{
		std::map<std::string, boost::any> input;

		Subscription::Lock<multi_img> lock(*sub("source"));
		multi_img_base* source = lock();

		input["multi_img"] = source;

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

	if (output.count("multi_img")) {
		cv::Mat3f mat = boost::any_cast<cv::Mat3f>(output["multi_img"]);
		QPixmap   result;
		result.convertFromImage(Mat2QImage((cv::Mat3b)mat));

		Subscription::Lock<QPixmap, FalseColoring::Type> dest_lock(*sub("dest"));
		dest_lock.swap(result);
		dest_lock.swapMeta(coloringType);

		Subscription::Lock<multi_img> lock(*sub("source"));
		dest_lock.setVersion(lock.version());
	}

	return true;
}

