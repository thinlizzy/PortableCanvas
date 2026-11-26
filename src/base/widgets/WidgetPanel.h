#ifndef PORTABLE_CANVAS_WIDGET_PANEL_2026_11_24_H
#define PORTABLE_CANVAS_WIDGET_PANEL_2026_11_24_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "portablecanvas/Canvas.h"
#include "portablecanvas/Font.h"
#include "portablecanvas/Point.h"
#include "portablecanvas/Rect.h"
#include "portablecanvas/Window.h"
#include "portablecanvas/WindowEvents.h"
#include "Widget.h"

namespace pc {

// owns the widgets of one window, routes events to them, and moves focus with
// Tab. A focused widget that needs a timer keeps the window timer running.
// The panel must be destroyed before its window.
class WidgetPanel {
	static constexpr std::size_t kNoFocus = std::size_t(-1);

	Window & window_;
	std::vector<std::unique_ptr<Widget>> widgets_;
	std::size_t focused_ = kNoFocus;

	void updateTimer();
	void focusNext(bool reverse);

public:
	explicit WidgetPanel(Window & window);
	~WidgetPanel();

	Widget & addWidget(std::unique_ptr<Widget> widget);

	void paint(Canvas & canvas, Font const & font) const;
	void onMouseDown(MouseHits mouse, Point point);
	void onMouseMove(MouseHits mouse, Point point);
	void onMouseUp(MouseHits mouse, Point point);
	void onKeyDown(Key key, KeyModifiers modifiers);
	void onChar(std::uint32_t codePoint);
	void onTimer();

	void focus(std::size_t index);
	void clearFocus();
	std::size_t focusedIndex() const { return focused_; }
};

}

#endif
