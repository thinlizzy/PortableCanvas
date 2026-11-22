#include "GlContext.h"
#include <windows.h>
#include <gl/GL.h>
#include <stdexcept>

namespace pc {

namespace {

HGLRC createOpenGLContext(HDC hdc) {
	auto pfd = PIXELFORMATDESCRIPTOR({ 
		sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd  
		1,                                // version number  
		PFD_DRAW_TO_WINDOW |              // support window  
		PFD_SUPPORT_OPENGL |              // support OpenGL  
		PFD_DOUBLEBUFFER,                 // double buffered  
		PFD_TYPE_RGBA,                    // RGBA type  
		32,                               // 32-bit color depth
		0, 0, 0, 0, 0, 0,                 // color bits ignored  
		0,                                // no alpha buffer  
		0,                                // shift bit ignored  
		0,                                // no accumulation buffer  
		0, 0, 0, 0,                       // accum bits ignored  
		24,                               // 24-bit z-buffer      
		0,                                // no stencil buffer  
		0,                                // no auxiliary buffer  
		PFD_MAIN_PLANE,                   // main layer  
		0,                                // reserved  
		0, 0, 0                           // layer masks ignored  
	});
	auto pixelFormat = ChoosePixelFormat(hdc,&pfd);
	if( pixelFormat == 0 ) throw std::runtime_error(lastWindowsError());
	if( SetPixelFormat(hdc,pixelFormat,&pfd) == 0 ) throw std::runtime_error(lastWindowsError());

	auto hglrc = wglCreateContext(hdc);
	if( hglrc == nullptr ) throw std::runtime_error(lastWindowsError());
	if( wglMakeCurrent(hdc,hglrc) == 0 ) {
		auto error = lastWindowsError();
		wglDeleteContext(hglrc);
		throw std::runtime_error(error);
	}
	return hglrc;
}

void initOpenGLContext() {
	// create bitmaps for the device context font's first 256 glyphs
	wglUseFontBitmaps(wglGetCurrentDC(),0,256,1000);
	glListBase(1000);

	// enable alpha blending for RGBA textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

}

GlContext::GlContext(HWND hwnd):
	hdc_(GetDC(hwnd)),
	hglrc_(nullptr)
{
	if( hdc_ == nullptr ) throw std::runtime_error(lastWindowsError());
	try {
		hglrc_ = createOpenGLContext(hdc_);
		initOpenGLContext();
	} catch(...) {
		if( hglrc_ != nullptr ) {
			wglMakeCurrent(nullptr,nullptr);
			wglDeleteContext(hglrc_);
			hglrc_ = nullptr;
		}
		hdc_ = nullptr;
		throw;
	}
	wglMakeCurrent(nullptr,nullptr);
}

GlContext::~GlContext() {
	invalidate();
}

void GlContext::invalidate() {
	if( hglrc_ == nullptr ) return;
	try {
		auto previousHdc = wglGetCurrentDC();
		auto previousHglrc = wglGetCurrentContext();
		bool current = previousHglrc == hglrc_;
		if( !current ) current = wglMakeCurrent(hdc_,hglrc_) != 0;
		if( current ) {
			glDeleteLists(1000,256);
			wglMakeCurrent(nullptr,nullptr);
			previousHdc = nullptr;
			previousHglrc = nullptr;
		}
		wglDeleteContext(hglrc_);
		wglMakeCurrent(previousHdc,previousHglrc);
	} catch(...) {
	}
	hglrc_ = nullptr;
	hdc_ = nullptr;
}

}