#ifndef PORTABLE_CANVAS_WINDOW_2024_01_29_H
#define PORTABLE_CANVAS_WINDOW_2024_01_29_H

#include <memory>
#include "Font.h"
#include "FontParameters.h"
#include "Image.h"
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
	void onChar(CharEvent event);
	void onMouseDown(MouseDownEvent event);
	void onMouseMove(MouseMoveEvent event);
	void onMouseUp(MouseUpEvent event);
	void onClose(CloseEvent event);
	void onTimer(TimerEvent event);

	void repaint();
	bool visible() const;
	void close();

	// the timer wakes the window repeatedly; stop it when no widget needs it
	void startTimer(int milliseconds);
	void stopTimer();

	Image createImage(Dimensions dimensions, BytesPerPixel bpp, ImageBuffer bytes);
	Font createFont(FontParameters const & parameters);
};

}

#endif
