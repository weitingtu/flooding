#include "GridPointItem.h"
#include "FloorplanManager.h"
#include <QPainter>

void GridPointItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
	painter->save();
	const FloorplanManager& m = get_floorplan_manager();
	const GridPoint& p = m.get_grids().at(_x_idx).at(_y_idx);
	if (!p.is_valid)
	{
		QPen pen(Qt::gray, 1, Qt::SolidLine);
		QBrush brush;
		brush.setColor(Qt::gray);
		painter->setPen(pen);
		painter->setBrush(brush);
	    painter->drawEllipse(rect());
	}
	else if (p.is_source)
	{
    	QPen pen(Qt::red, 1, Qt::SolidLine);
		QBrush brush;
		brush.setColor(Qt::red);
		painter->setPen(pen);
		painter->setBrush(brush);
	    painter->drawEllipse(rect());
	}
	else if (p.is_target)
	{
    	QPen pen(Qt::blue, 1, Qt::SolidLine);
		QBrush brush;
		brush.setColor(Qt::blue);
		painter->setPen(pen);
		painter->setBrush(brush);
	    painter->drawEllipse(rect());
	}
	else
	{
		QPen pen(Qt::cyan, 1, Qt::SolidLine);
		QBrush brush;
		brush.setColor(Qt::cyan);
		painter->setPen(pen);
		painter->setBrush(brush);
	    painter->drawEllipse(rect());
	}
	//painter->drawEllipse(rect());
	painter->restore();

	//std::string text = std::to_string(_x_idx) + "," + std::to_string(_y_idx);
	//text += " ";
	//text += std::to_string(p.distance.at(0));
	//std::string text = std::to_string(p.distance.at(0));
	//std::string text = std::to_string(p.distance.at(1));
	//std::string text = std::to_string(p.distance.at(0));
	std::string text = std::to_string(p.total_dis);
	_draw_text(painter, rect(), text.c_str());
}

void GridPointItem::_draw_text(QPainter* painter, const QRectF& text_rect, const char* text)
{
	QFont font = painter->font();
	font.setWeight(15);
	QFontMetrics fm(font);
	QRect text_box = fm.boundingRect(QString(text));

	painter->save();
	painter->setFont(font);

	int width = (int)text_rect.width();
	int height = (int)text_rect.height();

	double ratio = std::min(0.7 * width / text_box.width(), 0.7 * height / text_box.height());
	painter->setFont(font);
	int x = (int)text_rect.left();
	int y = (int)text_rect.top();
	painter->translate(x, y);
	painter->scale(ratio, ratio);
	painter->drawText(0, 0, (int)(width / ratio), (int)(height / ratio), Qt::AlignCenter, QString(text));

	painter->restore();
}
