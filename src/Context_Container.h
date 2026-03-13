#pragma once

#include "Container.h"

#include "OpenGLViewer.h"
#include "OrbitCamera.h"
#include "Model.h"

/*
* The context class binds the model, view, and controller together. It
* inherits the Container class and is responsible for managing messages
* between the various modules.
* 
* This is ideal if in the future we want to manage multiple visualizations
* or serialize the visualizations and eventually save to file.
*/
class Context : public Container {
private:
	OpenGLViewer viewer;
	OrbitCamera camera;
	Model model;

public:
	Context();
	~Context();

	void init();
	void loop();
};