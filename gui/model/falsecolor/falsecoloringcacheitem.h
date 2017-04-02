#ifndef FALSECOLORINGCACHEITEM_H
#define FALSECOLORINGCACHEITEM_H

#include <QPixmap>

/** Cache item for computed false color images. */
class FalseColoringCacheItem {
public:
	FalseColoringCacheItem() : pixmap_() {}  // invalid cache item
	FalseColoringCacheItem(QPixmap img) : pixmap_(img)  {} // valid cache item

	// Default copy constructor and assignment operator.

	void invalidate() { pixmap_ = QPixmap(); }
	bool valid() { return ! pixmap_.isNull(); }
	QPixmap pixmap() { return pixmap_; }
private:
	QPixmap pixmap_;
};

#endif // FALSECOLORINGCACHEITEM_H
