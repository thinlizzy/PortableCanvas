#ifndef PORTABLE_CANVAS_TEXT_FIELD_2026_11_24_H
#define PORTABLE_CANVAS_TEXT_FIELD_2026_11_24_H

#include <cstddef>
#include <cstdint>
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

struct TextFieldParameters {
	Rect rect;
	std::string text;
	std::string hint;
	Color textColor;
	Color hintColor;
	Color borderColor;
	Color focusedBorderColor;
	Color fillColor;
	Color caretColor;
};

// a single-line edit box; the caret blinks while the panel's timer runs
class TextField: public Widget {
	TextFieldParameters parameters_;
	std::function<void()> onEnter_;
	std::size_t cursor_ = 0;
	bool focused_ = false;
	bool caretVisible_ = false;

public:
	explicit TextField(TextFieldParameters parameters);

	void setOnEnter(std::function<void()> callback) { onEnter_ = std::move(callback); }
	std::string const & text() const { return parameters_.text; }
	void setText(std::string text);
	void setRect(Rect rect) override;

	bool focusable() const override { return true; }
	bool needsTimer() const override { return true; }
	void setFocused(bool focused) override;
	void onTimer() override;

	void paint(Canvas & canvas, Font const & font) const override;
	void onMouseDown(MouseHits mouse, Point point) override;
	void onKeyDown(Key key, KeyModifiers modifiers) override;
	void onChar(std::uint32_t codePoint) override;
};

}

#endif
