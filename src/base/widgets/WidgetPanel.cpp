#include "WidgetPanel.h"

#include <cstddef>
#include <memory>
#include <utility>
#include "portablecanvas/Window.h"

namespace pc {

namespace {

constexpr int kCaretBlinkMilliseconds = 500;

}

WidgetPanel::WidgetPanel(Window & window):
	window_(window)
{
}

WidgetPanel::~WidgetPanel() {
	window_.stopTimer();
}

Widget & WidgetPanel::addWidget(std::unique_ptr<Widget> widget) {
	widgets_.push_back(std::move(widget));
	return *widgets_.back();
}

void WidgetPanel::paint(Canvas & canvas, Font const & font) const {
	for( auto && widget : widgets_ ) {
		widget->paint(canvas,font);
	}
}

void WidgetPanel::onMouseDown(MouseHits mouse, Point point) {
	if( ! mouse.leftClick() ) return;
	for( std::size_t index = widgets_.size(); index-- > 0; ) {
		auto && widget = widgets_[index];
		if( ! widget->contains(point) ) continue;
		if( widget->focusable() ) focus(index);
		widget->onMouseDown(mouse,point);
		window_.repaint();
		return;
	}
}

void WidgetPanel::onMouseMove(MouseHits mouse, Point point) {
	for( auto && widget : widgets_ ) widget->onMouseMove(mouse,point);
}

void WidgetPanel::onMouseUp(MouseHits mouse, Point point) {
	for( auto && widget : widgets_ ) widget->onMouseUp(mouse,point);
	window_.repaint();
}

void WidgetPanel::onKeyDown(Key key, KeyModifiers modifiers) {
	if( key == Key::tab ) {
		focusNext(modifiers.shiftPressed);
		window_.repaint();
		return;
	}
	if( focused_ != kNoFocus ) {
		widgets_[focused_]->onKeyDown(key,modifiers);
		window_.repaint();
	}
}

void WidgetPanel::onChar(std::uint32_t codePoint) {
	if( focused_ == kNoFocus ) return;
	widgets_[focused_]->onChar(codePoint);
	window_.repaint();
}

void WidgetPanel::onTimer() {
	if( focused_ != kNoFocus && widgets_[focused_]->needsTimer() ) {
		widgets_[focused_]->onTimer();
		window_.repaint();
	}
}

void WidgetPanel::focus(std::size_t index) {
	if( index >= widgets_.size() || index == focused_ ) return;
	if( focused_ != kNoFocus ) widgets_[focused_]->setFocused(false);
	focused_ = index;
	widgets_[focused_]->setFocused(true);
	updateTimer();
}

void WidgetPanel::clearFocus() {
	if( focused_ == kNoFocus ) return;
	widgets_[focused_]->setFocused(false);
	focused_ = kNoFocus;
	updateTimer();
}

void WidgetPanel::updateTimer() {
	if( focused_ != kNoFocus && widgets_[focused_]->needsTimer() ) {
		window_.startTimer(kCaretBlinkMilliseconds);
	} else {
		window_.stopTimer();
	}
}

void WidgetPanel::focusNext(bool reverse) {
	std::size_t count = widgets_.size();
	if( count == 0 ) return;

	std::size_t index;
	if( focused_ == kNoFocus ) {
		index = reverse ? count - 1 : 0;
	} else if( reverse ) {
		index = focused_ == 0 ? count - 1 : focused_ - 1;
	} else {
		index = (focused_ + 1) % count;
	}

	for( std::size_t visited = 0; visited < count; ++visited ) {
		if( widgets_[index]->focusable() ) {
			focus(index);
			return;
		}
		if( reverse ) {
			index = index == 0 ? count - 1 : index - 1;
		} else {
			index = (index + 1) % count;
		}
	}
}

}
