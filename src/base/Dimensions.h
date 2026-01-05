#ifndef PORTABLE_CANVAS_DIMS_2024_01_25_H
#define PORTABLE_CANVAS_DIMS_2024_01_25_H

#include "concept_arithmetic.h"

namespace pc {

struct Dimensions {
	int width;
	int height;

	Dimensions & operator+=(Dimensions const & d) {
		this->width += d.width;
		this->height += d.height;
		return *this;
	}

	Dimensions & operator-=(Dimensions const & d) {
		this->width -= d.width;
		this->height -= d.height;
		return *this;
	}
};

constexpr Dimensions operator+(Dimensions const & d1, Dimensions const & d2) {
	return Dimensions{ .width = d1.width + d2.width, .height = d1.height + d2.height };
}

constexpr Dimensions operator-(Dimensions const & d1, Dimensions const & d2) {
	return Dimensions{ .width = d1.width - d2.width, .height = d1.height - d2.height };
}

constexpr Dimensions operator*(Dimensions const & d1, arithmetic auto scale) {
	return Dimensions{ .width = int(d1.width * scale), .height = int(d1.height * scale) };
}

namespace literals {

constexpr Dimensions operator""_w(unsigned long long v) {
	return Dimensions{ .width = int(v) };
}

constexpr Dimensions operator""_h(unsigned long long v) {
	return Dimensions{ .height = int(v) };
}

}

}

#endif
