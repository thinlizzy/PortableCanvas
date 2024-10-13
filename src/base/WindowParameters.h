#ifndef PORTABLE_CANVAS_WINDOW_PARAMETERS_2024_03_01_H
#define PORTABLE_CANVAS_WINDOW_PARAMETERS_2024_03_01_H

#include <string>
#include <variant>
#include "Dimensions.h"

namespace pc {

struct WindowParameters {
	Dimensions dimensions = {};
	std::variant<std::monostate, std::string, std::wstring> name;

	bool isDefaultDimensions() {
		return dimensions.width == 0 && dimensions.height == 0;
	}
};

}

#endif
