#ifndef LABELS_MODEL_H
#define LABELS_MODEL_H

#include <QColor>
#include <QVector>

#include <model/representation.h>
#include <QObject>
#include <opencv2/core/core.hpp>
#include <model/model.h>
#include "subscription.h"


class LabelsModel : public Model
{
    Q_OBJECT

public:
    explicit LabelsModel(SubscriptionManager& sm,
                        TaskScheduler* scheduler, QObject *parent = nullptr);

    virtual void delegateTask(QString id, QString parentId = "") override;

private:


};

#endif // LABELS_MODEL_H
