#ifndef PORTABLE_CANVAS_IMAGE_2026_01_01_H
#define PORTABLE_CANVAS_IMAGE_2026_01_01_H

#include <memory>
#include <span>
#include "Dimensions.h"

namespace pc {

using BytesPerPixel = unsigned;

class Image {
public:
	virtual ~Image();
	virtual Dimensions dimensions() const = 0;
	virtual BytesPerPixel bpp() const = 0;
};

std::unique_ptr<Image> createImage(Dimensions dimensions, BytesPerPixel bpp, unsigned char const * bytes);

}

#endif
