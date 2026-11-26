#include "src/base/Window.h"
#include "WindowImpl.h"

namespace pc {

Window::Window(WindowImpl * impl): impl(impl) {
}

Window::~Window() {}

void Window::onPaint(PaintEvent event) { impl->onPaint(event); }
void Window::onKeyDown(KeyDownEvent event) { impl->onKeyDown(event); }
void Window::onChar(CharEvent event) { impl->onChar(event); }
void Window::onMouseDown(MouseDownEvent event) { impl->onMouseDown(event); }
void Window::onMouseMove(MouseMoveEvent event) { impl->onMouseMove(event); }
void Window::onMouseUp(MouseUpEvent event) { impl->onMouseUp(event); }
void Window::onClose(CloseEvent event) { impl->onClose(event); }
void Window::onTimer(TimerEvent event) { impl->onTimer(event); }

void Window::repaint() { impl->repaint(); }

bool Window::visible() const { return impl->visible; }

void Window::close() { impl->close(); }

void Window::startTimer(int milliseconds) { impl->startTimer(milliseconds); }

void Window::stopTimer() { impl->stopTimer(); }

Image Window::createImage(Dimensions dimensions, BytesPerPixel bpp, ImageBuffer bytes) {
	return impl->createImage(dimensions,bpp,bytes);
}

Font Window::createFont(FontParameters const & parameters) {
	return impl->createFont(parameters);
}

}
