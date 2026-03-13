#pragma once

#include "IModelData.h"

class FileModelLoader : public IModelData {
public:
	FileModelLoader();
	~FileModelLoader();

	void renderGUI() override;
};
