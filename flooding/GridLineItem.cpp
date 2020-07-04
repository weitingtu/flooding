#include "GridLineItem.h"
#include "FloorplanManager.h"
#include "DisplayManager.h"
#include <QPainter>

void GridLineItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    const DisplayManager& dm = get_display_manager();
	painter->save();
	const FloorplanManager& m = get_floorplan_manager();

	const GridPoint& p1 = m.get_grids().at(idx1.x).at(idx1.y);
	const GridPoint& p2 = m.get_grids().at(idx2.x).at(idx2.y);
	bool black = false;
	for (size_t i = 0; i < m.get_sources().size(); ++i)
	{
		if (p1.predecessor.at(i) == idx2
			|| p2.predecessor.at(i) == idx1)
		{
			black = true;
			break;
	    }
	}
	if (black)
	{
		QPen pen(Qt::black);
		painter->setPen(pen);
	}
	else
	{
		painter->setPen(pen());
	}

	if (!dm.is_show_grid_line())
	{
		if (black)
		{
		    painter->drawLine(line());
		}
	}
	else
	{
		painter->drawLine(line());
	}


	painter->restore();
}

