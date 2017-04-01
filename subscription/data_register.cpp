#include "data_register.h"

#include "subscription_manager.h"
#include "dependency.h"
#include "subscription.h"
#include <QDebug>

int DataRegister::idCounter = 0;
SubscriptionManager* DataRegister::sm = nullptr;

void DataRegister::init(SubscriptionManager* subscriptionManager)
{
	sm = subscriptionManager;
}

void DataRegister::add(Model *creator, QString dataId, std::vector<QString> dependencies)
{
	sm->registerData(creator, dataId, dependencies);
}

Subscription* DataRegister::subscribe(Dependency            dependency,
//                                          AccessType accessType,
                                      QObject               *requester,
                                      std::function<void()> updateSlot)
{
	assert(sm);
	return new Subscription(dependency, /* accessType,*/ idCounter++, *sm,
	                        requester, updateSlot);
}

bool DataRegister::isInitialized(QString dataId)
{
	return sm->isDataInitialized(dataId);
}

bool DataRegister::isUpToDate(QString dataId)
{
	return sm->isUpToDate(dataId);
}

int DataRegister::majorVersion(QString dataId)
{
	return sm->getMajorVersion(dataId);
}

int DataRegister::minorVersion(QString dataId)
{
	return sm->getMinorVersion(dataId);
}
