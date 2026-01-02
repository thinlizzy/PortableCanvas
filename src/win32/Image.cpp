#include "src/base/Image.h"
#include "ImageImpl.h"
#include <memory>

namespace pc {

Image::Image(std::unique_ptr<ImageImpl> impl):
	impl(std::move(impl))
{}

Image::~Image() = default;

Dimensions Image::dimensions() const { return impl->dimensions(); }
BytesPerPixel Image::bpp() const { return impl->bpp(); }

}
