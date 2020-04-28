#pragma once
#include "type.h"
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>

class GridPointItem :
	public QGraphicsEllipseItem
{
public:
	GridPointItem(const QPoint& p, int x_idx, int y_idx, const QPen& pen = QPen(), const QBrush& brush = QBrush()) :
	QGraphicsEllipseItem(p.x() - radius, p.y() - radius, radius * 2, radius * 2 ),
		_x_idx(x_idx),
		_y_idx(y_idx)
	{
		setPen(pen);
		setBrush(brush);
	}

	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;
private:
    void _draw_text(QPainter* painter, const QRectF& text_rect, const char* text);

	const int _x_idx;
	const int _y_idx;
};

