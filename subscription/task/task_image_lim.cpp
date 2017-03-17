#include "task_image_lim.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "multi_img/multi_img_offloaded.h"
#include "imginput.h"

TaskImageLim::TaskImageLim(const QString &filename, bool limitedMode)
	: Task("image", {}), filename(filename), limitedMode(limitedMode)
{}

TaskImageLim::~TaskImageLim()
{}

bool TaskImageLim::run()
{
	std::string fn = filename.toLocal8Bit().constData();
	if (limitedMode) {
		std::pair<std::vector<std::string>, std::vector<multi_img::BandDesc> >
		filelist = multi_img::parse_filelist(fn);
		multi_img_offloaded offloaded = multi_img_offloaded(filelist.first,
		                                                    filelist.second);

		Subscription::Lock<multi_img_offloaded> lock(*sub("dest"));
		lock.swap(offloaded);
		lock.setVersion(lock.version() + 1);
	} else {
		multi_img::ptr img = imginput::ImgInput::load(fn);

		Subscription::Lock<multi_img> lock(*sub("dest"));
		lock.swap(*img);
		lock.setVersion(lock.version() + 1);
	}

	return true;
}
