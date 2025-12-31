#include "Application.h"
#include "WindowParameters.h"

using namespace pc;

int main() {
	// Application owns everything and runs the event loop
	Application application;

	auto & window = application.createWindow(WindowParameters{.dimensions = 300_w + 300_h, .name = "TEST"});
	window.onPaint([](auto & canvas, auto updateRect) {
		canvas.textOut(10_x+20_y,"AUUUU!!",255_g);
		canvas.line(10_x+30_y,100_x+30_y,255_r);
	});
	window.onKeyDown([&](auto key) {
		application.stop();
	});
	window.onMouseDown([&](auto hits, auto point) {
		application.stop();
	});
	// TODO images

	application.run(); // process UI messages until it's interrupted or all windows are closed
}
