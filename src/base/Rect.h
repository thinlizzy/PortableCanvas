#ifndef PORTABLE_CANVAS_RECT_2024_09_30_H
#define PORTABLE_CANVAS_RECT_2024_09_30_H

#include <algorithm>
#include "Dimensions.h"
#include "Point.h"

namespace pc {

struct Rect {
	Point leftTop;
	Point rightBottom;

	bool contains(Point p) const {
		return p.x >= leftTop.x && p.x <= rightBottom.x
			&& p.y >= leftTop.y && p.y <= rightBottom.y;
	}

	Rect intersect(Rect const & other) const {
		return Rect{
			.leftTop = Point{
				.x = std::max(leftTop.x,other.leftTop.x),
				.y = std::max(leftTop.y,other.leftTop.y)
			},
			.rightBottom = Point{
				.x = std::min(rightBottom.x,other.rightBottom.x),
				.y = std::min(rightBottom.y,other.rightBottom.y)
			}
		};
	}

	static auto open(Point p, Dimensions d) {
		return Rect{ .leftTop=p, .rightBottom=Point{ .x=p.x+d.width, .y=p.y+d.height } };
	}
};

}

#endif

