/*
 *
 * Andrew Frost
 * camera.cpp
 * 2020
 *
 */

#include "camera.hpp"

namespace tools {

//-------------------------------------------------------------------------
// Math Functions
//
template <typename T>
bool isZero(const T& _a)
{
    return fabs(_a) < std::numeric_limits<T>::epsilon();
}

template <typename T>
bool isOne(const T& _a)
{
    return areEqual(_a, (T)1);
}

inline float sign(float s)
{
    return (s < 0.f) ? -1.f : 1.f;
}

///////////////////////////////////////////////////////////////////////////
// Camera                                                                //
///////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
// Constructor
//
Camera::Camera()
{
    update();
}

//-------------------------------------------------------------------------
// update internal transformation matrix
//
void Camera::update()
{
    m_matrix = glm::lookAt(m_pos, m_int, m_up);

    if (!isZero(m_roll)) {
        glm::mat4 rotate = glm::rotate(m_roll, glm::vec3(0, 0, 1));
        m_matrix = m_matrix * rotate;
    }
}

//-------------------------------------------------------------------------
// Set camera information and derive viewing matrix
//
void Camera::setLookAt(glm::vec3& eye, glm::vec3& center, glm::vec3& up)
{
    m_pos = eye;
    m_int = center;
    m_up  = up;
    update();
}

//--------------------------------------------------------------------------------------------------
// Retreive current camera information
// Position, interest and up vector
//
void Camera::getLookAt(glm::vec3& eye, glm::vec3& center, glm::vec3& up) const
{
    eye    = m_pos;
    center = m_int;
    up     = m_up;
}

//-------------------------------------------------------------------------
// Retreive transformation matrix of camera
//
const glm::mat4& Camera::getMatrix() const
{
    return m_matrix;
}


} // ! namespace tools