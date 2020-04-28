#pragma once
#include <QPoint>
#include <QRect>
#include <vector>
#include <queue>

	struct GridPointIdx
	{
		GridPointIdx() : x(-1), y(-1) {}
		GridPointIdx(int x_idx, int y_idx) : x(x_idx), y(y_idx) {}
		int x;
		int y;
	};
	struct GridPoint {
		bool is_valid;
		bool is_source;
		bool is_target;
		int x;
		int y;
		QPoint p;
		GridPointIdx idx;
		GridPointIdx top;
		GridPointIdx bottom;
		GridPointIdx right;
		GridPointIdx left;
		std::vector<int>   distance;
		std::vector<GridPointIdx> predecessor;
		int total_dis;
	};

enum class Color
{
	WHITE,
	GRAY,
	BLACK
};

class Flooding {
public:
	Flooding(const std::vector<std::vector<GridPoint>>& grids);

	std::queue<GridPointIdx> queue;
	std::vector<std::vector<Color> > color;
	std::vector<std::vector<int> >   distance;
	std::vector<std::vector<GridPointIdx> > predecessor;

	Color get_color(const GridPointIdx& i) const { return color.at(i.x).at(i.y); }
	int get_distance(const GridPointIdx& i) const { return distance.at(i.x).at(i.y); }
	const GridPointIdx& get_predecessor(const GridPointIdx& i) const { return predecessor.at(i.x).at(i.y); }

	Color get_color(int x, int y) const { return color.at(x).at(y); }
	int get_distance(int x, int y) const { return distance.at(x).at(y); }
	const GridPointIdx& get_predecessor(int x, int y) const { return predecessor.at(x).at(y); }

	void set_color(const GridPointIdx& i, Color c) { color.at(i.x).at(i.y) = c; }
	void set_distance(const GridPointIdx& i, int d) { distance.at(i.x).at(i.y) = d; }
	void set_predecessor(const GridPointIdx& i, const GridPointIdx& p) { predecessor.at(i.x).at(i.y) = p; }
};

class FloorplanManager
{
public:
	static FloorplanManager& get_instance()
	{
		static FloorplanManager inst;
		return inst;
	}

	FloorplanManager();

	void init();
	void generate(int width, int height, int num_rect, int num_point);
	void clear();
	void flooding();

	const QRect& get_site() const { return _site;  }
	const std::vector<QRect>& get_rects() const { return _rects; }
	const std::vector<QPoint>& get_points() const { return _points; }
	const std::vector<int>& get_x() const { return _x; }
	const std::vector<int>& get_y() const { return _y; }
	const std::vector<std::vector<GridPoint>>& get_grids() const { return _grids; }

private:
	QRect  _generate_random_rect() const;
	QPoint _generate_random_point() const;

	bool _is_in_site(const QRect& rect) const;
	bool _is_overlap(const QRect& rect) const;
	bool _is_overlap(const QPoint& p) const;
	void _generate_rects();
	void _generate_points();
	void _generate_coordinate();
	void _generate_grid();
	std::vector<std::vector<GridPoint>>
		_generate_grid_point(const std::vector<int>& x, const std::vector<int>& y) const;
	void _add_source_grid_point();
	void _update_grid_point_by_rect(const QRect& rect);
	void _update_grid_point_by_rects();

	int _get_site_height() const { return _site.height(); }
	int _get_site_width() const { return _site.width(); }
	int _get_x_idx(int x) const;
	int _get_y_idx(int x) const;

	void _init_flooding();
	void _flooding();
	void _flooding(const GridPointIdx& from_idx, const GridPoint& from, const GridPointIdx& to_idx, Flooding& f);
	void _flooding(size_t s_idx, const GridPointIdx& source);
	void _find_target();

	GridPoint& _get_grid_point(size_t x, size_t y) { return _grids.at(x).at(y); }
	GridPoint& _get_grid_point(const GridPointIdx& i) { return _grids.at(i.x).at(i.y); }

	QRect _site;
	int _num_rect;
	int _num_point;
	std::vector<QRect>  _rects;
	std::vector<QPoint> _points;
	std::vector<int> _x;
	std::vector<int> _y;
	std::vector<std::vector<GridPoint>> _grids;
	std::vector<GridPointIdx> _sources;
};

static FloorplanManager& get_floorplan_manager() 
{
	return FloorplanManager::get_instance();
}
