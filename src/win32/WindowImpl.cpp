#include "WindowImpl.h"
#include <windows.h>
#include <gl/GL.h>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include "windows_error.h"
#include "CanvasImpl.h"
#include "GlContext.h"
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
		case WM_MOUSEMOVE:
			if( wParam & MK_LBUTTON ) result.button = MouseButton::left;
			else if( wParam & MK_RBUTTON ) result.button = MouseButton::right;
			else if( wParam & MK_MBUTTON ) result.button = MouseButton::middle;
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
	gl(nullptr),
	visible(true)
{
	try {
		gl = std::make_shared<GlContext>(hwnd);
		RECT rect;
		GetClientRect(hwnd,&rect);
		gl->withCurrent([&] { resizeOpenGLContext(rect.right,rect.bottom); });
	} catch(...) {
		release();
		throw;
	}
}

WindowImpl::~WindowImpl() {
	release();
}

void WindowImpl::release() {
	if( gl ) gl->invalidate();
	gl.reset();
	if( hwnd != nullptr ) {
		DestroyWindow(hwnd);
		hwnd = nullptr;
	}
}

LRESULT WindowImpl::processMessage(UINT message, WPARAM wParam, LPARAM lParam) { 
	switch(message) {
		// events we handle internally
		case WM_DESTROY:
			// release GL resources while the window (and its device context) still exist
			if( gl ) gl->invalidate();
			gl.reset();
			break;
		case WM_SIZE:
			if( wParam != SIZE_MINIMIZED ) {
				LOG_MESSAGE("resizeOpenGLContext", message, hwnd, wParam, lParam);
				gl->withCurrent([&] { resizeOpenGLContext(LOWORD(lParam),HIWORD(lParam)); });
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
				gl->withCurrent([&] {
					auto canvas = CanvasImpl();
					paintEvent(canvas,wRectToRect(ps.rcPaint));
				});
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
				SetCapture(hwnd);
				mouseDownEvent(toMouseHits(message, wParam),lParamToPoint(lParam));
				return 1;
			}
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			ReleaseCapture();
			if( mouseUpEvent ) {
				LOG_MESSAGE("mouseUpEvent", message, hwnd, wParam, lParam);
				mouseUpEvent(toMouseHits(message, wParam),lParamToPoint(lParam));
				return 1;
			}
			break;
		case WM_MOUSEMOVE:
			if( mouseMoveEvent ) {
				mouseMoveEvent(toMouseHits(message, wParam),lParamToPoint(lParam));
				return 1;
			}
			break;
		case WM_CHAR:
			if( charEvent ) {
				auto code = static_cast<std::uint32_t>(wParam);
				if( code >= 0xD800 && code <= 0xDBFF ) {
					pendingHighSurrogate = static_cast<wchar_t>(wParam);
					return 1;
				}
				if( code >= 0xDC00 && code <= 0xDFFF && pendingHighSurrogate != 0 ) {
					code = 0x10000 +
						((static_cast<std::uint32_t>(pendingHighSurrogate) - 0xD800) << 10) +
						(code - 0xDC00);
				}
				pendingHighSurrogate = 0;
				charEvent(code);
				return 1;
			}
			break;
		case WM_CLOSE:
			if( closeEvent && ! closeEvent() ) return 1;
			visible = false;
			break;

	}
	return 0;
}

void WindowImpl::repaint() {
	InvalidateRect(hwnd,0,false);
}

void WindowImpl::close() {
	if( hwnd != nullptr ) PostMessageW(hwnd,WM_CLOSE,0,0);
}

Image WindowImpl::createImage(Dimensions dimensions, BytesPerPixel bpp, ImageBuffer bytes) {
	if( !gl ) throw std::runtime_error("window has no OpenGL context");
	return Image(std::make_unique<ImageImpl>(gl,dimensions,bpp,bytes));
}

}
