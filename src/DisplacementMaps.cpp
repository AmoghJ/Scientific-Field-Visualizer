#include "DisplacementMaps.h"


std::string dispFuncLabels[8]{
    "Twist",
    "Ripple",
    "Curl",
    "Pinch",
    "Shockwave",
    "Squish",
    "Spike",
    "Breathe"
};


DispFunc twist = [](glm::vec3 p) -> glm::vec3 {
    float angle = p.y * 0.3f;
    return glm::vec3(glm::cos(angle) * p.x - p.x, 0, glm::sin(angle) * p.x);
    };

// Ripple from center — expanding rings
DispFunc ripple = [](glm::vec3 p) -> glm::vec3 {
    float d = glm::length(p);
    float wave = glm::sin(d * 2.0f) * 0.3f;
    return glm::normalize(p + glm::vec3(0.001f)) * wave;
    };

// Noise-like curl — swirling displacement
DispFunc curl = [](glm::vec3 p) -> glm::vec3 {
    return glm::vec3(
        glm::sin(p.y * 1.3f + p.z * 0.7f),
        glm::sin(p.z * 1.1f + p.x * 0.9f),
        glm::sin(p.x * 1.7f + p.y * 0.5f)
    ) * 0.25f;
    };

// Pinch toward axis — sucked into Y axis
DispFunc pinch = [](glm::vec3 p) -> glm::vec3 {
    glm::vec2 xz = glm::vec2(p.x, p.z);
    float strength = 32.0f - glm::clamp(glm::length(xz), -32.0f, 32.0f);
    return glm::vec3(-p.x, 0, -p.z) * strength * 0.5f;
    };

// Shockwave ring — displacement peaks at a specific radius
DispFunc shockwave = [](glm::vec3 p) -> glm::vec3 {
    float d = glm::length(p);
    float ring = glm::exp(-glm::pow(d - 1.5f, 2.0f) * 4.0f);
    return glm::normalize(p + glm::vec3(0.001f)) * ring * 0.6f;
    };

// Squish and stretch — compress Y, expand XZ
DispFunc squish = [](glm::vec3 p) -> glm::vec3 {
    return glm::vec3(p.x * 0.3f, -p.y * 0.4f, p.z * 0.3f);
    };

// Twisted spike — spiral outward from top
DispFunc spike = [](glm::vec3 p) -> glm::vec3 {
    float angle = p.y * 1.5f;
    float radius = glm::length(glm::vec2(p.x, p.z));
    return glm::vec3(
        glm::cos(angle) * radius * 0.3f,
        glm::pow(glm::max(p.y, 0.0f), 2.0f) * 0.2f,
        glm::sin(angle) * radius * 0.3f
    );
    };

// Breathing — pulsate along surface normal direction
DispFunc breathe = [](glm::vec3 p) -> glm::vec3 {
    float d = glm::length(p);
    float pulse = glm::sin(d * 3.0f - 1.0f) * 0.15f;
    return glm::normalize(p + glm::vec3(0.001f)) * pulse;
    };

DispFunc dispFunctionsArray[8] = {
    twist,
    ripple,
    curl,
    pinch,
    shockwave,
    squish,
    spike,
    breathe
};