#ifndef SOURCE_DECLARATION_H
#define SOURCE_DECLARATION_H

#include <QString>
#include "subscription.h"

struct SourceDeclaration
{
	SourceDeclaration() {}
	SourceDeclaration(QString dataId)
		: dataId(dataId)
	{}

	SourceDeclaration(QString dataId, int version)
		: dataId(dataId), version(version)
	{}

	SourceDeclaration(QString dataId, AccessType accessType)
		: dataId(dataId), accessType(accessType)
	{}

	SourceDeclaration(QString dataId, int version, AccessType accessType)
		: dataId(dataId), version(version), accessType(accessType)
	{}

	operator QString&() { return dataId; }

	QString    dataId     = "";
	int        version    = -1;
	AccessType accessType = AccessType::DIRECT;
};

#endif // SOURCE_DECLARATION_H
