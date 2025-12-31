#include "src/base/Window.h"
#include "WindowImpl.h"

namespace pc {

Window::Window(WindowImpl * impl): impl(impl) {
}

Window::~Window() {}

void Window::onPaint(PaintEvent event) { impl->onPaint(event); }
void Window::onKeyDown(KeyDownEvent event) { impl->onKeyDown(event); }
void Window::onMouseDown(MouseDownEvent event) { impl->onMouseDown(event); }

}
