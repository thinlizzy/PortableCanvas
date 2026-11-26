#include "TextLayout.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "Utf8.h"

namespace pc {

namespace {

void splitLongWord(std::vector<std::string> & lines, Font const & font,
	std::string_view word, int maxWidth) {
	std::string current;
	std::size_t index = 0;
	while( index < word.size() ) {
		std::size_t next = Utf8::nextCodePoint(word,index);
		std::string candidate = current + std::string(word.substr(index,next-index));
		if( ! current.empty() && font.measure(candidate) > maxWidth ) {
			lines.push_back(std::move(current));
			current.clear();
			continue;
		}
		current = std::move(candidate);
		index = next;
	}
	if( ! current.empty() ) lines.push_back(std::move(current));
}

void wrapLine(std::vector<std::string> & lines, Font const & font,
	std::string_view line, int maxWidth) {
	std::size_t index = 0;
	while( index < line.size() && line[index] == ' ' ) ++index;
	if( index == line.size() ) {
		lines.emplace_back();
		return;
	}

	std::string current;
	while( index < line.size() ) {
		std::size_t wordEnd = index;
		while( wordEnd < line.size() && line[wordEnd] != ' ' ) ++wordEnd;
		std::string_view word = line.substr(index,wordEnd-index);

		std::string candidate = current.empty() ? std::string(word) : current + " " + std::string(word);
		if( font.measure(candidate) <= maxWidth ) {
			current = std::move(candidate);
		} else if( ! current.empty() ) {
			lines.push_back(std::move(current));
			current.clear();
			if( font.measure(word) > maxWidth ) {
				splitLongWord(lines,font,word,maxWidth);
				current.clear();
			} else {
				current = std::string(word);
			}
		} else {
			splitLongWord(lines,font,word,maxWidth);
		}

		index = wordEnd;
		while( index < line.size() && line[index] == ' ' ) ++index;
	}
	if( ! current.empty() ) lines.push_back(std::move(current));
}

}

std::vector<std::string> wrapText(Font const & font, std::string_view text, int maxWidth) {
	std::vector<std::string> lines;
	if( maxWidth <= 0 ) return lines;

	std::size_t start = 0;
	while( start <= text.size() ) {
		std::size_t end = text.find('\n',start);
		if( end == std::string_view::npos ) end = text.size();
		wrapLine(lines,font,text.substr(start,end-start),maxWidth);
		if( end == text.size() ) break;
		start = end + 1;
	}
	return lines;
}

}
