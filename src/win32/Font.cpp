#include "src/base/Font.h"

#include <utility>
#include "FontImpl.h"

namespace pc {

Font::Font(std::unique_ptr<FontImpl> impl):
	impl(std::move(impl))
{
}

Font::~Font() = default;
Font::Font(Font && other) noexcept = default;

FontMetrics Font::metrics() const {
	return impl->metrics();
}

int Font::measure(std::string_view utf8) const {
	return impl->measure(utf8);
}

int Font::advance(std::uint32_t codePoint) const {
	return impl->advance(codePoint);
}

}
