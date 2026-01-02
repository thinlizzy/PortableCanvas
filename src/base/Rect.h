#ifndef PORTABLE_CANVAS_RECT_2024_09_30_H
#define PORTABLE_CANVAS_RECT_2024_09_30_H

#include "Dimensions.h"
#include "Point.h"

namespace pc {

struct Rect {
	Point leftTop;
	Point rightBottom;

	static auto open(Point p, Dimensions d) {
		return Rect{ .leftTop=p, .rightBottom=Point{ .x=p.x+d.width, .y=p.y+d.height } };
	}
};

}

#endif

