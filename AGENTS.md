# PortableCanvas

A small portable library that hides the window-system API (Win32, X11) and
OpenGL behind high-level objects for windows, their events, drawing and
images. Windows is implemented; the Linux/X11 backend is **TBD**.

This file is a briefing for AI agents. Keep it concise and current.

## What it is

- **Platform-neutral public API** lives in `src/base/`: `Application`,
  `Window`, `Canvas`, `Image`, plus header-only value types (`Point`,
  `Dimensions`, `Rect`, `Color`) and literals (`_x`, `_y`, `_w`, `_h`,
  `_r`, `_g`, `_b`).
- **Platform backends** live in `src/win32/` (Win32 + fixed-function OpenGL)
  and `src/linux/` (stub only, no sources yet).
- Consumers depend on `@portable-canvas//src/base` for headers and
  `@portable-canvas//:pc` for the implementation. `//:pc` selects
  `//src/win32` or `//src/linux` by platform and has no sources of its own —
  do not put code there.
- `src/base` must never include or depend on platform headers. Keeping the
  split clean is what makes the Linux port tractable.

```
src/base   headers only, no deps          (include as "portablecanvas/...")
src/win32  Application.cpp, Window(Impl), CanvasImpl, Image(Impl), GlContext, windows_error
src/linux  BUILD.bazel only, srcs = []
examples   SimpleExample.cpp, ImageExample.cpp (smoke tests)
```

## Object model and lifetime invariants

- **`Application` is a process-wide singleton.** Win32 needs one `WndProc`,
  so `src/win32/Application.cpp` keeps `applicationImplSingleton`. A second
  live `Application` throws; `~Application` resets the pointer and destroys
  its `ApplicationImpl`, which clears the window map (destroying each
  `WindowImpl`/HWND) and unregisters the window class, so a later
  `Application` can be created again.
- **Windows live in `unordered_map<HWND, Window>` owned by
  `ApplicationImpl`.** `createWindow` returns a `Window &` into that map.
  The entry is erased on `WM_NCDESTROY`, so a cached reference dangles once
  the window is closed/destroyed. There is no API to destroy a window; only
  the OS does, and `WM_CLOSE` merely sets `visible = false` before
  `DefWindowProcW` destroys it.
- **Windows live in `unordered_map<HWND, Window>` owned by
  `ApplicationImpl`.** `createWindow` returns a `Window &` into that map.
  The entry is erased on `WM_NCDESTROY`, so a cached reference dangles once
  the window is closed/destroyed. There is no API to destroy a window; only
  the OS does, and `WM_CLOSE` merely sets `visible = false`.
- **`Application::run` blocks in `WaitMessage`.** `stop()` only breaks the
  loop when called from inside a message callback; it does not post
  `WM_QUIT`. Do not call UI APIs from other threads — the backend is
  single-threaded.
- **`Window` is a pimpl owning `WindowImpl`** (HWND + GL context). It is
  non-copyable and effectively non-movable (user-declared destructor).
- **`Canvas` is ephemeral.** `CanvasImpl` is constructed on the stack inside
  `WM_PAINT`, inside `GlContext::withCurrent`, and swaps the buffers in its
  destructor. Never store or reuse a `Canvas`.
- **`Image` is move-constructible, not move-assignable**, and owns an
  `ImageImpl` texture handle bound to the creating window's context. `impl`
  is public because `CanvasImpl` needs the texture id.
- **`WindowImpl` owns its `HWND` and a `std::shared_ptr<GlContext>`.** Its
  constructor creates the HWND, then the context; if context creation throws
  it calls `DestroyWindow` before rethrowing, so no HWND leaks on a failed
  construction. `createWindow` builds a `unique_ptr<WindowImpl>` and only
  releases it into the map after `try_emplace` succeeds, so a container
  allocation failure also cleans up the HWND.
- **`GlContext` (new, `src/win32/GlContext.*`) owns one window's `HDC` and
  `HGLRC`.** The class sets/restores the process-wide current context,
  builds the font display lists per context, and tears everything down in
  `invalidate()`. `CS_OWNDC` means the DC lives as long as the window, so it
  is not released explicitly.

## OpenGL context model (important)

GL's current context is **process-global**; `wglMakeCurrent` switches it for
the whole process. The library now supports **several windows**, each with
its own `GlContext`:

- `GlContext(HWND)` creates the pixel format and `HGLRC`, makes it current,
  runs `initOpenGLContext()` (font display lists 1000..1255, `GL_BLEND`),
  then unbinds. Nothing stays current between operations.
- Every GL operation borrows the context: `GlContext::withCurrent(f)` makes
  this window's context current, runs `f`, and restores the previously
  current context (even on exceptions). Use it for paint, resize, image
  creation/deletion, and teardown; never assume "the current context is
  mine".
- `WM_PAINT` constructs `CanvasImpl` *inside* `withCurrent` so the destructor
  can `SwapBuffers` before the context is released.
- Display lists are per context, so each window builds and deletes its own
  copy (`glDeleteLists(1000,256)` in `invalidate()`).
- **Textures belong to the context that created them.** `ImageImpl` keeps a
  `shared_ptr` to the owning `GlContext` and makes it current for upload and
  deletion. An image created by window A cannot be drawn by window B — its
  texture name does not exist there.
- `invalidate()` is idempotent and no-throw. It runs on `WM_DESTROY` (while
  the HWND/DC are still valid) and from the destructor. Images that outlive
  the window keep the `GlContext` object alive but see `valid() == false`
  and skip GPU calls, leaking nothing at destruction time.
- Do not destroy a window's context implicitly via
  `wglGetCurrentContext()`-style lookups. The old `destroyOpenGLContext`
  did exactly that and deleted the wrong window's context; it is gone.

## Rendering details

- Fixed-function GL only: `glBegin`/`glEnd`, `glOrtho`, `GL_TEXTURE_2D`.
- `CanvasImpl`'s constructor clears color/depth; each draw call sets color
  itself (`glColor3ub`) — there is no persistent drawing state.
- Text uses display lists 1000..1255 built by `wglUseFontBitmaps(hdc,0,256,1000)`
  and `glCallLists`; it is single-line ASCII only.
- `bppToOpenGlFormat`: 1→`GL_RED`, 3→`GL_RGB`, 4→`GL_RGBA`; the internal
  format is always `GL_RGBA`. Only 3/4 are realistically exercised.
- `drawImage` sets `glColor4f(1,1,1,1)` as a workaround (marked TODO).

## Events

- Hooks are `std::function`s in `WindowEvents.h`: `PaintEvent`
  (`Canvas &`, update `Rect`), `KeyDownEvent` (`Key`), `MouseDownEvent`
  (`MouseHits`, `Point`). There are no move/up/wheel/close hooks yet.
- `processMessage` returns nonzero only for user events it handled.
  `Application::processMessages` skips `DispatchMessageW` for those, which
  prevents user callbacks firing twice. Internal messages (`WM_SIZE`,
  `WM_DESTROY`, `WM_NCDESTROY`) are seen twice — once directly and once via
  `WndProc` — so keep any new internal handling idempotent.
- `wParamToKey` is a raw cast into an empty `Key` enum; key events carry no
  meaningful value yet. `MouseHits` reports button plus Ctrl/Shift only, and
  `extra1`/`extra2` are unused.
- Unhandled messages fall through to `DefWindowProcW`.

## Value types and literals

```cpp
using namespace pc::literals;
auto dims = 300_w + 300_h;          // Dimensions{300,300}
auto p    = 10_x + 20_y;            // Point{10,20}
auto col  = 255_g + 32_r;           // Color, no alpha
auto r    = Rect::open(p, dims);    // Rect{leftTop, rightBottom}
```

- `Point`/`Dimensions` support `+ - * /` (arithmetic concept), compound
  assignment and per-axis literals. Combining `_w` and `_h` is how you build
  a `Dimensions`.
- `Rect` is two corner `Point`s; `contains` uses **inclusive** bounds.
- `Color` is `uint8_t r,g,b` — no alpha, no equality/printing operators.
- `WindowParameters::name` is
  `std::variant<std::monostate, std::string, std::wstring>`; empty dimensions
  mean "use the platform default".

## Build & test

Build system: **Bazel** (Bzlmod, `MODULE.bazel`). Run from the repo root.

- Build the aggregate library: `bazel build //:pc`
- Bazel 9 removed the native `cc_*` rules, so every `BUILD.bazel` and the
  generated repo build files must `load("@rules_cc//cc:...")`; `MODULE.bazel`
  depends on `rules_cc` and `platforms`. Platform selects use
  `@platforms//os:windows`, not `@bazel_tools//src/conditions:windows`.
  This repo has no `.bazelversion`, so the installed Bazel version decides
  whether the files load.
- Run examples (Windows only today):
  `bazel run //examples:SimpleExample`, `bazel run //examples:ImageExample`
- There are **no `cc_test` targets**. `//examples:*` are the smoke tests.
  Geometry/value types in `src/base` are header-only and can be unit-tested
  without a window; add tests there for logic changes, not for GL behavior.
- `MODULE.bazel` contains **machine-specific absolute paths** for the Windows
  SDK (`C:/Program Files (x86)/Windows Kits/...`) and `/usr/lib`. Don't edit
  or commit them without asking; the README tells users to set their own.
- Local sibling-checkout override:
  `bazel <cmd> --override_module=portable-canvas=../PortableCanvas`
  (from a consumer repo).
- `examples/BUILD.bazel` hardcodes `BAZEL_DATA_PATH=../../../../../examples/`
  to find assets under `bazel run`. That hack is example-only; don't reuse it
  in library code — use runfiles or an explicit tool.

Agent rules (carried over from the CardsServer briefing):

- Use a dedicated output base, e.g.
  `bazel --output_base=<agent-only path> <cmd> ...`. Sharing the interactive
  user's output base with different flags can shut down their Bazel server
  and discard its analysis cache.
- **Never run `bazel shutdown` or `bazel clean`.**
- Prefer `bazel build`/`bazel test` on the narrow target rather than the whole
  repo; combine selectors in one invocation.

## Linux/X11 port (TBD)

- Implement `ApplicationImpl`, `WindowImpl`, `CanvasImpl`, `ImageImpl` under
  `src/linux/`; keep `src/base` untouched and the same public API.
- Replace `srcs = []` with `glob(["*.cpp"])` and add `hdrs`.
- `@linux_libs//:x11` currently lists only `libX11.so`; a real backend also
  needs GL/GLX libs and an X11 include path.
- `.bazelrc` sets `--cxxopt=-std:c++20`, which is MSVC syntax. Linux builds
  need a platform-aware flag (`select`able or a toolchain config), otherwise
  GCC rejects it.
- Until sources exist, `//:pc` links nothing on Linux and the examples fail
  to link. That is expected, not a regression.

## Style and code quality

Follow the sibling repo's `style_guide/README.md`: Google C++, **tabs for
indentation**, C++20. This repo has no `.clang-format`; match surrounding
code and don't reformat unrelated whitespace (some files, e.g. `Color.h`,
already use spaces — leave them).

- Compact style: `if( ... )`, `for( auto && x : ... )`, pimpl with a private
  `std::unique_ptr<Impl>` and the destructor defined in the `.cpp`.
- Head-style include guards: `PORTABLE_CANVAS_<FILE>_<YYYY_MM_DD>_H`.
- IWYU: include what you name directly. Platform headers (`<windows.h>`,
  `<gl/GL.h>`) belong only in `src/win32`. Don't include headers for types
  that only flow through `auto`, return values, or members.
- Don't copy header comments into the `.cpp`.
- No cargo-cult comments and none that restate the C++ standard; comment only
  non-obvious behavior (GL context assumptions, `processMessage` return
  protocol, singleton).
- Error handling: use `std::runtime_error` with `lastWindowsError()` for
  Win32/GL failures (all fixed throw sites now do this). Never let a
  destructor throw; `~ImageImpl`, `~CanvasImpl` and `GlContext::invalidate`
  swallow errors.
- Logging: `windows_error.h` defines `LOG_MESSAGE` (active) and
  `LOG_APP_MESSAGE` (deliberately empty). It used to define `LOG_MESSAGE`
  twice, which warned on every compile; that is fixed. Keep new logging
  behind `LOG_MESSAGE` rather than always-on `std::cerr`.

## Known TODOs and defects

- `Key` enum is empty; `wParamToKey` is a raw `WPARAM` cast.
- `Image`'s move constructor is not `noexcept`; move assignment is absent.
- An image that outlives its window cannot free its GPU storage (the context
  is already gone); it only drops the handle. Prefer destroying images while
  their window lives.
- `CanvasImpl` draws `Image`s from other windows' contexts as if the texture
  existed; guard against cross-window images if you expose longer-lived
  canvases.
- No close/move/scroll events; `WM_CLOSE` handling is commented out and
  hard-sets `visible = false`.
- `CanvasImpl` still swaps buffers through `wglGetCurrentDC()`; that is safe
  only because paint runs inside `GlContext::withCurrent`.
- `Application::stop()` does not post `WM_QUIT`; it relies on the message
  loop checking `running` after `processMessages`.
- `windows_error.cpp` truncation warning on `HWND` was fixed by formatting
  with `uintptr_t`; keep pointer formats width-safe on x64.

## Working rules for agents

- Make surgical changes; preserve the base/platform split and public API.
- Any behavior change in the backend should be verified through
  `bazel build //:pc` plus `bazel build //examples:all`. GL behavior cannot
  be validated headlessly, but window lifetime can: run a two-window binary,
  close one window and confirm the other keeps painting, then close the last
  one and confirm the process exits. There is no `--config=ci` in this repo.
- Update this file when you add a backend, change an invariant, or fix one of
  the defects above.
