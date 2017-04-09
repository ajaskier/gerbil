#ifndef DATA_TASK_CHAIN_H
#define DATA_TASK_CHAIN_H

#include "task_scheduler.h"
#include "task/task.h"

#include <QDebug>
#include <QString>

struct DataTaskChain
{
	DataTaskChain()
	{
		chain["ROI"] = {};
		chain["image"]         = { "image" };
		chain["image.IMG"]     = { "ROI", "image", "image.IMG" };
		chain["image.NORM"]    = { "ROI", "image", "image.IMG", "image.NORM" };
		chain["image.GRAD"]    = { "ROI", "image", "image.IMG", "image.GRAD" };
		chain["image.IMGPCA"]  = { "ROI", "image", "image.IMG", "image.IMGPCA" };
		chain["image.GRADPCA"] = { "ROI", "image", "image.IMG", "image.GRAD", "image.GRADPCA" };
		chain["image.rgb"]     = { "image", "image.bgr", "image.rgb" };
		chain["image.bgr"]     = { "image", "image.bgr" };
		chain["labels"]        = { "ROI", "setLabels" };
		chain["labels.icons"]  = { "ROI", "setLabels", "labels.icons" };
		chain["dist.IMG"]      = { "ROI", "image", "image.IMG", "setLabels", "taskAdd" };
	}

	const std::vector<QString>& operator()(QString id)
	{
		return chain[id];
	}

	std::map<QString, std::vector<QString> > chain;
};


#endif // DATA_TASK_CHAIN_H
