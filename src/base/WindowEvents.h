#ifndef PORTABLE_CANVAS_WINDOW_EVENTS_2024_03_06_H
#define PORTABLE_CANVAS_WINDOW_EVENTS_2024_03_06_H

#include <functional>
#include "Canvas.h"
#include "Keys.h"
#include "Point.h"
#include "Rect.h"

namespace pc {

enum class MouseButton {
	left = 1,
	right,
	middle,
	extra1,
	extra2,
};

struct MouseHits {
	MouseButton button;
	bool controlPressed;
	bool shiftPressed;
};

using PaintEvent = std::function<void(Canvas &,Rect)>;
using KeyDownEvent = std::function<void(Key)>;
using MouseDownEvent = std::function<void(MouseHits,Point)>;

}

#endif
