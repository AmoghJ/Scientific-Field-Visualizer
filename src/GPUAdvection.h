#pragma once

#include <memory>
#include <functional>
#include "IModelData.h"
#include <string>
#include <map>

#include "gl/glew.h"

class Container;

class GPUAdvection : public IModelData {

	void runAdvectionPass();

public:
	GPUAdvection(Container* cont);
	~GPUAdvection();

	void init() override;
	void renderGUI() override;
};