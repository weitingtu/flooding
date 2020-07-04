#include "FloodingScene.h"
#include "type.h"
#include "FloorplanManager.h"
#include "GridPointItem.h"
#include "GridLineItem.h"

FloodingScene::FloodingScene(QObject* parent ) : QGraphicsScene(parent)
{}

void FloodingScene::init()
{
	clear();
	const FloorplanManager& m = get_floorplan_manager();
	addRect(m.get_site());
	add_rect();
	add_grid_seg();
	add_grid_point();
}

void FloodingScene::init_point_rect()
{
	clear();
	const FloorplanManager& m = get_floorplan_manager();
	addRect(m.get_site());
	add_rect();
	add_point();
}

void FloodingScene::add_rect()
{
	const FloorplanManager& m = get_floorplan_manager();
	QPen pen(Qt::cyan);
	pen.setWidth(0);
	QBrush brush(Qt::cyan);
	for (QRect rect : m.get_rects())
	{
		rect = rect.marginsRemoved(QMargins(0, 0, 1, 1));
		addRect(rect, pen, brush);
	}
}

void FloodingScene::add_point()
{
	const FloorplanManager& m = get_floorplan_manager();
    QPen pen = QPen(Qt::red, 1, Qt::SolidLine);
	QBrush brush;
	for (const QPoint& p : m.get_points())
	{
		addEllipse(p.x() - radius, p.y() - radius , radius * 2, radius * 2, pen, brush);
	}
}

void FloodingScene::add_grid_line()
{
	const FloorplanManager& m = get_floorplan_manager();
	QPen pen;
	pen.setColor("gray");
	for (int x : m.get_x())
	{
		addLine(x, m.get_site().top(), x, m.get_site().bottom(), pen);
	}
	for (int y : m.get_y())
	{
		addLine(m.get_site().left(), y, m.get_site().right(), y, pen);
	}
}

void FloodingScene::add_grid_point()
{
	const FloorplanManager& m = get_floorplan_manager();
	QPen pen;
	QBrush brush(Qt::cyan);
	for (size_t x = 0; x < m.get_grids().size(); ++x)
	{
		for (size_t y = 0; y < m.get_grids().at(x).size(); ++y)
		{
			const GridPoint& p = m.get_grids()[x][y];
			if (!p.is_valid)
			{
				continue;
			}
			GridPointItem* item = new GridPointItem(p.p, p.idx.x, p.idx.y);
			addItem(item);
		}
	}
}

void FloodingScene::add_grid_seg()
{
	const FloorplanManager& m = get_floorplan_manager();
	QPen pen(Qt::gray);
	for (size_t x = 0; x < m.get_grids().size() - 1; ++x)
	{
		for (size_t y = 0; y < m.get_grids().at(x).size(); ++y)
		{
			const GridPoint& p1 = m.get_grids()[x][y];
			const GridPoint& p2 = m.get_grids()[x + 1][y];
			if (!p1.is_valid || !p2.is_valid)
			{
				continue;
			}
			if (p1.right.x < 0)
			{
				continue;
			}
			GridLineItem* item = new GridLineItem(p1.idx, p2.idx, QLineF(p1.p, p2.p));
			item->setPen(pen);
			addItem(item);
		}
	}
	for (size_t x = 0; x < m.get_grids().size(); ++x)
	{
		for (size_t y = 0; y < m.get_grids().at(x).size() - 1; ++y)
		{
			const GridPoint& p1 = m.get_grids()[x][y];
			const GridPoint& p2 = m.get_grids()[x][y+1];
			if (!p1.is_valid || !p2.is_valid)
			{
				continue;
			}
			if (p1.bottom.x < 0)
			{
				continue;
			}
			GridLineItem* item = new GridLineItem(p1.idx, p2.idx, QLineF(p1.p, p2.p));
			item->setPen(pen);
			addItem(item);
		}
	}
}

