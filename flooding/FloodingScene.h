#pragma once
#include <qgraphicsscene.h>
#include <vector>

class FloodingScene :
	public QGraphicsScene
{
public:
	FloodingScene(QObject* parent = nullptr);

	void init();
	void init_point_rect();

private:

	void add_rect();
	void add_point();
	void add_grid_line();
	void add_grid_point();
	void add_grid_seg();
};

