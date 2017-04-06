#include "falsecolor_subscription_delegate.h"

#include "model/falsecolor_model.h"
#include "model/falsecolor/falsecoloring.h"
#include "data_register.h"
#include "subscription.h"

#include <QDebug>

FalseColorSubscriptionDelegate::FalseColorSubscriptionDelegate(FalseColoring::Type coloringType,
                                                               QObject             *parent)
    : QObject(parent), coloringType(coloringType)
{}

void FalseColorSubscriptionDelegate::invalidate()
{
	qDebug() << "invalidating sub" << FalsecolorModel::coloringTypeToString(coloringType);
	sub.reset(nullptr);
}

void FalseColorSubscriptionDelegate::establish()
{
	qDebug() << "establishing sub" << FalsecolorModel::coloringTypeToString(coloringType);

	QString dataSub = "falsecolor." + FalsecolorModel::coloringTypeToString(coloringType);
	sub = std::unique_ptr<Subscription>(DataRegister::subscribe(Dependency(dataSub,
	                                                                       SubscriptionType::READ,
	                                                                       AccessType::DEFERRED),
	                                                            this,
	                                                            std::bind(&
	                                                                      FalseColorSubscriptionDelegate
	                                                                      ::
	                                                                      updated, this)));
}

void FalseColorSubscriptionDelegate::updated()
{
	emit coloringUpdated(coloringType);
}
