#include "WindowImpl.h"
#include <windows.h>
#include <gl/GL.h>
#include <cstdlib>
#include <stdexcept>
#include "windows_error.h"
#include "CanvasImpl.h"
#include "ImageImpl.h"
#include "src/base/Dimensions.h"
#include "src/base/Point.h"
#include "src/base/Rect.h"

namespace pc {

Dimensions resizeDimensions(Dimensions dims, DWORD dwStyle, DWORD dwExStyle) {
	RECT rect = {0,0,dims.width,dims.height};
	AdjustWindowRectEx(
		&rect,
		dwStyle,
		false,
		dwExStyle
	);
	return Dimensions({rect.right-rect.left, rect.bottom-rect.top});
}

static HWND create(WindowParameters parameters, WNDCLASSW const & windowClass) {
	auto dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	auto windowPos = Point({CW_USEDEFAULT, CW_USEDEFAULT});
	// TODO get window pos from parameters
	auto newDimensions = parameters.isDefaultDimensions() 
		? Dimensions({CW_USEDEFAULT, CW_USEDEFAULT}) 
		: resizeDimensions(parameters.dimensions,dwStyle,WS_EX_APPWINDOW);
	// get windowname from the string variant
	wchar_t const * windowname = L"";
	std::wstring windownameV;
	std::visit([&](auto && s) {
		using T = std::decay_t<decltype(s)>;
		if constexpr( std::is_same_v<T, std::string> ) {
			windownameV.resize(s.size());
			std::mbstowcs(windownameV.data(),s.data(),s.size());
			windowname = windownameV.c_str();
		} else
		if constexpr( std::is_same_v<T, std::wstring> ) {
			windowname = s.c_str();
		}
	}, parameters.name);

	HWND hWnd = CreateWindowExW(
		WS_EX_APPWINDOW,
		windowClass.lpszClassName, 
		windowname,
		dwStyle,
		windowPos.x, windowPos.y,
		newDimensions.width, newDimensions.height,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);
	if( hWnd == NULL ) {
		throw std::runtime_error(lastWindowsError());
	}
	return hWnd;
}

void createOpenGLContext(HWND hwnd) {
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
	auto hdc = GetDC(hwnd);
	if( auto iPixelFormat = ChoosePixelFormat(hdc,&pfd) ) {
		if( SetPixelFormat(hdc,iPixelFormat,&pfd) ) {
			if( auto hglrc = wglCreateContext(hdc) ) {
				if( wglMakeCurrent(hdc,hglrc) ) {
					goto okay;
				}
			}
		}
	}
	throw std::runtime_error(lastWindowsError());

	okay:
	// create bitmaps for the device context font's first 256 glyphs
	wglUseFontBitmaps(hdc,0,256,1000);
	glListBase(1000);
}

void destroyOpenGLContext(HWND hwnd) {
	if( auto hglrc = wglGetCurrentContext() ) {
		glDeleteLists(1000,256);

		auto hdc = wglGetCurrentDC();
		wglMakeCurrent(NULL,NULL);
		ReleaseDC(hwnd,hdc);
		wglDeleteContext(hglrc);
	}
}

void resizeOpenGLContext(UINT width, UINT height) {
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(
		0.0f,width,  // left,right
		height,0.0f, // bottom,top
		0.0f,1.0f);  // zNear,zFar
	glMatrixMode(GL_MODELVIEW);
}

// TODO properly implement wParamToKey
Key wParamToKey(WPARAM wParam) { return Key(wParam); }

Rect wRectToRect(RECT rect) {
	return Rect({.leftTop=Point({.x=rect.left, .y=rect.top}), .rightBottom=Point({.x=rect.right, .y=rect.bottom})});
}

Point lParamToPoint(LPARAM lParam) {
	POINTS p = MAKEPOINTS(lParam);
	return Point{.x=p.x, .y=p.y};
}

MouseHits toMouseHits(UINT message, WPARAM wParam) {
	MouseHits result = {};
	switch(message) {
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			result.button = MouseButton::left;
			break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			result.button = MouseButton::right;
			break;
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			result.button = MouseButton::middle;
			break;
		default:
			// TODO log that
			break;
	}
	result.controlPressed = wParam & MK_CONTROL;
	result.shiftPressed = wParam & MK_SHIFT;
	return result;
}

// WindowImpl //

WindowImpl::WindowImpl(WindowParameters parameters, WNDCLASSW const & windowClass):
	hwnd(create(parameters,windowClass)),
	visible(true)
{
	createOpenGLContext(hwnd);
	RECT rect;
	GetClientRect(hwnd,&rect);
	resizeOpenGLContext(rect.right,rect.bottom);
}

LRESULT WindowImpl::processMessage(UINT message, WPARAM wParam, LPARAM lParam) { 
	switch(message) {
		// events we handle internally
		case WM_DESTROY:
			destroyOpenGLContext(hwnd);
			break;
		case WM_SIZE:
			if( wParam != SIZE_MINIMIZED ) {
				LOG_MESSAGE("resizeOpenGLContext", message, hwnd, wParam, lParam);
				resizeOpenGLContext(LOWORD(lParam),HIWORD(lParam));
			}
			break;
		
		// events that can be handled by the user
		case WM_KEYDOWN:
			if( keyDownEvent ) {
				LOG_MESSAGE("keyDownEvent", message, hwnd, wParam, lParam);
				keyDownEvent(wParamToKey(wParam));
				return 1;
			}
			break;
		case WM_PAINT:
			if( paintEvent ) {
				// LOG_MESSAGE("paintEvent", message, hwnd, wParam, lParam);
				PAINTSTRUCT ps;
				BeginPaint(hwnd,&ps);
				auto canvas = CanvasImpl();
				paintEvent(canvas,wRectToRect(ps.rcPaint));
				EndPaint(hwnd,&ps);
				return 1;
			}
			// the logging below was used to confirm the OpenGL context is already working during the paint event
			// LOG_MESSAGE(reinterpret_cast<char const *>(glGetString(GL_VERSION)), message, hwnd, wParam, lParam);
			break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			if( mouseDownEvent ) {
				LOG_MESSAGE("mouseDownEvent", message, hwnd, wParam, lParam);
				mouseDownEvent(toMouseHits(message, wParam),lParamToPoint(lParam));
				return 1;
			}
			break;
		case WM_CLOSE:
			// TODO implement closeEvent
			// if( closeEvent ) {
			// 	auto canClose = closeEvent();
			// 	if( canClose ) {
			// 		visible = false;
			// 	}
			// 	return 1;
			// }
			visible = false;
			break;

	}
	return 0;
}

void WindowImpl::repaint() {
	InvalidateRect(hwnd,0,false);
}

Image WindowImpl::createImage(Dimensions dimensions, BytesPerPixel bpp, ImageBuffer bytes) {
	return Image(std::make_unique<ImageImpl>(dimensions,bpp,bytes));
}

}
