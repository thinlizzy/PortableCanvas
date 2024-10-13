cc_library(
	name = "simple-example-lib",
	srcs = ["SimpleExample.cpp"],
	deps = [
		"//src/base",
	],
	copts = ["-Isrc/base"],
)

cc_binary(
	name = "SimpleExample",
	deps = [
		":simple-example-lib",
		"//:pc",
	],
)
