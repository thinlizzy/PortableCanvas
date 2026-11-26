#ifndef PORTABLE_CANVAS_LABEL_2026_11_24_H
#define PORTABLE_CANVAS_LABEL_2026_11_24_H

#include <string>
#include <utility>
#include "portablecanvas/Canvas.h"
#include "portablecanvas/Color.h"
#include "portablecanvas/Font.h"
#include "portablecanvas/Rect.h"
#include "Widget.h"

namespace pc {

struct LabelParameters {
	Rect rect;
	std::string text;
	Color color;
};

class Label: public Widget {
	LabelParameters parameters_;

public:
	explicit Label(LabelParameters parameters);

	void setText(std::string text) { parameters_.text = std::move(text); }
	void setRect(Rect rect) override;
	void paint(Canvas & canvas, Font const & font) const override;
};

}

#endif
