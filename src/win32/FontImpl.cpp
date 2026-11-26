#include "FontImpl.h"

#include <gl/GL.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include "windows_error.h"
#include "src/base/Utf8.h"

namespace pc {

namespace {

constexpr int kGlyphCount = 256;

std::wstring toWide(std::string_view utf8) {
	if( utf8.empty() ) return {};
	int count = MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,utf8.data(),int(utf8.size()),nullptr,0);
	if( count <= 0 ) throw std::runtime_error(lastWindowsError());
	std::wstring result(std::size_t(count),L'\0');
	int written = MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,utf8.data(),int(utf8.size()),result.data(),count);
	if( written <= 0 ) throw std::runtime_error(lastWindowsError());
	return result;
}

std::wstring toUtf16(std::uint32_t codePoint) {
	std::wstring result;
	if( codePoint <= 0xFFFF ) {
		result.push_back(static_cast<wchar_t>(codePoint));
		return result;
	}
	if( codePoint <= 0x10FFFF ) {
		codePoint -= 0x10000;
		result.push_back(static_cast<wchar_t>(0xD800 + (codePoint >> 10)));
		result.push_back(static_cast<wchar_t>(0xDC00 + (codePoint & 0x3FF)));
	}
	return result;
}

}

FontImpl::FontImpl(std::shared_ptr<GlContext> context, FontParameters const & parameters):
	context_(std::move(context))
{
	try {
		memoryDc_ = CreateCompatibleDC(nullptr);
		if( memoryDc_ == nullptr ) throw std::runtime_error(lastWindowsError());

		std::wstring family = toWide(parameters.family);
		font_ = CreateFontW(-parameters.height,0,0,0,parameters.weight,
			parameters.italic ? TRUE : FALSE,FALSE,FALSE,DEFAULT_CHARSET,
			OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_DONTCARE,
			family.empty() ? nullptr : family.c_str());
		if( font_ == nullptr ) throw std::runtime_error(lastWindowsError());

		oldFont_ = SelectObject(memoryDc_,font_);
		if( oldFont_ == nullptr || oldFont_ == HGDI_ERROR ) {
			throw std::runtime_error(lastWindowsError());
		}

		TEXTMETRICW textMetrics;
		if( GetTextMetricsW(memoryDc_,&textMetrics) == 0 ) {
			throw std::runtime_error(lastWindowsError());
		}
		metrics_ = FontMetrics{
			.ascent = int(textMetrics.tmAscent),
			.descent = int(textMetrics.tmDescent),
			.lineHeight = int(textMetrics.tmHeight)
		};

		context_->withCurrent([this] {
			listBase_ = glGenLists(kGlyphCount);
			if( listBase_ == 0 ) throw std::runtime_error("could not allocate font display lists");

			auto dc = wglGetCurrentDC();
			auto previousFont = SelectObject(dc,font_);
			bool created = wglUseFontBitmapsA(dc,0,kGlyphCount,listBase_) != 0;
			SelectObject(dc,previousFont);
			if( ! created ) throw std::runtime_error(lastWindowsError());
		});
	} catch(...) {
		release();
		throw;
	}
}

FontImpl::~FontImpl() {
	release();
}

void FontImpl::release() noexcept {
	if( listBase_ != 0 && context_ != nullptr && context_->valid() ) {
		try {
			context_->withCurrent([this] { glDeleteLists(listBase_,kGlyphCount); });
		} catch(...) {
		}
		listBase_ = 0;
	}
	if( memoryDc_ != nullptr ) {
		if( oldFont_ != nullptr && oldFont_ != HGDI_ERROR ) SelectObject(memoryDc_,oldFont_);
		DeleteDC(memoryDc_);
		memoryDc_ = nullptr;
	}
	if( font_ != nullptr ) {
		DeleteObject(font_);
		font_ = nullptr;
	}
}

int FontImpl::measure(std::string_view utf8) const {
	std::wstring text = toWide(utf8);
	if( text.empty() ) return 0;
	SIZE size;
	if( GetTextExtentPoint32W(memoryDc_,text.data(),int(text.size()),&size) == 0 ) {
		throw std::runtime_error(lastWindowsError());
	}
	return size.cx;
}

int FontImpl::advance(std::uint32_t codePoint) const {
	std::wstring text = toUtf16(codePoint);
	if( text.empty() ) return 0;
	SIZE size;
	if( GetTextExtentPoint32W(memoryDc_,text.data(),int(text.size()),&size) == 0 ) {
		throw std::runtime_error(lastWindowsError());
	}
	return size.cx;
}

void FontImpl::textOut(Point point, std::string_view utf8, Color color) const {
	if( utf8.empty() ) return;
	std::vector<unsigned char> glyphs;
	glyphs.reserve(utf8.size());
	for( auto codePoint : Utf8::decode(utf8) ) {
		glyphs.push_back(codePoint < std::uint32_t(kGlyphCount) ? static_cast<unsigned char>(codePoint) : '?');
	}

	glPushAttrib(GL_LIST_BIT);
	glColor3ub(color.r,color.g,color.b);
	glRasterPos2i(point.x,point.y);
	glListBase(listBase_);
	glCallLists(static_cast<GLsizei>(glyphs.size()),GL_UNSIGNED_BYTE,glyphs.data());
	glPopAttrib();
}

}
