#include "CanvasImpl.h"
#include "ImageImpl.h"
#include "src/base/Rect.h"
#include <iostream>
#include <windows.h>
#include <gl/GL.h>

namespace pc {

CanvasImpl::CanvasImpl() {
	// glClearColor(0.0F, 0.3F, 0.3F, 1.0F);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

CanvasImpl::~CanvasImpl() {
	SwapBuffers(wglGetCurrentDC());
}

CanvasImpl & CanvasImpl::textOut(Point p, std::string_view text, Color color) {
	glColor3ub(color.r,color.g,color.b);
	glRasterPos2i(p.x,p.y);
	glPushAttrib(GL_CURRENT_BIT);
	glCallLists(text.size(),GL_UNSIGNED_BYTE,text.data());
	glPopAttrib();
	return *this;
}

CanvasImpl & CanvasImpl::line(Point p1, Point p2, Color color) {
	glBegin(GL_LINES);
	glColor3ub(color.r,color.g,color.b);
	glVertex2i(p1.x,p1.y);
	glVertex2i(p2.x,p2.y);
	glEnd();
	return *this;
}

CanvasImpl & CanvasImpl::rectangle(Rect rect, Color color) {
	glBegin(GL_LINE_LOOP);
	glColor3ub(color.r,color.g,color.b);
	glVertex2i(rect.leftTop.x,rect.leftTop.y);
	glVertex2i(rect.rightBottom.x,rect.leftTop.y);
	glVertex2i(rect.rightBottom.x,rect.rightBottom.y);
	glVertex2i(rect.leftTop.x,rect.rightBottom.y);
	glEnd();
	return *this;
}

CanvasImpl & CanvasImpl::drawImage(Point p, Image const & image) {
	auto r = Rect::open(p,image.dimensions());
	return drawImage(r,image);
}

CanvasImpl & CanvasImpl::drawImage(Point p, int scale, Image const & image) {
	auto r = Rect::open(p,image.dimensions() * scale);
	return drawImage(r,image);
}

CanvasImpl & CanvasImpl::drawImage(Point p, float scale, Image const & image) {
	auto r = Rect::open(p,image.dimensions() * scale);
	return drawImage(r,image);
}

CanvasImpl & CanvasImpl::drawImage(Rect r, Image const &image) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,image.impl->texture);
	glColor4f(1.0, 1.0, 1.0, 1.0); // TODO remove glColor4f() when figuring out how to do it
	glBegin(GL_QUADS);
	glTexCoord2f(0.f,0.f),
	glVertex2i(r.leftTop.x,r.leftTop.y);
	glTexCoord2f(1.f,0.f),
	glVertex2i(r.rightBottom.x,r.leftTop.y);
	glTexCoord2f(1.f,1.f),
	glVertex2i(r.rightBottom.x,r.rightBottom.y);
	glTexCoord2f(0.f,1.f),
	glVertex2i(r.leftTop.x,r.rightBottom.y);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	return *this;
}

}
