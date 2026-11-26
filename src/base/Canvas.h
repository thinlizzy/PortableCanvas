#ifndef PORTABLE_CANVAS_CANVAS_2024_01_25_H
#define PORTABLE_CANVAS_CANVAS_2024_01_25_H

#include <string_view>
#include "Color.h"
#include "Font.h"
#include "Image.h"
#include "Point.h"
#include "Rect.h"

namespace pc {

class Canvas {
public:
	// scoped clipping; the guard restores the previous clip when destroyed
	class Clip {
	public:
		Clip(Clip && other) noexcept;
		~Clip();
		Clip(Clip const &) = delete;
		Clip & operator=(Clip const &) = delete;
		Clip & operator=(Clip &&) = delete;

	private:
		friend class Canvas;
		explicit Clip(Canvas & canvas): canvas(&canvas) {}
		Canvas * canvas;
	};

	virtual ~Canvas() = 0;

	virtual Canvas & textOut(Point p, std::string_view text, Color color) = 0;

	virtual Canvas & textOut(Point p, std::string_view text, Font const & font, Color color) = 0;

	virtual Canvas & line(Point p1, Point p2, Color color) = 0;

	virtual Canvas & rectangle(Rect rect, Color color) = 0;

	virtual Canvas & fillRect(Rect rect, Color color) = 0;

	virtual Canvas & drawImage(Point p, Image const & image) = 0;

	virtual Canvas & drawImage(Rect rect, Image const & image) = 0;

	virtual Canvas & drawImage(Point p, int scale, Image const & image) = 0;

	virtual Canvas & drawImage(Point p, float scale, Image const & image) = 0;

	Clip clip(Rect rect) {
		pushClip(rect);
		return Clip(*this);
	}

protected:
	virtual void pushClip(Rect rect) = 0;
	virtual void popClip() = 0;
};

}

#endif
