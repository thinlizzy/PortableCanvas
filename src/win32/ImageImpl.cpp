#include "ImageImpl.h"
#include <windows.h>
#include <gl/GL.h>

namespace pc {

auto genTexture() {
	unsigned int texture;
	glGenTextures(1,&texture);
	return texture;
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

ImageImpl::ImageImpl(Dimensions dimensions, BytesPerPixel bpp, ImageBuffer bytes):
	texture(genTexture()),
	dimensions_(dimensions),
	bpp_(bpp)
{
	auto sourceFormat = bppToOpenGlFormat(bpp);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,dimensions.width,dimensions.height,0,sourceFormat,GL_UNSIGNED_BYTE,bytes);
	auto error = glGetError();
	if( error != GL_NO_ERROR ) {
		throw "error loading texture";
	}
}

ImageImpl::~ImageImpl() {
	glDeleteTextures(1, &texture);
}

Dimensions ImageImpl::dimensions() const {
    return dimensions_;
}

BytesPerPixel ImageImpl::bpp() const {
    return bpp_;
}

}
