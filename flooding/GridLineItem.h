#pragma once
#include <qgraphicsitem.h>
#include "type.h"

class GridLineItem : public QGraphicsLineItem
{
public:
	GridLineItem(const GridPointIdx& i1, const GridPointIdx& i2, const QLineF &line, QGraphicsItem *parent = nullptr) : 
		QGraphicsLineItem(line, parent),
		idx1(i1),
		idx2(i2)
	{}

	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;
private:
	GridPointIdx idx1;
	GridPointIdx idx2;
};

