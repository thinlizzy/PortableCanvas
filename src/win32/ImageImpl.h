#ifndef PORTABLE_CANVAS_IMAGE_IMPL_2026_01_01_H
#define PORTABLE_CANVAS_IMAGE_IMPL_2026_01_01_H

#include <memory>
#include "GlContext.h"
#include "src/base/Dimensions.h"
#include "src/base/ImageTypes.h"

namespace pc {

class ImageImpl {
	std::shared_ptr<GlContext> context_;
	Dimensions dimensions_;
	BytesPerPixel bpp_;
public:
	unsigned int texture;

	ImageImpl(std::shared_ptr<GlContext> context, Dimensions dimensions, BytesPerPixel bpp, ImageBuffer bytes);
	~ImageImpl() noexcept;

	Dimensions dimensions() const;
	BytesPerPixel bpp() const;
};

}

#endif
