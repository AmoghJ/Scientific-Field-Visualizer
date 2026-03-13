#pragma once

#include "IModelData.h"

class Container;

class FileModelLoader : public IModelData {
public:
	FileModelLoader(Container* cont);
	~FileModelLoader();

	void renderGUI() override;
};
