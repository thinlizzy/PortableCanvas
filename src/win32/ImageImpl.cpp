#include "ImageImpl.h"
#include <gl/GL.h>
#include <stdexcept>

namespace pc {

namespace {

auto genTexture() {
	unsigned int texture;
	glGenTextures(1,&texture);
	return texture;
}

auto bppToOpenGlFormat(BytesPerPixel bpp) {
	switch(bpp) {
		case 1: return GL_RED;
		case 3: return GL_RGB;
		case 4: return GL_RGBA;
		default: throw std::runtime_error("invalid bpp format");
	}
}

}

ImageImpl::ImageImpl(std::shared_ptr<GlContext> context, Dimensions dimensions, BytesPerPixel bpp, ImageBuffer bytes):
	context_(std::move(context)),
	dimensions_(dimensions),
	bpp_(bpp),
	texture(0)
{
	context_->withCurrent([&] {
		texture = genTexture();
		try {
			auto sourceFormat = bppToOpenGlFormat(bpp);
			glBindTexture(GL_TEXTURE_2D,texture);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,dimensions.width,dimensions.height,0,sourceFormat,GL_UNSIGNED_BYTE,bytes);
			auto error = glGetError();
			if( error != GL_NO_ERROR ) throw std::runtime_error("error loading texture");
			glBindTexture(GL_TEXTURE_2D,0);
		} catch(...) {
			glDeleteTextures(1,&texture);
			texture = 0;
			throw;
		}
	});
}

ImageImpl::~ImageImpl() noexcept {
	if( texture == 0 ) return;
	if( !context_ || !context_->valid() ) return;   // the window is gone and its GL context with it
	try {
		context_->withCurrent([&] { glDeleteTextures(1,&texture); });
	} catch(...) {
	}
	texture = 0;
}

Dimensions ImageImpl::dimensions() const {
	return dimensions_;
}

BytesPerPixel ImageImpl::bpp() const {
	return bpp_;
}

}
