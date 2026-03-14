#include "Context_Container.h"

Context::Context() : viewer(this), camera(this), model(this) {

}

Context::~Context() {

}

void Context::init() {
	viewer.init();
	model.init();
}

void Context::loop() {

	model.guiRender();

	viewer.update();
	viewer.render();
}