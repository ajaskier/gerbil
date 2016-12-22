#ifndef TASK_DIST_SUB_H
#define TASK_DIST_SUB_H

#include <QObject>
#include <task/task.h>

#include <task/dist/task_distviewbins_tbb.h>

class TaskDistSub : public TaskDistviewBinsTbb
{

public:
    //labels seems to be redundant!!!
    //the same with colors
    //the same with illuminant
    explicit TaskDistSub(QString destId, SourceDeclaration sourceId, SourceDeclaration sourceDistId,
                         std::vector<multi_img::Value> &illuminant,
                         const cv::Mat1b &mask = cv::Mat1b(), bool apply = true);
    virtual ~TaskDistSub();

    virtual bool run() override;

private:

    //virtual bool isCancelled() { return stopper.is_group_execution_cancelled(); }

    bool apply;


};


#endif // TASK_DIST_SUB_H
