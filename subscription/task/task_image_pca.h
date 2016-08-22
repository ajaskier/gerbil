#ifndef TASK_IMAGE_IMGPCA_H
#define TASK_IMAGE_IMGPCA_H

#include <QObject>
#include <task/task.h>
#include <opencv2/core/core.hpp>
#include <multi_img.h>
#include "model/representation.h"

class TaskImagePCA : public Task
{

public:
    explicit TaskImagePCA(QString sourceId, QString destId, multi_img::NormMode normMode,
                          multi_img_base::Range normRange,
                          representation::t type, bool update,
                          unsigned int components = 0, bool includecache = true);
    virtual ~TaskImagePCA();
    virtual bool run() override;

private:

    bool includecache;
    unsigned int components;

    //normalization
    multi_img::NormMode normMode;
    multi_img_base::Range normRange;
    representation::t type;
    bool update;
};


#endif // TASK_IMAGE_IMGPCA_H
