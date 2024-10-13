#ifndef PORTABLE_CANVAS_WINDOW_IMPL_2024_01_31_H
#define PORTABLE_CANVAS_WINDOW_IMPL_2024_01_31_H

#include <windows.h>
#include "src/base/WindowEvents.h"
#include "src/base/WindowParameters.h"

namespace pc {

class WindowImpl {
public:
	HWND hwnd;
	PaintEvent paintEvent;
	KeyDownEvent keyDownEvent;

	WindowImpl(WindowParameters parameters, WNDCLASSW const & windowClass);
	LRESULT processMessage(UINT message, WPARAM wParam, LPARAM lParam);

	void onPaint(PaintEvent event) { paintEvent = event; }
	void onKeyDown(KeyDownEvent event) { keyDownEvent = event; }
};

}

#endif