#ifndef PORTABLE_CANVAS_CANVAS_2024_01_25_H
#define PORTABLE_CANVAS_CANVAS_2024_01_25_H

#include <string_view>
#include "Color.h"
#include "Point.h"

namespace pc {

class Canvas {
public:
	virtual ~Canvas() = 0;

	virtual Canvas & textOut(Point p, std::string_view text, Color color) = 0;

	virtual Canvas & line(Point p1, Point p2, Color color) = 0;
};

}

#endif
