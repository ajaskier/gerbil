#include "falsecolor_model.h"

#include "task_scheduler.h"
#include "task/task.h"

#include "data_register.h"
#include "data.h"
#include "lock.h"

#include "qtopencv.h"

#include <QVector>

#include "task/task_rgb_display.h"

QString FalsecolorModel::coloringTypeToString(const FalseColoring::Type &coloringType)
{
	switch (coloringType) {
	case FalseColoring::CMF:
		return QString("CMF");
	case FalseColoring::PCA:
		return QString("PCA");
	case FalseColoring::PCAGRAD:
		return QString("PCAGRAD");
	case FalseColoring::SOM:
		return QString("SOM");
	case FalseColoring::SOMGRAD:
		return QString("SOMGRAD");
	}
}

FalseColoring::Type FalsecolorModel::stringToColoringType(QString coloringType)
{
	if (coloringType == "falsecolor.CMF") {
		return FalseColoring::CMF;
	} else if (coloringType == "falsecolor.PCA") {
		return FalseColoring::PCA;
	} else if (coloringType == "falsecolor.PCAGRAD") {
		return FalseColoring::PCAGRAD;
	} else if (coloringType == "falsecolor.SOM") {
		return FalseColoring::SOM;
	} else if (coloringType == "falsecolor.SOMGRAD") {
		return FalseColoring::SOMGRAD;
	}
}

FalsecolorModel::FalsecolorModel(TaskScheduler *scheduler, QObject *parent)
    : Model(scheduler, parent)
{
	registerData("falsecolor.CMF", { "image.IMG" });
	registerData("falsecolor.PCA", { "image.IMG" });
	registerData("falsecolor.SOM", { "image.IMG" });
	registerData("falsecolor.PCAGRAD", { "image.GRAD" });
	registerData("falsecolor.SOMGRAD", { "image.GRAD" });
}

void FalsecolorModel::delegateTask(QString id, QString parentId)
{
	requestColoring(stringToColoringType(id));
}


void FalsecolorModel::requestColoring(FalseColoring::Type coloringType, bool recalc)
{
	qDebug() << "request for" << coloringType;

	//cache thing
	//TBD if neccessary now

	computeColoring(coloringType);
}

void FalsecolorModel::setupConfig(rgb::RGBDisplay *cmd, FalseColoring::Type coloringType)
{
	switch (coloringType) {
	case FalseColoring::CMF:
		cmd->config.algo = rgb::COLOR_XYZ;
		break;
	case FalseColoring::PCA:
	case FalseColoring::PCAGRAD:
		cmd->config.algo = rgb::COLOR_PCA;
		break;
#ifdef WITH_SOM
	case FalseColoring::SOM:
	case FalseColoring::SOMGRAD:
		// default parameters for false coloring (different to regular defaults)
		cmd->config.algo        = rgb::COLOR_SOM;
		cmd->config.som.maxIter = 50000;
		cmd->config.som.seed    = time(NULL);

		// CUBE parameters
		cmd->config.som.type       = som::SOM_CUBE;
		cmd->config.som.dsize      = 10;
		cmd->config.som.sigmaStart = 4;
		cmd->config.som.sigmaEnd   = 1;
		cmd->config.som.learnStart = 0.75;
		cmd->config.som.learnEnd   = 0.01;

		break;
#endif  /* WITH_SOM */
	default:
		assert(false);
	}
}

void FalsecolorModel::computeColoring(FalseColoring::Type coloringType)
{
	rgb::RGBDisplay *cmd = new rgb::RGBDisplay();
	setupConfig(cmd, coloringType);

	QString imgDependency;
	if (FalseColoring::isBasedOn(coloringType, representation::IMG)) {
		imgDependency = "image.IMG";
	} else {
		imgDependency = "image.GRAD";
	}

	std::shared_ptr<TaskRGBDisplay> task(new TaskRGBDisplay("falsecolor." + coloringTypeToString(
	                                                            coloringType), imgDependency,
	                                                        coloringType));
	task->setCommand(cmd);

	connect(task.get(), SIGNAL(progressChanged(int)),
	        this, SIGNAL(progressChanged(int)), Qt::QueuedConnection);

	connect(this, &FalsecolorModel::abort,
	        task.get(), &CommandTask::abort);

	sendTask(task);
}

void FalsecolorModel::taskFinished(QString id, bool success)
{
	Model::taskFinished(id, success);

	emit coloringCompleted();
}
