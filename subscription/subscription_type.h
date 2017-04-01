#ifndef SUBSCRIPTION_TYPE_H
#define SUBSCRIPTION_TYPE_H

enum class SubscriptionType {
	READ,
	WRITE
};

inline QString toString(SubscriptionType s)
{
	switch (s) {
	case SubscriptionType::READ:
		return "READ";
	case SubscriptionType::WRITE:
		return "WRITE";
	}

	throw "!";
}

#endif // SUBSCRIPTION_TYPE_H
