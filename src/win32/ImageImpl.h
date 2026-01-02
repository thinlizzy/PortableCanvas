#ifndef PORTABLE_CANVAS_IMAGE_IMPL_2026_01_01_H
#define PORTABLE_CANVAS_IMAGE_IMPL_2026_01_01_H

#include "src/base/Image.h"

namespace pc {

class ImageImpl: public Image {
public:
	unsigned int texture;
private:
	Dimensions dimensions_;
	BytesPerPixel bpp_;
public:
	ImageImpl(unsigned int texture, Dimensions dimensions, BytesPerPixel bpp);
	~ImageImpl() override;
	Dimensions dimensions() const override;
	BytesPerPixel bpp() const override;
};

}

#endif
