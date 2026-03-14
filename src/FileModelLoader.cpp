#include "FileModelLoader.h"

#include "imgui.h"

FileModelLoader::FileModelLoader(Container* cont) : IModelData(cont) {

}

FileModelLoader::~FileModelLoader() {

}

void FileModelLoader::init() {

}

void FileModelLoader::renderGUI() {
	ImGui::Text("File model loader");
}