#ifndef TASK_DIST_ADD_H
#define TASK_DIST_ADD_H

#include <QObject>
#include <task/task.h>

#include <task/dist/task_distviewbins_tbb.h>

class TaskDistAdd : public TaskDistviewBinsTbb
{

public:
    explicit TaskDistAdd(QString destId, SourceDeclaration sourceImgId,
                         std::vector<multi_img::Value> &illuminant, bool apply,
                         bool partialLabelsUpdate = false,
                         ViewportCtx* args = nullptr);

    explicit TaskDistAdd(QString destId,
                         SourceDeclaration sourceImgId, SourceDeclaration sourceTempId,
                         std::vector<multi_img_base::Value> &illuminant, bool apply,
                         bool partialLabelsUpdate = false,
                         ViewportCtx* args = nullptr);

    virtual ~TaskDistAdd();
    virtual bool run() override;

protected:

    std::vector<BinSet> coreExecution(ViewportCtx* args, cv::Mat1s &labels,
                                      QVector<QColor> &colors, cv::Mat1b &mask);

    bool apply;
    bool partialLabelsUpdate;
    ViewportCtx* args;

};

#endif // TASK_DIST_ADD_H
