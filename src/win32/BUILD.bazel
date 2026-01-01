package(default_visibility = ["//visibility:public"])

cc_library(
	name = "win32",
	srcs = glob([
		"*.cpp",
	]),
	hdrs = glob([
		"*.h",
	]),
	deps = [
		"//src/base",
		"@win32_libs//:win32",
	],
)
