#pragma once

#include <string>

#include "imgui.h"
#include <GL/glew.h>

//#include "implot.h"

/*Responsible for creating glfw and imgui contexts*/
class Window {

public:

	using LoopCallback = void(*)();
	using InitCallback = void(*)();

	Window(std::string windowTitle, const InitCallback& initFunc, const LoopCallback& loopFunc);
};