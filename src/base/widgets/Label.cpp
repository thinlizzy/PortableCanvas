#include "Label.h"

#include <string>
#include <utility>
#include "portablecanvas/Canvas.h"
#include "portablecanvas/Font.h"
#include "portablecanvas/Point.h"
#include "portablecanvas/Rect.h"
#include "portablecanvas/TextLayout.h"

namespace pc {

Label::Label(LabelParameters parameters):
	parameters_(std::move(parameters))
{
	Widget::setRect(parameters_.rect);
}

void Label::setRect(Rect rect) {
	parameters_.rect = rect;
	Widget::setRect(rect);
}

void Label::paint(Canvas & canvas, Font const & font) const {
	auto rect = this->rect();
	auto lines = wrapText(font,parameters_.text,rect.rightBottom.x - rect.leftTop.x);
	auto metrics = font.metrics();
	auto clip = canvas.clip(rect);
	Point point{.x = rect.leftTop.x,.y = rect.leftTop.y + metrics.ascent};
	for( auto && line : lines ) {
		canvas.textOut(point,line,font,parameters_.color);
		point.y += metrics.lineHeight;
	}
}

}
