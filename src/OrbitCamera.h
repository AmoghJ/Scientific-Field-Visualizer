#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Component.h"
class Container;

#include "Events.h"

// ---------------------------------------------------------------------------
//  OrbitCamera
//
//  Spherical-coordinate camera that orbits a target point.
//  Coordinate convention: Y-up, right-handed (standard OpenGL).
//
//  Usage:
//    OrbitCamera cam;
//    cam.setTarget(glm::vec3(0, 0, 0));
//
//    // In your input callbacks:
//    cam.orbit(deltaX, deltaY);   // mouse drag
//    cam.zoom(scrollDelta);        // scroll wheel
//    cam.pan(deltaX, deltaY);      // middle-mouse drag
//
//    // In your render loop:
//    glm::mat4 mvp = cam.projection() * cam.view() * modelMatrix;
//    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
// ---------------------------------------------------------------------------

class OrbitCamera : public Component {

private:

    // -----------------------------------------------------------------------
    //  State
    // -----------------------------------------------------------------------

    // Spherical coords (degrees, Y-up)
    float m_yaw = -90.0f;
    float m_pitch = 20.0f;
    float m_distance = 5.0f;

    // Limits
    float m_minPitch = -89.0f;
    float m_maxPitch = 89.0f;
    float m_minDistance = 0.1f;
    float m_maxDistance = 1000.0f;

    // Projection
    float m_fov = 45.0f;
    float m_aspect = 16.0f / 9.0f;
    float m_near = 0.01f;
    float m_far = 1000.0f;

    // Sensitivity
    float m_orbitSensitivity = 0.3f;  // deg per pixel
    float m_zoomFactor = 1.1f;  // distance multiplier per scroll tick
    float m_panSensitivity = 0.001f;

    // Target
    glm::vec3 m_target{ 0.0f, 0.0f, 0.0f };

    // Derived (cached, recomputed in updateView)
    glm::vec3 m_position{ 0.0f, 0.0f, 5.0f };
    glm::vec3 m_forward{ 0.0f, 0.0f,-1.0f };
    glm::vec3 m_right{ 1.0f, 0.0f, 0.0f };
    glm::vec3 m_up{ 0.0f, 1.0f, 0.0f };

    glm::mat4 m_view{ 1.0f };
    glm::mat4 m_projection{ 1.0f };

    static constexpr glm::vec3 WORLD_UP{ 0.0f, 1.0f, 0.0f };

    // -----------------------------------------------------------------------
    //  Internal recompute
    // -----------------------------------------------------------------------
    void updateView();

    void updateProjection();

public:
    // -----------------------------------------------------------------------
    //  Construction / reset
    // -----------------------------------------------------------------------
    OrbitCamera(Container* cont);
    ~OrbitCamera();

    // Reset to default state
    void reset();

    // -----------------------------------------------------------------------
    //  Projection parameters
    // -----------------------------------------------------------------------
    void setPerspective(float fovDeg, float aspect, float near, float far);

    void setAspect(float aspect);

    // -----------------------------------------------------------------------
    //  Orbit target & distance
    // -----------------------------------------------------------------------
    void setTarget(const glm::vec3& target);

    void setDistance(float d);

    void setDistanceLimits(float minD, float maxD);

    void setPitchLimits(float minPitch, float maxPitch);

    // -----------------------------------------------------------------------
    //  Input handlers
    //  Call these from your GLFW/SDL callbacks.
    //  deltaX / deltaY are in screen pixels; sensitivity scales them.
    // -----------------------------------------------------------------------

    // Left-mouse drag — rotates around target
    void orbit(float deltaX, float deltaY);

    // Scroll wheel — moves camera closer / farther
    void zoom(float delta);

    // Middle-mouse drag — pans target in the camera's local XY plane
    void pan(float deltaX, float deltaY);

    // -----------------------------------------------------------------------
    //  Sensitivity tweaks
    // -----------------------------------------------------------------------
    void setOrbitSensitivity(float s);
    void setZoomFactor(float f);     // >1 = faster
    void setPanSensitivity(float s);

    // -----------------------------------------------------------------------
    //  Matrix accessors
    // -----------------------------------------------------------------------
    const glm::mat4& view()       const { return m_view; }
    const glm::mat4& projection() const { return m_projection; }

    // Combined VP — multiply by your model matrix on the CPU, or pass
    // separately to the shader so you can do lighting in world space.
    glm::mat4 viewProjection() const;

    // Raw pointer for glUniformMatrix4fv
    const float* viewPtr() const;
    const float* projectionPtr() const;

    // -----------------------------------------------------------------------
    //  Animate to a new orientation (call each frame with dt in seconds)
    //  Useful for smooth auto-rotation or "focus on object" transitions.
    // -----------------------------------------------------------------------
    void smoothOrbitTo(float targetYaw, float targetPitch, float targetDist,
        float dt, float speed = 5.0f);
};