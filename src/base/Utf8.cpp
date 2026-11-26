#include "Utf8.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace pc {

namespace {

std::size_t lengthAt(std::string_view text, std::size_t index) {
	if( index >= text.size() ) return 0;
	unsigned char lead = static_cast<unsigned char>(text[index]);
	if( (lead & 0xE0) == 0xC0 ) return 2;
	if( (lead & 0xF0) == 0xE0 ) return 3;
	if( (lead & 0xF8) == 0xF0 ) return 4;
	return 1;
}

}

std::string Utf8::encode(std::uint32_t codePoint) {
	if( codePoint > 0x10FFFF || (codePoint >= 0xD800 && codePoint <= 0xDFFF) ) return {};
	std::string result;
	if( codePoint <= 0x7F ) {
		result.push_back(static_cast<char>(codePoint));
	} else if( codePoint <= 0x7FF ) {
		result.push_back(static_cast<char>(0xC0 | (codePoint >> 6)));
		result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
	} else if( codePoint <= 0xFFFF ) {
		result.push_back(static_cast<char>(0xE0 | (codePoint >> 12)));
		result.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
		result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
	} else {
		result.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
		result.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
		result.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
		result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
	}
	return result;
}

std::vector<std::uint32_t> Utf8::decode(std::string_view text) {
	std::vector<std::uint32_t> codePoints;
	for( std::size_t i = 0; i < text.size(); ) {
		unsigned char lead = static_cast<unsigned char>(text[i]);
		std::uint32_t codePoint = lead;
		std::size_t length = 1;
		if( (lead & 0xE0) == 0xC0 ) {
			codePoint = lead & 0x1F;
			length = 2;
		} else if( (lead & 0xF0) == 0xE0 ) {
			codePoint = lead & 0x0F;
			length = 3;
		} else if( (lead & 0xF8) == 0xF0 ) {
			codePoint = lead & 0x07;
			length = 4;
		}

		if( i + length > text.size() ) break;
		for( std::size_t part = 1; part < length; ++part ) {
			unsigned char continuation = static_cast<unsigned char>(text[i + part]);
			if( (continuation & 0xC0) != 0x80 ) {
				codePoint = 0xFFFD;
				length = part;
				break;
			}
			codePoint = (codePoint << 6) | (continuation & 0x3F);
		}
		codePoints.push_back(codePoint);
		i += length;
	}
	return codePoints;
}

std::size_t Utf8::previousCodePoint(std::string_view text, std::size_t index) {
	if( index == 0 || index > text.size() ) return 0;
	std::size_t result = index - 1;
	while( result > 0 && (static_cast<unsigned char>(text[result]) & 0xC0) == 0x80 ) {
		--result;
	}
	return result;
}

std::size_t Utf8::nextCodePoint(std::string_view text, std::size_t index) {
	if( index >= text.size() ) return text.size();
	std::size_t length = lengthAt(text,index);
	return std::min(index + length,text.size());
}

}
