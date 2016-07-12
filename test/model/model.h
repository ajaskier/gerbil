#ifndef MODEL_H
#define MODEL_H

#include <QObject>

class SubscriptionManager;
class TaskScheduler;
enum class Subscription;
enum class DataId;

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = 0) : QObject(parent) {}

public slots:
    virtual void delegateTask() = 0;

protected:
    void registerData(DataId dataId);

};

class Signature {

public:
    Signature(DataId dataId, Subscription subscription)
        : dataId(dataId), subscription(subscription) {}

    DataId dataId;
    Subscription subscription;
};

class Task : public QObject
{
    Q_OBJECT

public:
    explicit Task(DataId id, QObject *parent = 0) : id(id), QObject(parent)  {}
    virtual void start() {
        run();
        emit finished(id);
    }
    std::vector<Signature>& getSignatures() { return signatures; }
    DataId getId() { return id; }

signals:
    void finished(DataId id);

protected:
    virtual void run() = 0;

    std::vector<Signature> signatures;
    DataId id;

};

#endif // MODEL_FIRST_H
