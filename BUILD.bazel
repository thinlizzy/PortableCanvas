package(default_visibility = ["//visibility:public"])

cc_library(
	name = "pc",
	deps = select({
		"@bazel_tools//src/conditions:windows": ["//src/win32"],
		"//conditions:default": ["//src/linux"],
	}),
)
