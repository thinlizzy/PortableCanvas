#ifndef PORTABLE_CANVAS_WINDOW_IMPL_2024_01_31_H
#define PORTABLE_CANVAS_WINDOW_IMPL_2024_01_31_H

#include <windows.h>
#include "src/base/Image.h"
#include "src/base/WindowEvents.h"
#include "src/base/WindowParameters.h"

namespace pc {

class WindowImpl {
	PaintEvent paintEvent;
	KeyDownEvent keyDownEvent;
	MouseDownEvent mouseDownEvent;
public:
	HWND hwnd;

	WindowImpl(WindowParameters parameters, WNDCLASSW const & windowClass);
	LRESULT processMessage(UINT message, WPARAM wParam, LPARAM lParam);

	void onPaint(PaintEvent event) { paintEvent = event; }
	void onKeyDown(KeyDownEvent event) { keyDownEvent = event; }
	void onMouseDown(MouseDownEvent event) { mouseDownEvent = event; }

	Image createImage(Dimensions dimensions, BytesPerPixel bpp, ImageBuffer bytes);
};

}

#endif