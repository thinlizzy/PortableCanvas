#ifndef PORTABLE_CANVAS_UTF8_2026_11_25_H
#define PORTABLE_CANVAS_UTF8_2026_11_25_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace pc {

// UTF-8 helpers for code point encoding, decoding and byte-index navigation
class Utf8 {
public:
	static std::string encode(std::uint32_t codePoint);
	static std::vector<std::uint32_t> decode(std::string_view text);
	static std::size_t previousCodePoint(std::string_view text, std::size_t index);
	static std::size_t nextCodePoint(std::string_view text, std::size_t index);
};

}

#endif
