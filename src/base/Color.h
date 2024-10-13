#ifndef PORTABLE_CANVAS_COLOR_2024_10_05_H
#define PORTABLE_CANVAS_COLOR_2024_10_05_H

#include <cstdint>

struct Color {
    std::uint8_t r,g,b;
};

// literals

constexpr Color operator""_r(unsigned long long v) {
	return Color{ .r = std::uint8_t(v) };
}

constexpr Color operator""_g(unsigned long long v) {
	return Color{ .g = std::uint8_t(v) };
}

constexpr Color operator""_b(unsigned long long v) {
	return Color{ .b = std::uint8_t(v) };
}

constexpr Color operator+(Color const & c1, Color const & c2) {
	return Color{ .r = std::uint8_t(c1.r + c2.r), .g = std::uint8_t(c1.g + c2.g), .b = std::uint8_t(c1.b + c2.b) };
}


#endif
