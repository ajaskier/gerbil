#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <utility>
#include <memory>
#include "task/task.h"

class TaskScheduler;
enum class SubscriptionType;

class Model : public QObject
{
	Q_OBJECT

public:
	explicit Model(TaskScheduler* scheduler,
	               QObject      *parent = 0);
	virtual ~Model();

public slots:
	virtual void delegateTask(QString requestedId, QString parentId = "") = 0;
	virtual void taskFinished(QString id, bool success);

protected:
	void registerData(QString dataId, std::vector<QString> dependencies);
	bool isTaskCurrent(QString id);

	// send a prepared task
	virtual void sendTask(std::shared_ptr<Task> t);

	// construct task of type T and send it right away
	template<typename T, typename ... A>
	void sendTask(A && ... args)
	{
		sendTask(std::make_shared<T>(std::forward<A>(args) ...));
	}

private:
	TaskScheduler* scheduler;
	std::map<QString, std::shared_ptr<Task> > tasks;
};

#endif // MODEL_H
