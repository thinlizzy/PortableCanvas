#include "ImageImpl.h"
#include <windows.h>
#include <gl/GL.h>

namespace pc {

ImageImpl::ImageImpl(unsigned int texture, Dimensions dimensions, BytesPerPixel bpp):
	texture(texture),
	dimensions_(dimensions),
	bpp_(bpp)
{}

ImageImpl::~ImageImpl() {
	glDeleteTextures(1, &texture);
}

Dimensions ImageImpl::dimensions() const {
    return dimensions_;
}

BytesPerPixel ImageImpl::bpp() const {
    return bpp_;
}

auto bppToOpenGlFormat(BytesPerPixel bpp) {
	// TODO test this to make sure this works as expected
	switch(bpp) {
		case 1: return GL_DEPTH_COMPONENT;
		case 3: return GL_RGB;
		case 4: return GL_RGBA;
		default: throw "invalid bpp format";
	}
}

std::unique_ptr<Image> createImage(Dimensions dimensions, BytesPerPixel bpp, unsigned char const * bytes) {
	auto sourceFormat = bppToOpenGlFormat(bpp);
	unsigned int texture;
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,dimensions.width,dimensions.height,0,sourceFormat,GL_UNSIGNED_BYTE,bytes);
	auto error = glGetError();
	if( error != GL_NO_ERROR ) {
		throw "error loading texture";
	}
	return std::make_unique<ImageImpl>(texture,dimensions,bpp);
}

}
