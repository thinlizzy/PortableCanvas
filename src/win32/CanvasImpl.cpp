#include "CanvasImpl.h"
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

}