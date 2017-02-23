#ifndef TASK_DISTVIEWBINS_TBB_H
#define TASK_DISTVIEWBINS_TBB_H

#include <QObject>
#include <task/task.h>
#include <tbb/tbb.h>

#include <tbb/parallel_for.h>
#include <multi_img.h>

#include <opencv2/core/core.hpp>

#include <QVector>
#include <QColor>

class BinSet;
class ViewportCtx;

class TaskDistviewBinsTbb : public Task
{

public:
    explicit TaskDistviewBinsTbb(QString id, QString target, std::map<QString, SourceDeclaration> sources,
//                             const cv::Mat1s &labels,
//                             QVector<QColor> &colors,
                             std::vector<multi_img::Value> &illuminant,
                             //ViewportCtx &args,
                             const cv::Mat1b &mask = cv::Mat1b());//,
                             //bool inplace = false, bool apply = true);
    virtual ~TaskDistviewBinsTbb();

    virtual bool run() override;

protected:

    void expression(bool subtract, std::vector<cv::Rect> &collection,
                    multi_img &multi, std::vector<BinSet> &sets, cv::Mat1s &labels,
                    const ViewportCtx *args);
    void createBinSets(multi_img &multi, QVector<QColor> &colors, std::vector<BinSet> &sets);
    void updateContext(multi_img &multi, ViewportCtx *args);
    virtual bool isCancelled() { return stopper.is_group_execution_cancelled(); }

private:

    tbb::task_group_context stopper;

    const cv::Mat1b mask;
    std::vector<multi_img::Value> illuminant;

protected:
//    cv::Mat1s labels;
//    QVector<QColor> colors;

};


#endif // TASK_DISTVIEWBINS_TBB_H
