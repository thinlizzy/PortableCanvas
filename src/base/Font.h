#ifndef PORTABLE_CANVAS_FONT_2026_11_24_H
#define PORTABLE_CANVAS_FONT_2026_11_24_H

#include <cstdint>
#include <memory>
#include <string_view>
#include "FontMetrics.h"

namespace pc {

class FontImpl;

// a font belongs to the window's rendering context, so it must be destroyed
// before that window
class Font {
public:
	std::unique_ptr<FontImpl> impl;

	explicit Font(std::unique_ptr<FontImpl> impl);
	Font(Font &&) noexcept;
	~Font();

	Font(Font const &) = delete;
	Font & operator=(Font const &) = delete;

	FontMetrics metrics() const;
	int measure(std::string_view utf8) const;
	int advance(std::uint32_t codePoint) const;
};

}

#endif
