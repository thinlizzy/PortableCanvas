#ifndef PORTABLE_CANVAS_IMAGE_IMPL_2026_01_01_H
#define PORTABLE_CANVAS_IMAGE_IMPL_2026_01_01_H

#include "src/base/Dimensions.h"
#include "src/base/ImageTypes.h"

namespace pc {

class ImageImpl {
public:
	unsigned int texture;
private:
	Dimensions dimensions_;
	BytesPerPixel bpp_;
public:
	ImageImpl(Dimensions dimensions, BytesPerPixel bpp, ImageBuffer bytes);
	~ImageImpl();
	Dimensions dimensions() const;
	BytesPerPixel bpp() const;
};

}

#endif
