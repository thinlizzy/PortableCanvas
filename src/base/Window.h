#ifndef PORTABLE_CANVAS_WINDOW_2024_01_29_H
#define PORTABLE_CANVAS_WINDOW_2024_01_29_H

#include <memory>
#include "WindowEvents.h"

namespace pc {

class WindowImpl;

class Window {
	friend class Application;
	friend class ApplicationImpl;
	std::unique_ptr<WindowImpl> impl;
public:
	explicit Window(WindowImpl * impl);
	~Window();
	void onPaint(PaintEvent event);
	void onKeyDown(KeyDownEvent event);
};

}

#endif
