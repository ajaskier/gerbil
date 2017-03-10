#ifndef TASK_DIST_SUB_LABELS_PARTIAL_H
#define TASK_DIST_SUB_LABELS_PARTIAL_H

#include <QObject>
#include <task/task.h>

#include <task/dist/task_distviewbins_tbb.h>

class TaskDistSubLabelsPartial : public TaskDistviewBinsTbb
{

public:
    explicit TaskDistSubLabelsPartial(QString destId, SourceDeclaration sourceId, SourceDeclaration sourceDistId,
                                         std::vector<multi_img::Value> &illuminant, bool apply = true);
    virtual ~TaskDistSubLabelsPartial();

    virtual bool run() override;

protected:

    std::vector<BinSet> coreExecution(ViewportCtx* args, cv::Mat1s &labels,
                                      QVector<QColor> &colors, cv::Mat1b &mask);

    bool apply;


};


#endif // TASK_DIST_SUB_LABELS_PARTIAL_H
