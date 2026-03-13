#include "Context_Container.h"

Context::Context() : viewer(this) {

}

Context::~Context() {

}

void Context::init() {
	viewer.init();
}

void Context::loop() {

	viewer.update();
	viewer.render();
}