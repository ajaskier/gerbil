#include "clusterization_model.h"

#include "task_scheduler.h"
#include "task/task.h"

#include "data_register.h"
#include "data.h"
#include "lock.h"

#include "qtopencv.h"

#include <QVector>

#include "task/task_meanshift_sp.h"

#include "meanshift_shell.h"
#include "meanshift_sp.h"

ClusterizationModel::ClusterizationModel(TaskScheduler *scheduler, QObject *parent)
    : Model(scheduler, parent)
{}

void ClusterizationModel::delegateTask(QString id, QString parentId)
{}

void ClusterizationModel::taskFinished(QString id, bool success)
{
	Model::taskFinished(id, success);

	emit segmentationCompleted();
}

void ClusterizationModel::requestSegmentation(const ClusteringRequest &r)
{
	const bool onGradient = r.repr == representation::GRAD;


	using namespace seg_meanshift;

	//this should be parametrized, because
	//the task can get different combination of images
	std::shared_ptr<TaskMeanShiftSP> task(new TaskMeanShiftSP("image.IMG", "image.GRAD"));

	if (r.method == ClusteringMethod::FAMS || r.method == ClusteringMethod::PSPMS) {
		MeanShiftShell   *cmd   = new MeanShiftShell();
		MeanShiftConfig &config = cmd->config;
		task->setCommand(cmd);

		config.verbosity = 0;
		config.use_LSH   = r.lsh;

		if (r.method == ClusteringMethod::PSPMS) {
			config.sp_withGrad         = onGradient;
			config.starting            = SUPERPIXEL;
			config.superpixel.eqhist   = 1;
			config.superpixel.c        = 0.05f;
			config.superpixel.min_size = 5;
			config.superpixel.similarity.function =
			    similarity_measures::SPEC_INF_DIV;
		}

	} else if (r.method == ClusteringMethod::FSPMS) {
		MeanShiftSP      *cmd   = new MeanShiftSP();
		MeanShiftConfig &config = cmd->config;
		task->setCommand(cmd);

		config.verbosity           = 0;
		config.sp_withGrad         = onGradient;
		config.superpixel.eqhist   = 1;
		config.superpixel.c        = 0.05f;
		config.superpixel.min_size = 5;
		config.superpixel.similarity.function
		    = similarity_measures::SPEC_INF_DIV;
		config.sp_weight = 2;
		config.use_LSH   = r.lsh;
	}

	connect(task.get(), SIGNAL(progressChanged(int)),
	        this, SIGNAL(progressChanged(int)), Qt::QueuedConnection);

	connect(this, &ClusterizationModel::abort,
	        task.get(), &CommandTask::abort);

	sendTask(task);
}
