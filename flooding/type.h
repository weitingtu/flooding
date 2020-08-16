#pragma once
#include <tuple>

const int radius = 3;
const int default_width = 400;
const int default_height = 400;
const int default_num_rect = 4;
const int default_num_point = 3;
const int random_seed = 0;

struct GridPointIdx
{
	GridPointIdx() : x(-1), y(-1) {}
	GridPointIdx(int x_idx, int y_idx) : x(x_idx), y(y_idx) {}
	int x;
	int y;
	bool is_valid() const { return x >= 0 && y >= 0; }
	bool operator < (const GridPointIdx& rhs) const
	{
		return std::tie(x, y) < std::tie(rhs.x, rhs.y);
	}
	bool operator == (const GridPointIdx& rhs) const
	{
		return std::tie(x, y) == std::tie(rhs.x, rhs.y);
	}
	bool operator != (const GridPointIdx& rhs) const
	{
		return std::tie(x, y) != std::tie(rhs.x, rhs.y);
	}
};

