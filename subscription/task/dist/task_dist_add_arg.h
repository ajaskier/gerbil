#ifndef TASK_DIST_ADD_ARG_H
#define TASK_DIST_ADD_ARG_H

#include <QObject>
#include <task/task.h>

#include <task/dist/task_distviewbins_tbb.h>

class TaskDistAddArg : public TaskDistviewBinsTbb
{

public:
    explicit TaskDistAddArg(QString destId, QString sourceImgId,
                         ViewportCtx *args,
                         std::vector<multi_img::Value> &illuminant,
                         const cv::Mat1b &mask = cv::Mat1b(), bool apply = true);

    explicit TaskDistAddArg(QString destId,
                         QString sourceImgId,
                         QString sourceTempId,
                         ViewportCtx *args,
                         std::vector<multi_img_base::Value> &illuminant,
                         const cv::Mat1b &mask, bool apply = true);

    virtual ~TaskDistAddArg();

    virtual bool run() override;

private:

    //virtual bool isCancelled() { return stopper.is_group_execution_cancelled(); }

    bool apply;
    ViewportCtx *args;

};


#endif // TASK_DIST_ADD_ARG_H
