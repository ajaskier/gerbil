#ifndef TASK_DIST_SUB_ARG_H
#define TASK_DIST_SUB_ARG_H

#include <QObject>
#include <task/task.h>

#include "task/dist/task_dist_sub.h"

class TaskDistSubArg : public TaskDistSub
{

public:
    //labels seems to be redundant!!!
    //the same with colors
    //the same with illuminant
    explicit TaskDistSubArg(QString destId, SourceDeclaration sourceId,
                         ViewportCtx *args,
                         std::vector<multi_img::Value> &illuminant,
                        /* const cv::Mat1b &mask = cv::Mat1b(),*/ bool apply = true);
    virtual ~TaskDistSubArg();

    virtual bool run() override;

private:

    //virtual bool isCancelled() { return stopper.is_group_execution_cancelled(); }

    bool apply;
    ViewportCtx *args;

};


#endif // TASK_DIST_SUB_ARG_H
