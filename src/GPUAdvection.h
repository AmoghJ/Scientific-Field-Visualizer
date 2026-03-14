#pragma once

#include <memory>
#include <functional>
#include "IModelData.h"
#include <string>
#include <map>
#include <chrono>

#include "gl/glew.h"

class Container;

class GPUAdvection : public IModelData {
public:
	GPUAdvection(Container* cont);
	~GPUAdvection();

	void init() override;
	void renderGUI() override;

private:
	void runAdvectionPass();

	bool advectionRunning = false;
	bool resetAdvection = false;

	GLuint computeProgram;

	GLuint timeLocation, deltaTimeLocation, numParticlesLocation,
		resetLocation, maxAgeLocation, boundsSizeLocation, trailLengthLocation;

	float time, deltaTime, maxAge = 5.0f;

	std::chrono::high_resolution_clock::time_point lastTime;
	bool timerInitialized = false;
};