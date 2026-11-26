#ifndef PORTABLE_CANVAS_TEXT_LAYOUT_2026_11_24_H
#define PORTABLE_CANVAS_TEXT_LAYOUT_2026_11_24_H

#include <string>
#include <string_view>
#include <vector>
#include "Font.h"

namespace pc {

// splits text into lines that fit maxWidth, keeping the original line breaks
std::vector<std::string> wrapText(Font const & font, std::string_view text, int maxWidth);

}

#endif
