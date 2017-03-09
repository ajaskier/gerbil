#ifndef DATA_REGISTER_H
#define DATA_REGISTER_H

#include <QObject>
#include <functional>

enum class AccessType;
class SubscriptionManager;
class Model;
class Dependency;
class Subscription;

class DataRegister
{

public:
	static void init(SubscriptionManager *subscriptionManager);

	static bool isInitialized(QString dataId);
	static bool isUpToDate(QString dataId);

	static int majorVersion(QString dataId);
	static int minorVersion(QString dataId);

	static void add(Model* creator,
	                QString dataId,
	                std::vector<QString> dependencies);
	static Subscription* subscribe(Dependency dependency,
	                               QObject *requester = {},
	                               std::function<void(void)> updateSlot = {});

private:

    static SubscriptionManager* sm;
    static int idCounter;
};

#endif // DATA_REGISTER_H
