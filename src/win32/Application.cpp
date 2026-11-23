#include "src/base/Application.h"
#include <windows.h>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include "WindowImpl.h"
#include "windows_error.h"
#include "src/base/Window.h"

namespace pc {

class ApplicationImpl {
public:
	bool running;
	WNDCLASSW windowClass;
	std::unordered_map<HWND,Window> windows;

	ApplicationImpl();
	~ApplicationImpl();
	LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

ApplicationImpl * applicationImplSingleton = nullptr;  // all windows use a global WndProc, so we need a singleton :(

ApplicationImpl * getApplicationInstance() {
	if( applicationImplSingleton != nullptr ) throw std::runtime_error("can't create two or more Application instances");
	return applicationImplSingleton = new ApplicationImpl();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return applicationImplSingleton->WndProc(hWnd,message,wParam,lParam);
}

Application::Application(): impl(getApplicationInstance()) {
}

Application::~Application() {
	impl.reset();
	applicationImplSingleton = nullptr;
}

auto Application::createWindow(WindowParameters parameters) -> Window & {
	// see https://godbolt.org/z/r7Tsq6svz
	auto windowImpl = std::make_unique<WindowImpl>(parameters,impl->windowClass);
	auto hwnd = windowImpl->hwnd;
	auto [it,inserted] = impl->windows.try_emplace(hwnd,windowImpl.get());
	if( !inserted ) throw std::runtime_error("window already registered");
	windowImpl.release();
	return it->second;
}

void Application::processMessages() {
	MSG msg;
	while( PeekMessageW(&msg,NULL,0,0,PM_REMOVE) != 0 ) {
		LOG_APP_MESSAGE("Application",msg.message,msg.hwnd,msg.wParam,msg.lParam);
		// translate before the direct call: a handled WM_KEYDOWN returns early,
		// but the WM_CHAR it generates must still reach the window
		TranslateMessage(&msg);  // post a WM_CHAR message if msg is a WM_KEY message
		auto it = impl->windows.find(msg.hwnd);
		if( it != impl->windows.end() ) {
			// non zero = message was processed so go back to fetch the next message
			if( it->second.impl->processMessage(msg.message,msg.wParam,msg.lParam) ) continue;
		}
		DispatchMessageW(&msg);  // send it to WndProc (suggested by MSDN)
	}
}

void Application::run() {
	do {
		WaitMessage();
		processMessages();
	} while( impl->running );
}

void Application::stop() {
	impl->running = false;
	// TODO post quit message?
}

// ApplicationImpl

WNDCLASSW getWindowClass() {
	WNDCLASSW windowClass;
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = ::pc::WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	windowClass.hbrBackground = HBRUSH(GetStockObject(BLACK_BRUSH));
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = L"portable-canvas";
	return windowClass;
}

ApplicationImpl::ApplicationImpl(): 
		running(true),
		windowClass(getWindowClass()) {
	RegisterClassW(&windowClass);
}

ApplicationImpl::~ApplicationImpl() {
	// move the map out first: destroying the windows sends WM_ messages, and
	// WndProc must not observe the map while it is being cleared
	auto doomed = std::move(windows);
	doomed.clear();
	UnregisterClassW(windowClass.lpszClassName,windowClass.hInstance);
}

LRESULT ApplicationImpl::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LOG_APP_MESSAGE("WndProc",message,hwnd,wParam,lParam);
	auto it = windows.find(hwnd);
	if( it != windows.end() ) {
		auto & window = *it->second.impl;
		auto result = window.processMessage(message,wParam,lParam);
		if( result ) return result;  // non zero = message was processed so return the value to the api

		// now handle the unprocessed important messages
		switch (message) {
			case WM_NCDESTROY:
				windows.erase(it);  // it is pointing to an already gone window, so... goodbye
				if( windows.empty() ) {
					// application is no longer running after the last window is destroyed
					running = false;
				}
				break;
			}
	}

	return DefWindowProcW(hwnd,message,wParam,lParam);
}

}
