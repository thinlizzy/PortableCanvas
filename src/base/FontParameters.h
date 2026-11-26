#ifndef PORTABLE_CANVAS_FONT_PARAMETERS_2026_11_24_H
#define PORTABLE_CANVAS_FONT_PARAMETERS_2026_11_24_H

#include <string>

namespace pc {

struct FontParameters {
	std::string family;  // empty uses the platform's default UI font
	int height = 16;
	int weight = 400;
	bool italic = false;
};

}

#endif
