#pragma once

#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

//#include "implot.h"

/*Responsible for creating glfw and imgui contexts*/
class Window {

public:

	using LoopCallback = void(*)();
	Window(std::string windowTitle, const LoopCallback& loopFunc);
};