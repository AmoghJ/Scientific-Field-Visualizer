#include "ScalarMaps.h"

std::string scalarFuncLabels[8]{
	"Sine Ripples",
	"Product Bands",
	"Twisted Bands",
	"Concentric Shells",
	"Diagonal Gradient",
	"Polar Angle",
	"Checkerboard",
	"Displacement"
};

// Sine ripples from center
ScalarFunc sinRipples = [](glm::vec3 p) -> float {
	float d = glm::length(p);
	return glm::sin(d * 0.8f) * 0.5f + 0.5f;
	};

// XYZ product bands
ScalarFunc productBands = [](glm::vec3 p) -> float {
	return glm::sin(p.x) * glm::sin(p.y) * glm::sin(p.z) * 0.5f + 0.5f;
	};

// Twisted bands
ScalarFunc twistedBands = [](glm::vec3 p) -> float {
	float angle = atan2(p.z, p.x);
	return glm::sin(p.y * 0.5f + angle * 3.0f) * 0.5f + 0.5f;
	};

// Concentric shells
ScalarFunc concShells = [](glm::vec3 p) -> float {
	return glm::fract(glm::length(p) * 0.3f);
	};

// Diagonal gradient with turbulence
ScalarFunc diagonalGrad = [](glm::vec3 p) -> float {
	float base = (p.x + p.y + p.z) / 3.0f;
	float noise = glm::sin(p.x * 1.7f) * glm::cos(p.z * 1.3f) * 0.2f;
	return glm::clamp(base * 0.5f + 0.5f + noise, 0.0f, 1.0f);
	};

// Polar angle
ScalarFunc polarAngle = [](glm::vec3 p) -> float {
	return atan2(p.z, p.x) / (2.0f * 3.14159f) + 0.5f;
	};

// Checkerboard 3D
ScalarFunc checkboard = [](glm::vec3 p) -> float {
	int ix = (int)glm::floor(p.x * 0.3f);
	int iy = (int)glm::floor(p.y * 0.3f);
	int iz = (int)glm::floor(p.z * 0.3f);
	return ((ix + iy + iz) % 2 == 0) ? 1.0f : 0.0f;
	};

ScalarFunc disp = [](glm::vec3 p) -> float {
	return 1.0f;
};

ScalarFunc scalarFunctionsArray[8] = {
	sinRipples,
	productBands,
	twistedBands,
	concShells,
	diagonalGrad,
	polarAngle,
	checkboard,
	disp
};