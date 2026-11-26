#ifndef PORTABLE_CANVAS_FONT_IMPL_2026_11_24_H
#define PORTABLE_CANVAS_FONT_IMPL_2026_11_24_H

#include <cstdint>
#include <memory>
#include <string_view>
#include <windows.h>
#include "GlContext.h"
#include "src/base/Color.h"
#include "src/base/FontMetrics.h"
#include "src/base/FontParameters.h"
#include "src/base/Point.h"

namespace pc {

class FontImpl {
	std::shared_ptr<GlContext> context_;
	HFONT font_ = nullptr;
	HDC memoryDc_ = nullptr;
	HGDIOBJ oldFont_ = nullptr;
	unsigned int listBase_ = 0;
	FontMetrics metrics_ = {};

	void release() noexcept;

public:
	FontImpl(std::shared_ptr<GlContext> context, FontParameters const & parameters);
	~FontImpl();

	FontMetrics metrics() const { return metrics_; }
	int measure(std::string_view utf8) const;
	int advance(std::uint32_t codePoint) const;
	void textOut(Point point, std::string_view utf8, Color color) const;
};

}

#endif
