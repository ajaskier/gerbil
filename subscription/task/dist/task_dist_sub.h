#ifndef TASK_DIST_SUB_H
#define TASK_DIST_SUB_H

#include <QObject>
#include <task/task.h>

#include <task/dist/task_distviewbins_tbb.h>

class TaskDistSub : public TaskDistviewBinsTbb
{
public:
	explicit TaskDistSub(QString destId, SourceDeclaration sourceId,
	                     SourceDeclaration sourceDistId,
	                     std::vector<multi_img::Value> &illuminant,
	                     bool apply = true, bool partialLabelsUpdate = false,
	                     ViewportCtx* args = nullptr);

	explicit TaskDistSub(QString destId, SourceDeclaration sourceId,
	                     std::vector<multi_img::Value> &illuminant,
	                     bool apply = true, bool partialLabelsUpdate = false,
	                     ViewportCtx* args = nullptr);

	virtual ~TaskDistSub();
	virtual bool run() override;

protected:

	std::vector<BinSet> coreExecution(ViewportCtx* args, cv::Mat1s &labels,
	                                  const QVector<QColor> &colors, cv::Mat1b &mask,
	                                  const multi_img &img,
	                                  std::vector<BinSet> *reuseDist);



	std::vector<cv::Rect> getDiff(cv::Mat1b &mask, const multi_img &img);

	bool apply;
	bool partialLabelsUpdate;

	ViewportCtx* args;
};


#endif // TASK_DIST_SUB_H
