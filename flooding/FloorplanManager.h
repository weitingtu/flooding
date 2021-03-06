#pragma once
#include <QPoint>
#include <QRect>
#include <vector>
#include <queue>
#include "type.h"

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
	std::vector<int> distance;
	std::vector<GridPointIdx> predecessor;
	std::vector<std::vector<GridPointIdx>> predecessors;
	std::vector<int> pred;
	int total_dis;
	int total_pred;
	int total_pred_dis;
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
	std::vector<std::vector<std::vector<GridPointIdx> > > predecessors;

	Color get_color(const GridPointIdx& i) const { return color.at(i.x).at(i.y); }
	int get_distance(const GridPointIdx& i) const { return distance.at(i.x).at(i.y); }
	const GridPointIdx& get_predecessor(const GridPointIdx& i) const { return predecessor.at(i.x).at(i.y); }
	const std::vector<GridPointIdx>& get_predecessors(const GridPointIdx& i) const { return predecessors.at(i.x).at(i.y); }

	Color get_color(int x, int y) const { return color.at(x).at(y); }
	int get_distance(int x, int y) const { return distance.at(x).at(y); }
	const GridPointIdx& get_predecessor(int x, int y) const { return predecessor.at(x).at(y); }
	const std::vector<GridPointIdx>& get_predecessors(int x, int y) const { return predecessors.at(x).at(y); }

	void set_color(const GridPointIdx& i, Color c) { color.at(i.x).at(i.y) = c; }
	void set_distance(const GridPointIdx& i, int d) { distance.at(i.x).at(i.y) = d; }
	void set_predecessor(const GridPointIdx& i, const GridPointIdx& p) { predecessor.at(i.x).at(i.y) = p; }
	void add_predecessors(const GridPointIdx& i, const GridPointIdx& p) { predecessors.at(i.x).at(i.y).push_back(p); }
	void clear_predecessors(const GridPointIdx& i, const GridPointIdx& p) { predecessors.at(i.x).at(i.y).clear(); }
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
	void backtrack();
	void idv_complete_steiner_tree();
	void complete_steiner_tree();
	void path_shortening();

	const QRect& get_site() const { return _site;  }
	const std::vector<QRect>& get_rects() const { return _rects; }
	const std::vector<QPoint>& get_points() const { return _points; }
	const std::vector<int>& get_x() const { return _x; }
	const std::vector<int>& get_y() const { return _y; }
	const std::vector<std::vector<GridPoint>>& get_grids() const { return _grids; }
	const std::vector<GridPointIdx>& get_sources() const { return _sources; }
	bool is_flooded() const { return _flooded;  }
	int get_total_pred_dis() const;

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
	void _flooding(const GridPointIdx& from_idx, const GridPoint& from, const GridPointIdx& to_idx, Flooding& f);
	void _flooding(size_t s_idx, const GridPointIdx& source);
	GridPointIdx _find_target();
	bool _is_selected(GridPointIdx idx, const GridPoint& point, GridPointIdx to_idx) const;
	bool _is_selected(GridPointIdx idx, const GridPoint& point) const;
	void _back_trace_by_pred(GridPointIdx idx, size_t i, bool check_selected);
	bool _back_trace_by_pred2(GridPointIdx idx, size_t i, bool check_selected);
	void _back_trace(const GridPointIdx& idx, size_t i);
	void _back_trace(const GridPointIdx& idx );
	void _update_total_pred();
	void _clear_pred();

	bool _has_edge(const GridPointIdx& idx1, const GridPoint& p1, int x, int y) const;
	bool _has_edge(size_t x1, size_t y1, size_t x2, size_t y2) const;
	bool _check_intersection() const;
	bool _check_intersection(const GridPointIdx& idx) const;
	size_t _get_out_edge_num(const GridPointIdx& idx) const;
	GridPointIdx _get_nearest_source(size_t i) const;
	std::vector<std::pair<int, GridPointIdx> > _get_source_dist(size_t i) const;
	void _clear_pred(size_t i);
	std::vector<std::vector<GridPointIdx>> _save_pred(size_t i) const;
	void _restore_pred(size_t i, const std::vector<std::vector<GridPointIdx>>& saved_pred_idx);

	GridPoint& _get_grid_point(size_t x, size_t y) { return _grids.at(x).at(y); }
	GridPoint& _get_grid_point(const GridPointIdx& i) { return _grids.at(i.x).at(i.y); }
	const GridPoint& _get_grid_point(size_t x, size_t y) const { return _grids.at(x).at(y); }
	const GridPoint& _get_grid_point(const GridPointIdx& i) const { return _grids.at(i.x).at(i.y); }

	struct IdxTotalPred 
	{
		IdxTotalPred(size_t i, int d) : idx(i), total_pred(d) {}
		size_t idx;
		int total_pred;
		bool operator < (const IdxTotalPred& rhs) const
		{
			return std::tie(total_pred, idx) < std::tie(rhs.total_pred, rhs.idx);
		}
		bool operator > (const IdxTotalPred& rhs) const
		{
			return std::tie(total_pred, idx) > std::tie(rhs.total_pred, rhs.idx);
		}
	};
	QRect _site;
	int _num_rect;
	int _num_point;
	std::vector<QRect>  _rects;
	std::vector<QPoint> _points;
	std::vector<int> _x;
	std::vector<int> _y;
	std::vector<std::vector<GridPoint>> _grids;
	std::vector<GridPointIdx> _sources;
	std::vector<IdxTotalPred>   _sources_to_complete;
	bool _flooded;
	bool _backtracked;
	bool _path_shorteded;
	size_t _idv_backtracking_by_pred_idx;
	GridPointIdx _target_idx;

};

static FloorplanManager& get_floorplan_manager() 
{
	return FloorplanManager::get_instance();
}
