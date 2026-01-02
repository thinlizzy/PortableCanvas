#include <format>
#include <iostream>
#include <string>
#include "portablecanvas/Application.h"
#include "portablecanvas/Image.h"
#include "portablecanvas/WindowParameters.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace pc;
using namespace pc::literals;

// This function is needed because Bazel puts data files in weird directories when doing a bazel run
std::string getFilePath(std::string const & filename) {
#ifdef BAZEL_DATA_PATH
#define xstr(a) str(a)
#define str(a) #a
	return xstr(BAZEL_DATA_PATH) + filename;
#else
	return filename;
#endif
}

int main() {
	Application application;

	auto & window = application.createWindow(WindowParameters{.dimensions = 300_w + 300_h, .name = "IMAGE"});

	auto filename = getFilePath("hearts.png");
	int x,y,bpp;
	unsigned char * data = stbi_load(filename.c_str(), &x, &y, &bpp, 0);
	using std::cout;
	using std::endl;
	cout << std::format("{} dims = {},{} bpp = {}", filename, x, y, bpp) << endl;
	// for(int p = 0; p < x*y*bpp; ++p) cout << int(data[p]) << ","; cout << endl;
	auto image = createImage(Dimensions{.width=x,.height=y},bpp,data);
	stbi_image_free(data);

	window.onPaint([&](auto & canvas, auto updateRect) {
		canvas.textOut(0_x+10_y,"IMAGE",255_g);
		canvas.drawImage(10_x+10_y,*image);
	});
	window.onKeyDown([&](auto key) {
		application.stop();
	});

	application.run();
}
