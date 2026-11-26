#include "CanvasImpl.h"
#include "FontImpl.h"
#include "ImageImpl.h"
#include "src/base/Rect.h"
#include <algorithm>
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

CanvasImpl & CanvasImpl::textOut(Point p, std::string_view text, Font const & font, Color color) {
	font.impl->textOut(p,text,color);
	return *this;
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

CanvasImpl & CanvasImpl::fillRect(Rect rect, Color color) {
	glBegin(GL_QUADS);
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

void CanvasImpl::pushClip(Rect rect) {
	if( ! clips_.empty() ) rect = clips_.back().intersect(rect);
	clips_.push_back(rect);
	applyClip();
}

void CanvasImpl::popClip() {
	if( clips_.empty() ) return;
	clips_.pop_back();
	applyClip();
}

void CanvasImpl::applyClip() {
	if( clips_.empty() ) {
		glDisable(GL_SCISSOR_TEST);
		return;
	}
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	auto const & rect = clips_.back();
	int width = rect.rightBottom.x - rect.leftTop.x;
	int height = rect.rightBottom.y - rect.leftTop.y;
	glEnable(GL_SCISSOR_TEST);
	glScissor(rect.leftTop.x,viewport[3] - rect.rightBottom.y,std::max(width,0),std::max(height,0));
}

}
