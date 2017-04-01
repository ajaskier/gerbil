#include "task.h"

#include <QDebug>
#include <cassert>
#include "subscription.h"

Task::Task(QString target, std::map<QString, SourceDeclaration> sources)
	: id(target), sources(sources)
{
	setDependencies(target);
}

Task::Task(QString id, QString target, std::map<QString, SourceDeclaration> sources)
	: id(id), sources(sources)
{
	setDependencies(target);
}

Task::~Task()
{
	//qDebug() << "deleting task" << id;
}

void Task::setDependencies(QString target)
{
	for (auto source : sources) {
		SourceDeclaration s = source.second;
		dependencies.push_back(Dependency(s.dataId, SubscriptionType::READ, s.accessType,
		                                  s.version));
		subscriptions[source.first] = std::shared_ptr<Subscription>();
	}

	dependencies.push_back(Dependency(target, SubscriptionType::WRITE, AccessType::DIRECT));
	subscriptions["dest"] = std::shared_ptr<Subscription>();
	this->sources["dest"] = target;
}

void Task::importSubscription(std::shared_ptr<Subscription> sub)
{
	/* get global data id */
	auto id = sub->getDependency().dataId;

	/* find task-local identifier */
	//TODO: this can be done better!
	auto it = std::find_if(sources.begin(), sources.end(),
	                       [ = ](const std::pair<QString, SourceDeclaration>& v) {
		return v.second.dataId == id;
	});
	QString parsedId = it->first;

	/* add accordingly */
	subscriptions[parsedId] = sub;
}

bool Task::start()
{
	bool success = run();
	emit taskFinished(this->id, success);
	return success;
}

void Task::invalidateSubscriptions()
{
	for (auto& s : subscriptions) {
		s.second->forceUnsubscribe();
	}

	emit taskFinished(id, false);
}

bool Task::subExists(QString id)
{
	return subscriptions.find(id) != subscriptions.end();
}

std::shared_ptr<Subscription> Task::sub(QString id)
{
	auto it = subscriptions.find(id);

	assert(it != subscriptions.end());
	assert(it->second);

	return it->second;
}

QString Task::targetId()
{
	auto it = std::find_if(dependencies.begin(), dependencies.end(),
	    [ = ](const Dependency& dep) {
		    return dep.subscription == SubscriptionType::WRITE;
	    });

	if (it != dependencies.end())
		return it->dataId;
	else
		return QString();
}
