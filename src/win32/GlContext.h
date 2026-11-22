#ifndef PORTABLE_CANVAS_GL_CONTEXT_2026_11_22_H
#define PORTABLE_CANVAS_GL_CONTEXT_2026_11_22_H

#include <windows.h>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include "windows_error.h"

namespace pc {

// Owns one window's device context and OpenGL rendering context. The GL
// current-context state is process-wide, so operations borrow it through
// withCurrent() and restore whatever context was current before.
class GlContext {
	HDC hdc_;
	HGLRC hglrc_;

public:
	explicit GlContext(HWND hwnd);
	~GlContext();

	GlContext(GlContext const &) = delete;
	GlContext & operator=(GlContext const &) = delete;

	// Destroys the GL context and releases the device context. Safe to call
	// more than once; images keep a share of this object and check valid()
	// before touching the GPU.
	void invalidate();
	bool valid() const { return hglrc_ != nullptr; }

	template<typename F>
	auto withCurrent(F && f) -> std::invoke_result_t<F> {
		using Result = std::invoke_result_t<F>;
		if( !valid() ) throw std::runtime_error("OpenGL context is no longer valid");
		auto previousHdc = wglGetCurrentDC();
		auto previousHglrc = wglGetCurrentContext();
		if( previousHglrc != hglrc_ && wglMakeCurrent(hdc_,hglrc_) == 0 ) {
			throw std::runtime_error(lastWindowsError());
		}
		try {
			if constexpr( std::is_void_v<Result> ) {
				std::invoke(f);
				wglMakeCurrent(previousHdc,previousHglrc);
			} else {
				Result result = std::invoke(f);
				wglMakeCurrent(previousHdc,previousHglrc);
				return result;
			}
		} catch(...) {
			wglMakeCurrent(previousHdc,previousHglrc);
			throw;
		}
	}
};

}

#endif
