#include "Button.h"

#include <functional>
#include <utility>
#include "portablecanvas/Canvas.h"
#include "portablecanvas/Font.h"
#include "portablecanvas/Point.h"
#include "portablecanvas/Rect.h"
#include "portablecanvas/WindowEvents.h"

namespace pc {

Button::Button(ButtonParameters parameters):
	parameters_(std::move(parameters))
{
	Widget::setRect(parameters_.rect);
}

void Button::setRect(Rect rect) {
	parameters_.rect = rect;
	Widget::setRect(rect);
}

void Button::paint(Canvas & canvas, Font const & font) const {
	auto rect = this->rect();
	canvas.fillRect(rect,parameters_.fillColor);
	canvas.rectangle(rect,focused_ ? parameters_.focusedBorderColor : parameters_.borderColor);

	int textWidth = font.measure(parameters_.label);
	int x = rect.leftTop.x + (rect.rightBottom.x - rect.leftTop.x - textWidth) / 2;
	auto metrics = font.metrics();
	int y = (rect.leftTop.y + rect.rightBottom.y) / 2 + metrics.ascent - metrics.lineHeight / 2;
	canvas.textOut(Point{.x = x,.y = y},parameters_.label,font,parameters_.textColor);
}

void Button::setFocused(bool focused) {
	focused_ = focused;
}

void Button::onMouseDown(MouseHits mouse, Point point) {
	pressed_ = parameters_.enabled && mouse.leftClick() && contains(point);
}

void Button::onMouseMove(MouseHits mouse, Point point) {
}

void Button::onMouseUp(MouseHits mouse, Point point) {
	bool activate = pressed_ && parameters_.enabled && mouse.leftClick() && contains(point);
	pressed_ = false;
	if( activate && onClick_ ) onClick_();
}

void Button::onKeyDown(Key key, KeyModifiers modifiers) {
	if( ! parameters_.enabled ) return;
	if( key == Key::enter || key == Key::numpadEnter || key == Key::space ) {
		if( onClick_ ) onClick_();
	}
}

}
