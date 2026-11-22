#ifndef PORTABLE_CANVAS_WINDOW_IMPL_2024_01_31_H
#define PORTABLE_CANVAS_WINDOW_IMPL_2024_01_31_H

#include <memory>
#include <windows.h>
#include "src/base/Image.h"
#include "src/base/WindowEvents.h"
#include "src/base/WindowParameters.h"

namespace pc {

class GlContext;

class WindowImpl {
	PaintEvent paintEvent;
	KeyDownEvent keyDownEvent;
	MouseDownEvent mouseDownEvent;

	void release();

public:
	HWND hwnd;
	std::shared_ptr<GlContext> gl;
	bool visible;

	WindowImpl(WindowParameters parameters, WNDCLASSW const & windowClass);
	~WindowImpl();
	LRESULT processMessage(UINT message, WPARAM wParam, LPARAM lParam);

	void onPaint(PaintEvent event) { paintEvent = event; }
	void onKeyDown(KeyDownEvent event) { keyDownEvent = event; }
	void onMouseDown(MouseDownEvent event) { mouseDownEvent = event; }

	void repaint();

	Image createImage(Dimensions dimensions, BytesPerPixel bpp, ImageBuffer bytes);
};

}

#endif