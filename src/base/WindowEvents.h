#ifndef PORTABLE_CANVAS_WINDOW_EVENTS_2024_03_06_H
#define PORTABLE_CANVAS_WINDOW_EVENTS_2024_03_06_H

#include <cstdint>
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

	bool leftClick() const { return button == MouseButton::left; }
	bool rightClick() const { return button == MouseButton::right; }
};

struct KeyModifiers {
	bool controlPressed;
	bool shiftPressed;
	bool altPressed;
};

using PaintEvent = std::function<void(Canvas &,Rect)>;
using KeyDownEvent = std::function<void(Key,KeyModifiers)>;
using CharEvent = std::function<void(std::uint32_t)>;
using MouseDownEvent = std::function<void(MouseHits,Point)>;
using MouseMoveEvent = std::function<void(MouseHits,Point)>;
using MouseUpEvent = std::function<void(MouseHits,Point)>;
// return false to veto the close and keep the window alive
using CloseEvent = std::function<bool()>;
using TimerEvent = std::function<void()>;

}

#endif
