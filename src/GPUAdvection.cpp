#include "GPUAdvection.h"

#include "imgui.h"

GPUAdvection::GPUAdvection(Container* cont) : IModelData(cont) {

}

GPUAdvection::~GPUAdvection() {

}

void GPUAdvection::init() {

}

void GPUAdvection::runAdvectionPass() {

}

void GPUAdvection::renderGUI() {
	ImGui::Text("Advection Settings");
}