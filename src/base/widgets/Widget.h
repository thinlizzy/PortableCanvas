#ifndef PORTABLE_CANVAS_WIDGET_2026_11_24_H
#define PORTABLE_CANVAS_WIDGET_2026_11_24_H

#include <cstdint>
#include "portablecanvas/Canvas.h"
#include "portablecanvas/Font.h"
#include "portablecanvas/Point.h"
#include "portablecanvas/Rect.h"
#include "portablecanvas/WindowEvents.h"

namespace pc {

// widgets draw with the font their panel paints them with, so they do not own
// rendering resources or look up a theme
class Widget {
	Rect rect_;

public:
	virtual ~Widget() = default;

	Rect rect() const { return rect_; }
	virtual void setRect(Rect rect) { rect_ = rect; }

	bool contains(Point point) const { return rect_.contains(point); }

	virtual void paint(Canvas & canvas, Font const & font) const = 0;

	virtual bool focusable() const { return false; }
	virtual bool needsTimer() const { return false; }

	virtual void setFocused(bool focused) {}
	virtual void onTimer() {}
	virtual void onMouseDown(MouseHits mouse, Point point) {}
	virtual void onMouseMove(MouseHits mouse, Point point) {}
	virtual void onMouseUp(MouseHits mouse, Point point) {}
	virtual void onKeyDown(Key key, KeyModifiers modifiers) {}
	virtual void onChar(std::uint32_t codePoint) {}
};

}

#endif
