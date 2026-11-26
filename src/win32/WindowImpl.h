#ifndef PORTABLE_CANVAS_WINDOW_IMPL_2024_01_31_H
#define PORTABLE_CANVAS_WINDOW_IMPL_2024_01_31_H

#include <memory>
#include <windows.h>
#include "src/base/Font.h"
#include "src/base/FontParameters.h"
#include "src/base/Image.h"
#include "src/base/WindowEvents.h"
#include "src/base/WindowParameters.h"

namespace pc {

class GlContext;

class WindowImpl {
	PaintEvent paintEvent;
	KeyDownEvent keyDownEvent;
	CharEvent charEvent;
	MouseDownEvent mouseDownEvent;
	MouseMoveEvent mouseMoveEvent;
	MouseUpEvent mouseUpEvent;
	CloseEvent closeEvent;
	TimerEvent timerEvent;
	UINT_PTR timerId = 0;
	wchar_t pendingHighSurrogate = 0;

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
	void onChar(CharEvent event) { charEvent = event; }
	void onMouseDown(MouseDownEvent event) { mouseDownEvent = event; }
	void onMouseMove(MouseMoveEvent event) { mouseMoveEvent = event; }
	void onMouseUp(MouseUpEvent event) { mouseUpEvent = event; }
	void onClose(CloseEvent event) { closeEvent = event; }
	void onTimer(TimerEvent event) { timerEvent = event; }

	void repaint();
	void close();
	void startTimer(int milliseconds);
	void stopTimer();

	Image createImage(Dimensions dimensions, BytesPerPixel bpp, ImageBuffer bytes);
	Font createFont(FontParameters const & parameters);
};

}

#endif