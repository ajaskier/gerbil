#ifndef TASK_DIST_ADD_LABELS_PARTIAL_H
#define TASK_DIST_ADD_LABELS_PARTIAL_H

#include <QObject>
#include <task/task.h>

#include <task/dist/task_distviewbins_tbb.h>

class TaskDistAddLabelsPartial : public TaskDistviewBinsTbb
{

public:
    explicit TaskDistAddLabelsPartial(QString destId,
                                        SourceDeclaration sourceImgId,
                                        SourceDeclaration sourceTempId,
                                        std::vector<multi_img_base::Value> &illuminant,
                                        bool apply);

    virtual ~TaskDistAddLabelsPartial();

    virtual bool run() override;

protected:

    std::vector<BinSet> coreExecution(ViewportCtx* args, cv::Mat1s &labels,
                                      QVector<QColor> &colors, cv::Mat1b &mask);

    bool apply;

};


#endif // TASK_DIST_ADD_LABELS_PARTIAL_H
