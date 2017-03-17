#ifndef TASK_IMAGE_IMG_H
#define TASK_IMAGE_IMG_H

#include <QObject>
#include <task/task.h>
#include <opencv2/core/core.hpp>
#include <multi_img.h>
#include "model/representation.h"

class TaskImageIMG : public Task
{
public:
	explicit TaskImageIMG(size_t bands, size_t roiBands,
	                      multi_img::NormMode normMode,
	                      multi_img_base::Range normRange,
	                      representation::t type, bool update,
	                      bool includecache = true);
	virtual ~TaskImageIMG();
	virtual bool run() override;

private:

	//rescale
	size_t bands;
	size_t roiBands;
	//size_t newsize;
	bool includecache;

	//normalization
	multi_img::NormMode normMode;
	multi_img_base::Range normRange;
	representation::t type;
	bool update;
};


#endif // TASK_IMAGE_IMG_H
