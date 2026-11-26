#include "TextField.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include "portablecanvas/Canvas.h"
#include "portablecanvas/Font.h"
#include "portablecanvas/Point.h"
#include "portablecanvas/Rect.h"
#include "portablecanvas/Utf8.h"
#include "portablecanvas/WindowEvents.h"

namespace pc {

namespace {

constexpr int kPadding = 6;

}

TextField::TextField(TextFieldParameters parameters):
	parameters_(std::move(parameters))
{
	Widget::setRect(parameters_.rect);
	cursor_ = parameters_.text.size();
}

void TextField::setText(std::string text) {
	parameters_.text = std::move(text);
	cursor_ = parameters_.text.size();
}

void TextField::setRect(Rect rect) {
	parameters_.rect = rect;
	Widget::setRect(rect);
}

void TextField::setFocused(bool focused) {
	focused_ = focused;
	if( focused_ ) {
		caretVisible_ = true;
	} else {
		caretVisible_ = false;
	}
}

void TextField::onTimer() {
	if( focused_ ) caretVisible_ = ! caretVisible_;
}

void TextField::paint(Canvas & canvas, Font const & font) const {
	auto rect = this->rect();
	canvas.fillRect(rect,parameters_.fillColor);
	canvas.rectangle(rect,focused_ ? parameters_.focusedBorderColor : parameters_.borderColor);

	auto metrics = font.metrics();
	int baseline = (rect.leftTop.y + rect.rightBottom.y) / 2 + metrics.ascent - metrics.lineHeight / 2;
	int left = rect.leftTop.x + kPadding;

	Rect inner{
		.leftTop = Point{.x = left,.y = rect.leftTop.y + 2},
		.rightBottom = Point{.x = rect.rightBottom.x - kPadding,.y = rect.rightBottom.y - 2}
	};
	auto clip = canvas.clip(inner);

	if( parameters_.text.empty() ) {
		canvas.textOut(Point{.x = left,.y = baseline},parameters_.hint,font,parameters_.hintColor);
	} else {
		canvas.textOut(Point{.x = left,.y = baseline},parameters_.text,font,parameters_.textColor);
	}

	if( focused_ && caretVisible_ ) {
		int caretX = left + font.measure(std::string_view(parameters_.text).substr(0,cursor_));
		canvas.fillRect(
			Rect{
				.leftTop = Point{.x = caretX,.y = baseline - metrics.ascent},
				.rightBottom = Point{.x = caretX + 1,.y = baseline + metrics.descent}
			},
			parameters_.caretColor);
	}
}

void TextField::onMouseDown(MouseHits mouse, Point point) {
	if( mouse.leftClick() && contains(point) ) cursor_ = parameters_.text.size();
}

void TextField::onKeyDown(Key key, KeyModifiers modifiers) {
	switch(key) {
		case Key::backspace:
			if( cursor_ > 0 ) {
				std::size_t start = Utf8::previousCodePoint(parameters_.text,cursor_);
				parameters_.text.erase(start,cursor_ - start);
				cursor_ = start;
			}
			break;
		case Key::deleteKey:
			if( cursor_ < parameters_.text.size() ) {
				std::size_t end = Utf8::nextCodePoint(parameters_.text,cursor_);
				parameters_.text.erase(cursor_,end - cursor_);
			}
			break;
		case Key::left:
			cursor_ = Utf8::previousCodePoint(parameters_.text,cursor_);
			break;
		case Key::right:
			cursor_ = Utf8::nextCodePoint(parameters_.text,cursor_);
			break;
		case Key::home:
			cursor_ = 0;
			break;
		case Key::end:
			cursor_ = parameters_.text.size();
			break;
		case Key::enter:
		case Key::numpadEnter:
			if( onEnter_ ) onEnter_();
			break;
		default:
			break;
	}
}

void TextField::onChar(std::uint32_t codePoint) {
	if( codePoint < 32 || codePoint == 127 ) return;
	std::string encoded = Utf8::encode(codePoint);
	if( encoded.empty() ) return;
	parameters_.text.insert(cursor_,encoded);
	cursor_ += encoded.size();
}

}
