/*
 *
 * Andrew Frost
 * camera.hpp
 * 2020
 *
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace tools {

///////////////////////////////////////////////////////////////////////////
// Camera                                                                //
///////////////////////////////////////////////////////////////////////////

class Camera
{
public:
    Camera();

    ~Camera() = default;

    void update();

    void setLookAt(glm::vec3& eye, glm::vec3& center, glm::vec3& up);

    void getLookAt(glm::vec3& eye, glm::vec3& center, glm::vec3& up) const;

    const glm::mat4& getMatrix() const;

private:
    // Camera Position
    glm::vec3 m_pos    = glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 m_int    = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 m_up     = glm::vec3(0.f, 1.f, 0.f);
    glm::mat4 m_matrix = glm::mat4(1.f);
    float     m_roll   = 0.f; // Rotation around Z axis

    // Screen 
    int m_width  = 1;
    int m_height = 1;

}; // ! class Camera

} // ! namespace tools