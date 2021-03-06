#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include <QObject>
#include <memory>
#include <functional>
#include <QDebug>

#include <boost/any.hpp>
#include <subscription_manager.h>
#include "dependency.h"

enum class AccessType {
	DIRECT,
	DEFERRED,
	FORCED
};

class Subscription : public QObject
{
	Q_OBJECT

public:
	virtual ~Subscription() override;
	void returnData();
	void forceUnsubscribe();

	int getId() { return id; }
	AccessType getAccessType() { return dependency.accessType; }
	Dependency getDependency() { return dependency; }
	QString getDataId() { return dependency.dataId; }

	template <class T1, class T2 = int>
	class Lock;

signals:
	void update();

private:
	explicit Subscription(Dependency dependency, /*AccessType accessType,*/
	                      int id, SubscriptionManager& sm,
	                      QObject* requester = {},
	                      std::function<void(void)> updateSlot = {});
	handle_pair leaseData();
	int version();
	void setVersion(int version);

	Dependency dependency;
	//AccessType accessType;
	SubscriptionManager& sm;
	const int id;


	static int spawnCounter;
	static int destroyCounter;

	bool forcedDelete = false;
	friend class DataRegister;
};

#endif // SUBSCRIPTION_H
