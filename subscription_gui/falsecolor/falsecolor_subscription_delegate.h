#ifndef FALSECOLOR_SUBSCRIPTION_DELEGATE_H
#define FALSECOLOR_SUBSCRIPTION_DELEGATE_H

#include "ui_falsecolordock_sel.h"

#include "model/falsecolor/falsecoloring.h"
#include "falsecolordock.h"

#include "subscription.h"
#include <memory>
#include <subscription_manager.h>

class FalseColorSubscriptionDelegate : public QObject {
	Q_OBJECT
public:
	explicit FalseColorSubscriptionDelegate(FalseColoring::Type coloringType, QObject *parent);

	void invalidate();
	void establish();
	bool established() { return sub.get(); }

	Subscription* subscription() { if (sub) return sub.get(); else return nullptr; }

signals:
	void coloringUpdated(FalseColoring::Type coloringType);

private:
	void updated();
	FalseColoring::Type coloringType;
	std::unique_ptr<Subscription> sub;
};

#endif // FALSECOLOR_SUBSCRIPTION_DELEGATE_H
