#include "falsecoloring.h"

QList<FalseColoring::Type> FalseColoring::allList = QList<FalseColoring::Type>()
                                                    << FalseColoring::CMF
                                                    << FalseColoring::PCA
                                                    << FalseColoring::PCAGRAD
                                                    << FalseColoring::SOM
                                                    << FalseColoring::SOMGRAD;

std::ostream &operator<<(std::ostream& os, const FalseColoring::Type& coloringType)
{
	if (coloringType < 0 ||
	    coloringType >= FalseColoring::Type(FalseColoring::SIZE)) {
		os << "INVALID";
		return os;
	}
	const char * const str[] = { "CMF", "PCA", "PCAGRAD", "SOM", "SOMGRAD" };
	os << str[coloringType];
	return os;
}
