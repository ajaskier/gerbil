#ifndef DATA_TASK_CHAIN_H
#define DATA_TASK_CHAIN_H

#include "task_scheduler.h"
#include "task/task.h"

#include <QDebug>
#include <QString>
#include <cstdlib>

struct MultiDataChainResult
{
	std::vector<std::vector<QString> > steps;
	std::vector<QString> totalChain = {};
};

struct DataTaskChain
{
	DataTaskChain()
	{
		setUpIds();
		setUpChain();
	}

	const std::vector<QString>& operator()(QString id)
	{
		return chain[id];
	}
	QString randomId()
	{
		return ids[rand() % ids.size()];
	}

	std::vector<QString> randomIds(int size = 10)
	{
		std::vector<QString> ids;
		while (size--) {
			ids.push_back(randomId());
		}
		return ids;
	}

	MultiDataChainResult multiDataChain(std::vector<QString> dataIds)
	{
		MultiDataChainResult result;
		for (QString& id : dataIds) {
			auto step = chain[id];

			std::vector<QString> stepTasks;
			for (QString& sid : step) {
				bool exists = std::find(result.totalChain.begin(),
				                        result.totalChain.end(),
				                        sid) != result.totalChain.end();

				if (!exists) {
					stepTasks.push_back(sid);
					result.totalChain.push_back(sid);
				}
			}
			result.steps.push_back(stepTasks);
		}

		return result;
	}

	std::map<QString, std::vector<QString> > chain;
	std::vector<QString> ids;

private:
	void setUpChain()
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
	void    setUpIds()
	{
		ids = { "ROI",           "image",
		        "image.IMG",     "image.NORM",
		        "image.GRAD",    "image.IMGPCA",
		        "image.GRADPCA", "image.rgb",
		        "image.bgr",     "labels",
		        "labels.icons",  "dist.IMG" };


//		//without imgage.PCA and image.GRADPCA
//		ids = { "ROI",          "image",
//		        "image.IMG",    "image.NORM",
//		        "image.GRAD",   "image.rgb",
//		        "image.bgr",    "labels",
//		        "labels.icons", "dist.IMG" };
	}
};


#endif // DATA_TASK_CHAIN_H
