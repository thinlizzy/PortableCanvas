#include "WindowImpl.h"
#include <windows.h>
#include <gl/GL.h>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include "FontImpl.h"
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

Key wParamToKey(WPARAM wParam, LPARAM lParam) {
	bool extended = (lParam & 0x01000000) != 0;
	switch(wParam) {
		case VK_ESCAPE: return Key::escape;
		case VK_RETURN: return extended ? Key::numpadEnter : Key::enter;
		case VK_TAB: return Key::tab;
		case VK_BACK: return Key::backspace;
		case VK_DELETE: return Key::deleteKey;
		case VK_INSERT: return Key::insert;
		case VK_HOME: return Key::home;
		case VK_END: return Key::end;
		case VK_PRIOR: return Key::pageUp;
		case VK_NEXT: return Key::pageDown;
		case VK_LEFT: return Key::left;
		case VK_RIGHT: return Key::right;
		case VK_UP: return Key::up;
		case VK_DOWN: return Key::down;
		case VK_SPACE: return Key::space;
		case 'A': return Key::a;
		case 'B': return Key::b;
		case 'C': return Key::c;
		case 'D': return Key::d;
		case 'E': return Key::e;
		case 'F': return Key::f;
		case 'G': return Key::g;
		case 'H': return Key::h;
		case 'I': return Key::i;
		case 'J': return Key::j;
		case 'K': return Key::k;
		case 'L': return Key::l;
		case 'M': return Key::m;
		case 'N': return Key::n;
		case 'O': return Key::o;
		case 'P': return Key::p;
		case 'Q': return Key::q;
		case 'R': return Key::r;
		case 'S': return Key::s;
		case 'T': return Key::t;
		case 'U': return Key::u;
		case 'V': return Key::v;
		case 'W': return Key::w;
		case 'X': return Key::x;
		case 'Y': return Key::y;
		case 'Z': return Key::z;
		case '0': return Key::digit0;
		case '1': return Key::digit1;
		case '2': return Key::digit2;
		case '3': return Key::digit3;
		case '4': return Key::digit4;
		case '5': return Key::digit5;
		case '6': return Key::digit6;
		case '7': return Key::digit7;
		case '8': return Key::digit8;
		case '9': return Key::digit9;
		case VK_NUMPAD0: return Key::numpad0;
		case VK_NUMPAD1: return Key::numpad1;
		case VK_NUMPAD2: return Key::numpad2;
		case VK_NUMPAD3: return Key::numpad3;
		case VK_NUMPAD4: return Key::numpad4;
		case VK_NUMPAD5: return Key::numpad5;
		case VK_NUMPAD6: return Key::numpad6;
		case VK_NUMPAD7: return Key::numpad7;
		case VK_NUMPAD8: return Key::numpad8;
		case VK_NUMPAD9: return Key::numpad9;
		case VK_MULTIPLY: return Key::numpadMultiply;
		case VK_ADD: return Key::numpadAdd;
		case VK_SEPARATOR: return Key::numpadSeparator;
		case VK_SUBTRACT: return Key::numpadSubtract;
		case VK_DECIMAL: return Key::numpadDecimal;
		case VK_DIVIDE: return Key::numpadDivide;
		case VK_OEM_1: return Key::semicolon;
		case VK_OEM_PLUS: return Key::equals;
		case VK_OEM_COMMA: return Key::comma;
		case VK_OEM_MINUS: return Key::minus;
		case VK_OEM_PERIOD: return Key::period;
		case VK_OEM_2: return Key::slash;
		case VK_OEM_5: return Key::backslash;
		case VK_OEM_4: return Key::leftBracket;
		case VK_OEM_6: return Key::rightBracket;
		case VK_OEM_7: return Key::quote;
		case VK_OEM_3: return Key::backtick;
		case VK_OEM_8: return Key::oem8;
		case VK_OEM_102: return Key::oem102;
		case VK_F1: return Key::f1;
		case VK_F2: return Key::f2;
		case VK_F3: return Key::f3;
		case VK_F4: return Key::f4;
		case VK_F5: return Key::f5;
		case VK_F6: return Key::f6;
		case VK_F7: return Key::f7;
		case VK_F8: return Key::f8;
		case VK_F9: return Key::f9;
		case VK_F10: return Key::f10;
		case VK_F11: return Key::f11;
		case VK_F12: return Key::f12;
		case VK_F13: return Key::f13;
		case VK_F14: return Key::f14;
		case VK_F15: return Key::f15;
		case VK_F16: return Key::f16;
		case VK_F17: return Key::f17;
		case VK_F18: return Key::f18;
		case VK_F19: return Key::f19;
		case VK_F20: return Key::f20;
		case VK_F21: return Key::f21;
		case VK_F22: return Key::f22;
		case VK_F23: return Key::f23;
		case VK_F24: return Key::f24;
		case VK_CAPITAL: return Key::capsLock;
		case VK_NUMLOCK: return Key::numLock;
		case VK_SCROLL: return Key::scrollLock;
		case VK_SNAPSHOT: return Key::printScreen;
		case VK_PAUSE: return Key::pause;
		case VK_SHIFT: return extended ? Key::rightShift : Key::leftShift;
		case VK_CONTROL: return extended ? Key::rightControl : Key::leftControl;
		case VK_MENU: return extended ? Key::rightAlt : Key::leftAlt;
		case VK_LWIN: return Key::leftWin;
		case VK_RWIN: return Key::rightWin;
		case VK_APPS: return Key::menu;
		case VK_CLEAR: return Key::clear;
		case VK_HELP: return Key::help;
		case VK_SELECT: return Key::select;
		case VK_EXECUTE: return Key::execute;
		case VK_PRINT: return Key::print;
		case VK_KANA: return Key::kana;
		case VK_JUNJA: return Key::junja;
		case VK_FINAL: return Key::finalKey;
		case VK_KANJI: return Key::kanji;
		case VK_CONVERT: return Key::convert;
		case VK_NONCONVERT: return Key::nonConvert;
		case VK_ACCEPT: return Key::accept;
		case VK_MODECHANGE: return Key::modeChange;
		case VK_BROWSER_BACK: return Key::browserBack;
		case VK_BROWSER_FORWARD: return Key::browserForward;
		case VK_BROWSER_REFRESH: return Key::browserRefresh;
		case VK_BROWSER_STOP: return Key::browserStop;
		case VK_BROWSER_SEARCH: return Key::browserSearch;
		case VK_BROWSER_FAVORITES: return Key::browserFavorites;
		case VK_BROWSER_HOME: return Key::browserHome;
		case VK_VOLUME_MUTE: return Key::volumeMute;
		case VK_VOLUME_DOWN: return Key::volumeDown;
		case VK_VOLUME_UP: return Key::volumeUp;
		case VK_MEDIA_NEXT_TRACK: return Key::mediaNextTrack;
		case VK_MEDIA_PREV_TRACK: return Key::mediaPrevTrack;
		case VK_MEDIA_STOP: return Key::mediaStop;
		case VK_MEDIA_PLAY_PAUSE: return Key::mediaPlayPause;
		case VK_LAUNCH_MAIL: return Key::launchMail;
		case VK_LAUNCH_MEDIA_SELECT: return Key::launchMediaSelect;
		case VK_LAUNCH_APP1: return Key::launchApp1;
		case VK_LAUNCH_APP2: return Key::launchApp2;
		case VK_SLEEP: return Key::sleep;
		default: return Key::unknown;
	}
}

KeyModifiers keyModifiers() {
	return KeyModifiers{
		.controlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0,
		.shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0,
		.altPressed = (GetKeyState(VK_MENU) & 0x8000) != 0
	};
}

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
	if( timerId != 0 ) {
		KillTimer(hwnd,timerId);
		timerId = 0;
	}
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
			if( timerId != 0 ) {
				KillTimer(hwnd,timerId);
				timerId = 0;
			}
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
				keyDownEvent(wParamToKey(wParam,lParam),keyModifiers());
				return 1;
			}
			break;
		case WM_TIMER:
			if( timerEvent && wParam == timerId ) {
				timerEvent();
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

void WindowImpl::startTimer(int milliseconds) {
	if( hwnd == nullptr ) throw std::runtime_error("window is not valid");
	stopTimer();
	timerId = SetTimer(hwnd,0,UINT(milliseconds),nullptr);
	if( timerId == 0 ) throw std::runtime_error(lastWindowsError());
}

void WindowImpl::stopTimer() {
	if( timerId != 0 ) {
		KillTimer(hwnd,timerId);
		timerId = 0;
	}
}

Image WindowImpl::createImage(Dimensions dimensions, BytesPerPixel bpp, ImageBuffer bytes) {
	if( !gl ) throw std::runtime_error("window has no OpenGL context");
	return Image(std::make_unique<ImageImpl>(gl,dimensions,bpp,bytes));
}

Font WindowImpl::createFont(FontParameters const & parameters) {
	if( !gl ) throw std::runtime_error("window has no OpenGL context");
	return Font(std::make_unique<FontImpl>(gl,parameters));
}

}
