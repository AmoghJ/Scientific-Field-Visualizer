#include "Context_Container.h"

Context::Context() : viewer(this), advViewer(this), camera(this), model(this) {

	Subscribe<ChangedLoader>([this](const ChangedLoader& cL) {
		currentLoaderIndex = cL.loaderIndex;
	});
}

Context::~Context() {

}

void Context::init() {
	viewer.init();
	advViewer.init();

	model.init();
}

void Context::loop() {

	model.guiRender();

	if (currentLoaderIndex == 0) {
		viewer.update();
		viewer.render();
	}
	else if (currentLoaderIndex == 1) {
		advViewer.update();
		advViewer.render();
	}
}