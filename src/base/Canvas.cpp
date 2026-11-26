#include "Canvas.h"

namespace pc {

Canvas::~Canvas() = default;

Canvas::Clip::Clip(Canvas::Clip && other) noexcept : canvas(other.canvas) {
	other.canvas = nullptr;
}

Canvas::Clip::~Clip() {
	if( canvas != nullptr ) canvas->popClip();
}

}
