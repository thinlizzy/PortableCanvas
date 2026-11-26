#ifndef PORTABLE_CANVAS_BUTTON_2026_11_24_H
#define PORTABLE_CANVAS_BUTTON_2026_11_24_H

#include <functional>
#include <string>
#include <utility>
#include "portablecanvas/Canvas.h"
#include "portablecanvas/Color.h"
#include "portablecanvas/Font.h"
#include "portablecanvas/Rect.h"
#include "portablecanvas/WindowEvents.h"
#include "Widget.h"

namespace pc {

struct ButtonParameters {
	Rect rect;
	std::string label;
	Color textColor;
	Color borderColor;
	Color fillColor;
	Color focusedBorderColor;
	bool enabled = true;
};

class Button: public Widget {
	ButtonParameters parameters_;
	std::function<void()> onClick_;
	bool pressed_ = false;
	bool focused_ = false;

public:
	explicit Button(ButtonParameters parameters);

	Button & setOnClick(std::function<void()> onClick) {
		onClick_ = std::move(onClick);
		return *this;
	}

	void setLabel(std::string label) { parameters_.label = std::move(label); }
	void setEnabled(bool enabled) { parameters_.enabled = enabled; }
	void setRect(Rect rect) override;

	void paint(Canvas & canvas, Font const & font) const override;
	bool focusable() const override { return true; }
	void setFocused(bool focused) override;
	void onMouseDown(MouseHits mouse, Point point) override;
	void onMouseMove(MouseHits mouse, Point point) override;
	void onMouseUp(MouseHits mouse, Point point) override;
	void onKeyDown(Key key, KeyModifiers modifiers) override;
};

}

#endif
