#ifndef TASK_DIST_ADD_H
#define TASK_DIST_ADD_H

#include <QObject>
#include <task/task.h>

#include <task/task_distviewbins_tbb.h>

class TaskDistAdd : public TaskDistviewBinsTbb
{

public:
    explicit TaskDistAdd(QString destId, QString sourceImgId,
                         ViewportCtx &args,
                         const cv::Mat1s &labels,
                         QVector<QColor> &colors,
                         std::vector<multi_img::Value> &illuminant,
                         const cv::Mat1b &mask = cv::Mat1b(), bool apply = true);

    explicit TaskDistAdd(QString destId,
                         QString sourceImgId,
                         QString sourceTempId,
                         ViewportCtx &args,
                         const cv::Mat1s &labels,
                         QVector<QColor> &colors,
                         std::vector<multi_img_base::Value> &illuminant,
                         const cv::Mat1b &mask, bool apply = true);

    virtual ~TaskDistAdd();

    virtual bool run() override;

private:

    //virtual bool isCancelled() { return stopper.is_group_execution_cancelled(); }

    bool apply;
    ViewportCtx &args;

};


#endif // TASK_DIST_ADD_H
