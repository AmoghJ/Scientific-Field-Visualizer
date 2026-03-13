#include "OrbitCamera.h"

#include <iostream>

OrbitCamera::OrbitCamera(Container* cont) : Component(cont) { 

    setPerspective(45.0f, 1280.0f / 720.0f, 0.01f, 1000.0f);
    setTarget(glm::vec3(0.0f));    // orbit around world origin
    setDistance(5.0f);
    setDistanceLimits(0.5f, 200.0f);

    updateView();

    Subscribe<ViewerResizeEvent>([this](const ViewerResizeEvent& rs) {
        setPerspective(45.0f, (float)rs.width / (float)rs.height, 0.01f, 1000.0f);
        updateView();
    });

    Subscribe<LeftMouseDragEvent>([this](const LeftMouseDragEvent& mD) {
        orbit(mD.dX, mD.dY);
    });

    Subscribe<RightMouseDragEvent>([this](const RightMouseDragEvent& mD) {
       pan(mD.dX, mD.dY);
    });

    Subscribe<MouseScrollEvent>([this](const MouseScrollEvent& sc) {
        zoom(sc.scrollY);
    });
}
OrbitCamera::~OrbitCamera() {}

void OrbitCamera::reset() {
    m_target = glm::vec3(0.0f);
    m_distance = 5.0f;
    m_yaw = -90.0f;  // face -Z by default
    m_pitch = 20.0f;
    updateView();
}

// -----------------------------------------------------------------------
//  Projection parameters
// -----------------------------------------------------------------------
void OrbitCamera::setPerspective(float fovDeg, float aspect, float near, float far) {
    m_fov = fovDeg;
    m_aspect = aspect;
    m_near = near;
    m_far = far;
    updateProjection();
}

void OrbitCamera::setAspect(float aspect) {
    m_aspect = aspect;
    updateProjection();
}

// -----------------------------------------------------------------------
//  Orbit target & distance
// -----------------------------------------------------------------------
void OrbitCamera::setTarget(const glm::vec3& target) {
    m_target = target;
    updateView();
}

void OrbitCamera::setDistance(float d) {
    m_distance = glm::clamp(d, m_minDistance, m_maxDistance);
    updateView();
}

void OrbitCamera::setDistanceLimits(float minD, float maxD) {
    m_minDistance = minD;
    m_maxDistance = maxD;
    m_distance = glm::clamp(m_distance, m_minDistance, m_maxDistance);
}

void OrbitCamera::setPitchLimits(float minPitch, float maxPitch) {
    m_minPitch = minPitch;
    m_maxPitch = maxPitch;
    m_pitch = glm::clamp(m_pitch, m_minPitch, m_maxPitch);
}

// -----------------------------------------------------------------------
//  Input handlers
//  Call these from your GLFW/SDL callbacks.
//  deltaX / deltaY are in screen pixels; sensitivity scales them.
// -----------------------------------------------------------------------

// Left-mouse drag — rotates around target
void OrbitCamera::orbit(float deltaX, float deltaY) {
    m_yaw += deltaX * m_orbitSensitivity;
    m_pitch += deltaY * m_orbitSensitivity;   // subtract: drag up = pitch up
    m_pitch = glm::clamp(m_pitch, m_minPitch, m_maxPitch);
    updateView();
}

// Scroll wheel — moves camera closer / farther
void OrbitCamera::zoom(float delta) {
    // Exponential zoom feels natural across large distance ranges
    m_distance *= std::pow(m_zoomFactor, -delta);
    m_distance = glm::clamp(m_distance, m_minDistance, m_maxDistance);
    updateView();
}

// Middle-mouse drag — pans target in the camera's local XY plane
void OrbitCamera::pan(float deltaX, float deltaY) {
    // Scale pan by distance so it feels consistent at any zoom level
    float scale = m_distance * m_panSensitivity;
    m_target -= m_right * (deltaX * scale);
    m_target += m_up * (deltaY * scale);
    updateView();
}

// -----------------------------------------------------------------------
//  Sensitivity tweaks
// -----------------------------------------------------------------------
void OrbitCamera::setOrbitSensitivity(float s) { m_orbitSensitivity = s; }
void OrbitCamera::setZoomFactor(float f) { m_zoomFactor = f; }     // >1 = faster
void OrbitCamera::setPanSensitivity(float s) { m_panSensitivity = s; }

void OrbitCamera::smoothOrbitTo(float targetYaw, float targetPitch, float targetDist,
    float dt, float speed) {
    m_yaw = glm::mix(m_yaw, targetYaw, 1.0f - std::exp(-speed * dt));
    m_pitch = glm::mix(m_pitch, targetPitch, 1.0f - std::exp(-speed * dt));
    m_distance = glm::mix(m_distance, targetDist, 1.0f - std::exp(-speed * dt));
    m_pitch = glm::clamp(m_pitch, m_minPitch, m_maxPitch);
    m_distance = glm::clamp(m_distance, m_minDistance, m_maxDistance);
    updateView();
}

void OrbitCamera::updateView() {
    // Spherical -> Cartesian (Y-up)
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);

    // Unit vector from target to camera (the "look-from" direction)
    glm::vec3 offset{
        std::cos(pitchRad) * std::cos(yawRad),
        std::sin(pitchRad),
        std::cos(pitchRad) * std::sin(yawRad)
    };

    m_position = m_target + offset * m_distance;
    m_forward = -offset;                                        // camera -> target
    m_right = glm::normalize(glm::cross(m_forward, WORLD_UP));
    m_up = glm::normalize(glm::cross(m_right, m_forward));

    m_view = glm::lookAt(m_position, m_target, WORLD_UP);

    Notify<CameraMoveEvent>(CameraMoveEvent(m_position, viewProjection()));
}

void OrbitCamera::updateProjection() {
    m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
}

const float* OrbitCamera::viewPtr()        const { return glm::value_ptr(m_view); }
const float* OrbitCamera::projectionPtr()  const { return glm::value_ptr(m_projection); }
glm::mat4 OrbitCamera::viewProjection() const { return m_projection * m_view; }