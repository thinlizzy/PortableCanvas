#ifndef PORTABLE_CANVAS_CANVAS_IMPL_2024_03_06_H
#define PORTABLE_CANVAS_CANVAS_IMPL_2024_03_06_H

#include "src/base/Canvas.h"

namespace pc {

class CanvasImpl: public Canvas {
public:
	CanvasImpl();
	~CanvasImpl() override;
	CanvasImpl & textOut(Point p, std::string_view text, Color color) override;
	CanvasImpl & line(Point p1, Point p2, Color color) override;
	CanvasImpl & drawImage(Point p, Image const & image) override;
};

}

#endif
