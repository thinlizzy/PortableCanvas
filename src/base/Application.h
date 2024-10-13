#ifndef PORTABLE_CANVAS_APPLICATION_2024_01_25_H
#define PORTABLE_CANVAS_APPLICATION_2024_01_25_H

#include <memory>
#include "Dimensions.h"
#include "Window.h"
#include "WindowParameters.h"

namespace pc {

class ApplicationImpl;

class Application {
	std::unique_ptr<ApplicationImpl> impl;
public:
	Application();
	~Application();  // need dtor in separate TU for unique_ptr to work with incomplete types
	auto createWindow(WindowParameters parameters) -> Window &;
	void processMessages();
	void run();
	void stop();
};

}

#endif
