#include "FloorplanManager.h"
#include "type.h"
#include <queue>
#include <numeric>
#include <set>
#include <map>
#include <algorithm>

Flooding::Flooding(const std::vector<std::vector<GridPoint>>& grids):
	queue(),
	color(),
	distance(),
	predecessor(),
	predecessors()
{
	color.resize(grids.size());
	distance.resize(grids.size());
	predecessor.resize(grids.size());
	predecessors.resize(grids.size());

	for (size_t x = 0; x < grids.size(); ++x)
	{
		color.at(x).resize(grids.at(x).size());
		distance.at(x).resize(grids.at(x).size(), std::numeric_limits<int>::max());
		predecessor.at(x).resize(grids.at(x).size());
		predecessors.at(x).resize(grids.at(x).size());
	}
}

FloorplanManager::FloorplanManager():
    _site(0, 0, default_width, default_height) ,
    _num_rect(default_num_rect),
	_num_point(default_num_point),
	_rects(),
	_points(),
	_x(),
	_y(),
	_grids(),
	_sources(),
	_sources_to_complete(),
	_flooded(false),
	_backtracked(false),
	_path_shorteded(false),
	_idv_backtracking_by_pred_idx(0),
	_target_idx()
	{}

void FloorplanManager::init()
{
	_generate_rects();
	_generate_points();
	_generate_grid();

	flooding();
	complete_steiner_tree();
}

void FloorplanManager::generate(int width, int height, int num_rect, int num_point)
{
	clear();
	_site = QRect(0, 0, width, height);
	_num_rect  = num_rect;
	_num_point = num_point;

	_generate_rects();
	_generate_points();
	_generate_grid();
}

void FloorplanManager::clear()
{
	_flooded = false;
	_backtracked = false;
	_path_shorteded = false;
	_idv_backtracking_by_pred_idx = 0;
	_target_idx = GridPointIdx();
	_site = QRect();
	_num_rect = 0;
	_num_point = 0;
	_rects.clear();
	_points.clear();
	_x.clear();
	_y.clear();
	_grids.clear();
	_sources.clear();
	_sources_to_complete.clear();
}

void FloorplanManager::backtrack()
{
	if (!_target_idx.is_valid())
	{
		printf("error, target idx is not valid\n");
		return;
	}
	if (_backtracked)
	{
		return;
	}
	_backtracked = true;

	for (size_t i = 0; i < _sources.size(); ++i)
	{
		_back_trace(_target_idx, i);
	}
	_update_total_pred();
	_clear_pred();

	// update source total_pred
	for (size_t i = 0; i < _sources.size(); ++i)
	{
		_back_trace_by_pred(_target_idx, i);
	}
	_clear_pred();

	_sources_to_complete.clear();
	for (size_t i = 0; i < _sources.size(); ++i)
	{
		_sources_to_complete.push_back(IdxTotalPred(i, _get_grid_point(_sources.at(i)).total_pred));
	}
	std::sort(_sources_to_complete.begin(), _sources_to_complete.end(), std::greater<IdxTotalPred>());
}

void FloorplanManager::idv_complete_steiner_tree()
{
	if (_sources_to_complete.size() != _sources.size())
	{
		printf("sources to complete size and sources size mismatch %zu != %zu\n", _sources_to_complete.size(), _sources.size());
		return;
	}
	if(_idv_backtracking_by_pred_idx < _sources_to_complete.size())
	{
		const IdxTotalPred& id = _sources_to_complete.at(_idv_backtracking_by_pred_idx);
		_back_trace_by_pred(_target_idx, id.idx);
		++_idv_backtracking_by_pred_idx;
	}
}

void FloorplanManager::complete_steiner_tree()
{
	if (_idv_backtracking_by_pred_idx >= _sources.size())
	{
		return;
	}
	if (!_target_idx.is_valid())
	{
		printf("error, target idx is not valid\n");
		return;
	}

	_back_trace(_target_idx);
}

int FloorplanManager::get_total_pred_dis() const
{
	int total_pred_dis = 0;
	for (size_t x = 0; x < _grids.size(); ++x)
	{
		for (size_t y = 0; y < _grids.at(x).size() - 1; ++y)
		{
			GridPointIdx idx1(x, y);
			GridPointIdx idx2(x, y + 1);
	        const GridPoint& p1 = get_grids().at(idx1.x).at(idx1.y);
	        const GridPoint& p2 = get_grids().at(idx2.x).at(idx2.y);
			for (size_t i = 0; i < p1.predecessor.size(); ++i)
			{
				if (p1.predecessor.at(i) == idx2 || p2.predecessor.at(i) == idx1)
				{
					total_pred_dis += abs(p1.x - p2.x) + abs(p1.y - p2.y);
					break;
				}
			}
		}
	}

	for (size_t x = 0; x < _grids.size() - 1; ++x)
	{
		for (size_t y = 0; y < _grids.at(x).size(); ++y)
		{
			GridPointIdx idx1(x, y);
			GridPointIdx idx2(x + 1, y);
	        const GridPoint& p1 = get_grids().at(idx1.x).at(idx1.y);
	        const GridPoint& p2 = get_grids().at(idx2.x).at(idx2.y);
			for (size_t i = 0; i < p1.predecessor.size(); ++i)
			{
				if (p1.predecessor.at(i) == idx2 || p2.predecessor.at(i) == idx1)
				{
					total_pred_dis += abs(p1.x - p2.x) + abs(p1.y - p2.y);
					break;
				}
			}
		}
	}

	return total_pred_dis;
}

bool FloorplanManager::_is_in_site(const QRect& rect) const
{
	if (rect.left() <= _site.left())
	{
		return false;
	}
	if (rect.top() <= _site.top())
	{
		return false;
	}
	if (rect.right() >= _site.right())
	{
		return false;
	}
	if (rect.bottom() >= _site.bottom())
	{
		return false;
	}
	return true;
}

bool FloorplanManager::_is_overlap(const QRect& rect) const
{
	QRect r1 = rect.marginsAdded(QMargins(3, 3, 3, 3));
	for (const QRect& r : _rects)
	{
	    QRect r2 = r.marginsAdded(QMargins(3, 3, 3, 3));
		//if (r.intersects(rect))
		if(r2.intersects(r1))
		{
			return true;
		}
	}
	return false;
}

bool FloorplanManager::_is_overlap(const QPoint& p) const
{
	QPoint top(p.x(), p.y() - radius);
	QPoint bottom(p.x(), p.y() + radius);
	QPoint left(p.x() - radius, p.y());
	QPoint right(p.x() + radius, p.y());

	for (const QRect& r : _rects)
	{
		if (r.contains(p)
			|| r.contains(top)
			|| r.contains(bottom)
			|| r.contains(left)
			|| r.contains(right)
			)
		{
			return true;
		}
	}
	return false;
}

QRect FloorplanManager::_generate_random_rect() const
{
	int max_w = std::min(_get_site_width()  * 0.3, (double)_get_site_width()  / _num_rect * 10);
	int max_h = std::min(_get_site_height() * 0.3, (double)_get_site_height() / _num_rect * 10);
	int min_w = (double)_get_site_width()  / _num_rect / 2;
	int min_h = (double)_get_site_height() / _num_rect / 2;
	int x = rand() % _get_site_width() * 0.9;
	int y = rand() % _get_site_height() * 0.9;

	int w = rand() % (max_w - min_w + 1) + min_w;
	int h = rand() % (max_h - min_h + 1) + min_h;

	return QRect(x, y, w, h);
}

QPoint FloorplanManager::_generate_random_point() const
{
	int x = rand() % _get_site_width() * 0.9;
	int y = rand() % _get_site_height() * 0.9;

	return QPoint(x, y);
}

void FloorplanManager::_generate_rects()
{
	int count = 0;
	int test = 0;
	while(count < _num_rect && test < 1000)
	{
		++test;
		QRect rect = _generate_random_rect();
		if (_is_overlap(rect))
		{
			continue;
		}
		if (!_is_in_site(rect))
		{
			continue;
        }
		++count;
		_rects.push_back(rect);
	}
	if (_rects.size() != _num_rect)
	{
		printf("warning, unable to create enough obs expected (%d), created (%zu)\n", _num_rect, _rects.size());
		printf("please enlarge area\n");
		_num_rect = _rects.size();
	}
}

void FloorplanManager::_generate_points()
{
	int count = 0;
	int test = 0;
	while(count < _num_point && test < 1000)
	{
		++test;
		QPoint p = _generate_random_point();
		if (_is_overlap(p))
		{
			continue;
		}
		++count;
		_points.push_back(p);
	}
	if (_points.size() != _num_point)
	{
		printf("warning, unable to create enough terminal expected (%d), created (%zu)\n", _num_point, _points.size());
		printf("please enlarge area\n");
		_num_point = _points.size();
	}
}

void FloorplanManager::_generate_coordinate()
{
	_x.clear();
	_y.clear();
	_x.push_back(_site.left());
	_x.push_back(_site.right());
	_y.push_back(_site.top());
	_y.push_back(_site.bottom());
	for (const QRect& rect : _rects)
	{
		_x.push_back(rect.left());
		_x.push_back(rect.right());
		_y.push_back(rect.top());
		_y.push_back(rect.bottom());
	}
	for (const QPoint& p : _points)
	{
	    _x.push_back(p.x());
	    _y.push_back(p.y());
	}
	std::sort(_x.begin(), _x.end());
	_x.erase(std::unique(_x.begin(), _x.end()), _x.end());
	std::sort(_y.begin(), _y.end());
	_y.erase(std::unique(_y.begin(), _y.end()), _y.end());
}

std::vector<std::vector<GridPoint>> 
FloorplanManager::_generate_grid_point(const std::vector<int>& xx, const std::vector<int>& yy) const
{
	std::vector<std::vector<GridPoint>> grids;
	grids.resize(xx.size());

	int x_size = grids.size();
	for (size_t x = 0; x < grids.size(); ++x)
	{
		std::vector<GridPoint>& grid = grids.at(x);
		grid.resize(yy.size());
	    int y_size = grid.size();
		for (size_t y = 0; y < grids.at(x).size(); ++y)
		{
			GridPoint& p = grid.at(y);
			p.is_valid  = true;
			p.is_source = false;
			p.is_target = false;
			p.x = xx.at(x);
			p.y = yy.at(y);
			p.p = QPoint(xx.at(x), yy.at(y));
			p.idx.x = x;
			p.idx.y = y;
			int left   = x - 1 >= 0     ? x - 1 : -1;
			int right  = x + 1 < x_size ? x + 1 : -1;
			//int bottom = y - 1 >= 0     ? y - 1 : -1;
			//int top    = y + 1 < y_size ? y + 1 : -1;
			// bottom is larger than top
			int top    = y - 1 >= 0     ? y - 1 : -1;
			int bottom = y + 1 < y_size ? y + 1 : -1;
			if (bottom >= 0)
			{
				p.bottom.x = x;
				p.bottom.y = bottom;
			}
			if (top >= 0)
			{
				p.top.x = x;
				p.top.y = top;
			}
			if (left >= 0)
			{
				p.left.x = left;
				p.left.y = y;
			}
			if (right >= 0)
			{
				p.right.x = right;
				p.right.y = y;
			}
			p.distance.resize(_points.size(), std::numeric_limits<int>::max());
			p.predecessor.resize(_points.size());
			p.predecessors.resize(_points.size());
			p.pred.resize(_points.size());
			p.total_dis      = std::numeric_limits<int>::max();
			p.total_pred     = std::numeric_limits<int>::max();
			p.total_pred_dis = std::numeric_limits<int>::max();
		}
	}

	return grids;
}

int FloorplanManager::_get_x_idx(int x) const
{
    std::vector<int>::const_iterator ite = std::find(_x.begin(), _x.end(), x);
	if (ite == _x.end())
	{
		printf("error, cannot find %d in x coordinates\n", x);
		return -1;
	}
    return std::distance(_x.begin(), ite);
}

int FloorplanManager::_get_y_idx(int y) const
{
    std::vector<int>::const_iterator ite = std::find(_y.begin(), _y.end(), y);
	if (ite == _y.end())
	{
		printf("error, cannot find %d in y coordinates\n", y);
		return -1;
	}
    return std::distance(_y.begin(), ite);
}

void FloorplanManager::_add_source_grid_point()
{
	_sources.clear();
	for (size_t i = 0; i < _points.size(); ++i)
	{
		const QPoint& p = _points.at(i);
		int x_idx = _get_x_idx(p.x());
		int y_idx = _get_y_idx(p.y());
		_sources.push_back(GridPointIdx(x_idx, y_idx));
		_grids.at(x_idx).at(y_idx).is_source = true;
	}
}

void FloorplanManager::_update_grid_point_by_rect(const QRect& rect)
{
	// bottom is larger than top
	int bottom_idx = _get_y_idx(rect.bottom());
	int top_idx    = _get_y_idx(rect.top());
	int left_idx   = _get_x_idx(rect.left());
	int right_idx  = _get_x_idx(rect.right());

	// disable grid point inside rect
	for (int x = left_idx + 1; x < right_idx; ++x)
	{
		for (int y = top_idx + 1; y < bottom_idx; ++y)
		{
			_grids.at(x).at(y).is_valid = false;
		}
	}

	// disable edges cross rect
	for (int x = left_idx + 1; x < right_idx; ++x)
	{
		_grids.at(x).at(top_idx).bottom.x = -1;
		_grids.at(x).at(top_idx).bottom.y = -1;
		_grids.at(x).at(bottom_idx).top.x = -1;
		_grids.at(x).at(bottom_idx).top.y = -1;
	}

	for (int y = top_idx + 1; y < bottom_idx; ++y)
	{
		_grids.at(left_idx).at(y).right.x = -1;
		_grids.at(left_idx).at(y).right.y = -1;
		_grids.at(right_idx).at(y).left.x = -1;
		_grids.at(right_idx).at(y).left.y = -1;
	}
}

void FloorplanManager::_update_grid_point_by_rects()
{
	for (size_t i = 0; i < _rects.size(); ++i)
	{
		_update_grid_point_by_rect(_rects.at(i));
	}
}

void FloorplanManager::_generate_grid()
{
	_generate_coordinate();
	_grids = _generate_grid_point(_x, _y);
	_add_source_grid_point();
	_update_grid_point_by_rects();
}

void FloorplanManager::_init_flooding()
{
	for (size_t x = 0; x < _grids.size(); ++x)
	{
		for (size_t y = 0; y < _grids.at(x).size(); ++y)
		{
			_grids.at(x).at(y).distance.clear();
			_grids.at(x).at(y).predecessor.clear();
			_grids.at(x).at(y).predecessors.clear();
			_grids.at(x).at(y).pred.clear();
			_grids.at(x).at(y).distance.resize(_sources.size(), std::numeric_limits<int>::max());
			_grids.at(x).at(y).predecessor.resize(_sources.size());
			_grids.at(x).at(y).predecessors.resize(_sources.size());
			_grids.at(x).at(y).pred.resize(_sources.size(), 0);
		}
	}
}

GridPointIdx FloorplanManager::_find_target()
{
	GridPointIdx idx;
	int min_dis = std::numeric_limits<int>::max();
	for (size_t x = 0; x < _grids.size(); ++x)
	{
	    for (size_t y = 0; y < _grids.at(x).size(); ++y)
    	{
			GridPoint& point = _get_grid_point(x, y);
			if (!point.is_valid)
			{
				continue;
			}
			point.total_dis = 0;
			for (size_t i = 0; i < point.distance.size(); ++i)
			{
				if (point.distance.at(i) < std::numeric_limits<int>::max())
				{
					point.total_dis += point.distance.at(i);
				}
			}

			if (point.total_dis < min_dis)
			{
				min_dis = point.total_dis;
				idx.x = x;
				idx.y = y;
			}
    	}
	}

	if (idx.x >= 0 && idx.y >= 0)
	{
		GridPoint& point = _get_grid_point(idx);
		point.is_target = true;
	}

	return idx;
}

void FloorplanManager::_back_trace_by_pred(GridPointIdx idx, size_t i)
{
	const GridPointIdx& source = _sources.at(i);

	int total_pred = 0;
	int total_pred_dis = 0;
	while (idx != source)
	{
		GridPoint& point = _get_grid_point(idx);
		int pred = -1;
		int dis = 0;
		for (size_t j = 0; j < point.predecessors.at(i).size(); ++j)
		{
			const GridPointIdx& pred_idx = point.predecessors.at(i).at(j);
		    const GridPoint& pred_point  = _get_grid_point(pred_idx);
			if (pred < pred_point.total_pred)
			{
				pred = pred_point.total_pred;
				dis = abs(point.x - pred_point.x) + abs(point.y - pred_point.y);
				idx = pred_idx;
			}
		}
		point.predecessor.at(i) = idx;
		total_pred_dis += dis;
		if (idx == source)
		{
			_get_grid_point(idx).total_pred     = total_pred;
			_get_grid_point(idx).total_pred_dis = total_pred_dis;
			break;
		}
		else
		{
			total_pred += pred;
		}
	}
}

void FloorplanManager::_back_trace(const GridPointIdx& idx, size_t i)
{
	std::set<GridPointIdx> set;
	std::queue<GridPointIdx> queue;
	queue.push(idx);

	while (!queue.empty())
	{
		GridPointIdx p = queue.front();
		queue.pop();
		if (set.find(p) != set.end())
		{
			continue;
		}

		set.insert(p);
		GridPoint& point = _get_grid_point(p);
		point.pred.at(i) = 1;
		for (size_t j = 0; j < point.predecessors.at(i).size(); ++j)
		{
			queue.push(point.predecessors.at(i).at(j));
		}
	}
}

void FloorplanManager::_update_total_pred()
{
	for (size_t x = 0; x < _grids.size(); ++x)
	{
		for (size_t y = 0; y < _grids.at(x).size(); ++y)
		{
			GridPoint& p = _get_grid_point(x, y);
			if (p.is_target)
			{
				p.total_pred = 0;
			}
			else
			{
				p.total_pred = std::accumulate(p.pred.begin(), p.pred.end(), 0);
			}
		}
	}
}

void FloorplanManager::_clear_pred()
{
	for (size_t x = 0; x < _grids.size(); ++x)
	{
		for (size_t y = 0; y < _grids.at(x).size(); ++y)
		{
			GridPoint& p = _get_grid_point(x, y);
			for (size_t i = 0; i < _sources.size(); ++i)
			{
				p.predecessor.at(i) = GridPointIdx();
			}
		}
	}
}

void FloorplanManager::_back_trace(const GridPointIdx& idx)
{
	if (idx.x < 0 || idx.y < 0)
	{
		printf("error, unable to back-trace\n");
		return;
	}

	for (size_t i = 0; i < _sources.size(); ++i)
	{
		_back_trace(idx, i);
	}

	_update_total_pred();
	_clear_pred();
	_backtracked = true;
	
	for (size_t i = 0; i < _sources.size(); ++i)
	{
		_back_trace_by_pred(idx, i);
		_idv_backtracking_by_pred_idx = i + 1;
	}
}

void FloorplanManager::flooding()
{
	if (_flooded)
	{
		return;
	}
	_flooded = true;

	_init_flooding();
	for (size_t i = 0; i < _sources.size(); ++i)
	{
		_flooding(i, _sources.at(i));
	}

	_target_idx = _find_target();
}

void FloorplanManager::_flooding(const GridPointIdx& idx, const GridPoint& from, const GridPointIdx& to_idx, Flooding& f)
{
	if (to_idx.x < 0 || to_idx.y < 0)
	{
		return;
	}

	if (f.get_color(to_idx) == Color::BLACK)
	{
		return;
	}
	else if (f.get_color(to_idx) == Color::WHITE)
	{
		f.queue.push(to_idx);
		f.set_color(to_idx, Color::GRAY);
	}
	const GridPoint& to = _get_grid_point(to_idx);
	int dis = abs(from.p.x() - to.p.x()) + abs(from.p.y() - to.p.y()) + f.get_distance(idx);
	if (dis == f.get_distance(to_idx))
	{
		f.add_predecessors(to_idx, idx);
	}
	else if (dis < f.get_distance(to_idx))
	{
		f.set_distance(to_idx, dis);
		//f.set_predecessor(to_idx, idx);
		f.clear_predecessors(to_idx, idx);
		f.add_predecessors(to_idx, idx);
	}
}

void FloorplanManager::_flooding(size_t s_idx, const GridPointIdx& source)
{
	Flooding f(_grids);

	for (size_t x = 0; x < _grids.size(); ++x)
	{
		for (size_t y = 0; y < _grids.at(x).size(); ++y)
		{
			GridPoint& point = _get_grid_point(x, y);
			point.distance.at(s_idx) = std::numeric_limits<int>::max();
			point.predecessor.at(s_idx) = GridPointIdx();
			point.predecessors.at(s_idx).clear();
		}
	}

	f.queue.push(source);
	f.set_color(source, Color::GRAY);
	f.set_distance(source, 0);

	while (!f.queue.empty())
	{
		GridPointIdx idx = f.queue.front();
		f.queue.pop();
		f.set_color(idx, Color::BLACK);

		const GridPoint& point = _get_grid_point(idx);
		_flooding(idx, point, point.top, f);
		_flooding(idx, point, point.bottom, f);
		_flooding(idx, point, point.left, f);
		_flooding(idx, point, point.right, f);
	}

	for (size_t x = 0; x < _grids.size(); ++x)
	{
		for (size_t y = 0; y < _grids.at(x).size(); ++y)
		{
			GridPoint& point = _get_grid_point(x, y);
			point.distance.at(s_idx) = f.get_distance(x, y);
			point.predecessors.at(s_idx) = f.get_predecessors(x, y);
		}
	}
}

bool FloorplanManager::_has_edge(const GridPointIdx& idx1, const GridPoint& p1, int x, int y) const
{
	if (x < 0)
	{
		return false;
	}
	if (x >= _grids.size())
	{
		return false;
	}
	if (y < 0)
	{
		return false;
	}
	if (y >= _grids.at(x).size())
	{
		return false;
	}

	GridPointIdx idx2(x, y);
	const GridPoint& p2 = _get_grid_point(idx2);

	for (size_t i = 0; i < p1.predecessor.size(); ++i)
	{
		if (p1.predecessor.at(i) == idx2 || p2.predecessor.at(i) == idx1)
		{
			return true;
		}
	}
	return false;
}

size_t FloorplanManager::_get_out_edge_num(const GridPointIdx& idx ) const
{
	const GridPoint& p = _get_grid_point(idx);
	size_t count = 0;
	if (_has_edge(idx, p, idx.x - 1, idx.y))
	{
		++count;
	}
	if (_has_edge(idx, p, idx.x + 1, idx.y))
	{
		++count;
	}
	if (_has_edge(idx, p, idx.x, idx.y - 1))
	{
		++count;
	}
	if (_has_edge(idx, p, idx.x, idx.y + 1))
	{
		++count;
	}

	return count;
}

GridPointIdx FloorplanManager::_get_nearest_source(size_t i) const
{
	int min_dis = std::numeric_limits<int>::max();
	GridPointIdx min_idx = GridPointIdx();
	for (size_t x = 0; x < _grids.size(); ++x)
	{
		for (size_t y = 0; y < _grids.at(x).size(); ++y)
		{
			const GridPoint& point = _get_grid_point(x, y);
			if (!point.is_source)
			{
				continue;
			}
			if (point.distance.at(i) <= 0)
			{
				continue;
			}
			if (point.distance.at(i) < min_dis)
			{
				min_dis = point.distance.at(i);
				min_idx = GridPointIdx(x, y);
			}
		}
	}

	return min_idx;
}

void FloorplanManager::_clear_pred(size_t i)
{
	for (size_t x = 0; x < _grids.size(); ++x)
	{
		for (size_t y = 0; y < _grids.at(x).size(); ++y)
		{
			GridPoint& p = _get_grid_point(x, y);
			p.predecessor.at(i) = GridPointIdx();
		}
	}
}

void FloorplanManager::path_shortening()
{
	if (!_backtracked)
	{
		return;
	}
	if (_path_shorteded)
	{
		return;
	}
	_path_shorteded = true;

	std::map<GridPointIdx, size_t> source_map;
	std::set<size_t> single_end_sources;
	for (size_t i = 0; i < _sources.size(); ++i)
	{
		const GridPointIdx& idx = _sources.at(i);
		source_map.insert(std::make_pair(idx, i));

		size_t count = _get_out_edge_num(idx);
		if(1 == count)
		{
			single_end_sources.insert(i);
		}
	}
	int orig_dist = get_total_pred_dis();

	while(!single_end_sources.empty())
	{
		size_t i = *(single_end_sources.begin());
		single_end_sources.erase(i);

		GridPointIdx nearest_idx = _get_nearest_source(i);
		if (!nearest_idx.is_valid())
		{
			continue;
		}
		int prev_dist = get_total_pred_dis();
	    _clear_pred(i);
		_back_trace_by_pred(nearest_idx, i);
		int new_dist = get_total_pred_dis();
		if (new_dist >= prev_dist)
		{
			// no better, recover
	        _clear_pred(i);
		    _back_trace_by_pred(_target_idx, i);
			continue;
		}

		auto source_map_ite = source_map.find(nearest_idx);
		if (source_map_ite != source_map.end())
		{
			size_t nearest_i = source_map_ite->second;
			auto ite = single_end_sources.find(nearest_i);
			if (ite != single_end_sources.end())
			{
				single_end_sources.erase(ite);
			}
		}
		else
		{
			printf("%s, %d error, cannot find %d,%d in source map\n", __func__, __LINE__, nearest_idx.x, nearest_idx.y);
			continue;
		}
	}
}
