#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <map>

#include "Component.h"
class Container;

#include "IModelData.h"

class Model : public Component {
private:
	
	IModelData* currentModelLoader = nullptr;

	std::string meshTypeLabels[2] = {
		"Marching Cubes (procedural)",
		"GPU Particle Advection"
	};

	enum MeshType {
		Marching_Cubes, GPUAdvec
	} selectedMeshType;

	std::map<MeshType, std::unique_ptr<IModelData>> loaders;

public:
	Model(Container* cont);
	~Model();

	void init();
	void guiRender();
};