#ifndef PORTABLE_CANVAS_WINDOW_EVENTS_2024_03_06_H
#define PORTABLE_CANVAS_WINDOW_EVENTS_2024_03_06_H

#include <functional>
#include "Canvas.h"
#include "Keys.h"
#include "Rect.h"

namespace pc {

using PaintEvent = std::function<void(Canvas &,Rect)>;
using KeyDownEvent = std::function<void(Key)>;

}

#endif
