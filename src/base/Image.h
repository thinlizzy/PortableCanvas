#ifndef PORTABLE_CANVAS_IMAGE_2026_01_01_H
#define PORTABLE_CANVAS_IMAGE_2026_01_01_H

#include <memory>
#include "Dimensions.h"
#include "ImageTypes.h"

namespace pc {

class ImageImpl;

class Image {
public:
	std::unique_ptr<ImageImpl> impl;

	Image(std::unique_ptr<ImageImpl> impl);
	~Image();
	Dimensions dimensions() const;
	BytesPerPixel bpp() const;
};

}

#endif
