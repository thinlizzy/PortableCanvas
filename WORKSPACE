# win32

new_local_repository(
	name = "win32_libs",
	# NOTE: replace with your own Windows libraries path
	path = "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.18362.0/um/x64",
	build_file_content = """
cc_library(
	name = "win32",
	srcs = [
		"Gdi32.lib",
		"User32.lib",
		"OpenGL32.lib",
	],
	visibility = ["//visibility:public"],
)
""",
)

# TODO review linux

new_local_repository(
	name = "linux_libs",
	path = "/usr/lib",
	build_file_content = """
cc_library(
	name = "x11",
	# pkg-config --variable=libdir x11
	srcs = ["x86_64-linux-gnu/libX11.so"],
	visibility = ["//visibility:public"],
)
""",
)


# TODO implement WASM repo
