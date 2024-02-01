#include "Camera.h"
#include <iostream>
#include <algorithm>

using namespace linalg;

void Camera::MoveTo(const vec3f& position) noexcept
{
	m_position = position;
}

void Camera::Move(const vec3f& direction) noexcept
{
	vec3f newDir;
	newDir = direction * WorldToViewMatrix().get_3x3();
	newDir.y = 0;

	//std::cout << "New dir without normalize: " << std::endl;
	//std::cout << newDir << std::endl;
	//std::cout << "New dir with normalize: " << std::endl;
	//std::cout << newDir.normalize() << std::endl;
	m_position += newDir;
}

void Camera::Rotate(long x, long y) noexcept
{
	yaw += x * sensitivity;
	pitch += y * sensitivity;

	pitch = clamp(pitch, clampMouseMin, clampMouseMax);

	m_rotation = mat4f::rotation(0, -yaw, -pitch);
}


mat4f Camera::WorldToViewMatrix() const noexcept
{
	// Assuming a camera's position and rotation is defined by matrices T(p) and R,
	// the View-to-World transform is T(p)*R (for a first-person style camera).
	//
	// World-to-View then is the inverse of T(p)*R;
	//		inverse(T(p)*R) = inverse(R)*inverse(T(p)) = transpose(R)*T(-p)
	// Since now there is no rotation, this matrix is simply T(-p)

	mat4f rotation = m_rotation;
	rotation.transpose();
	return rotation * mat4f::translation(-m_position);
}

mat4f Camera::ProjectionMatrix() const noexcept
{
	return mat4f::projection(m_vertical_fov, m_aspect_ratio, m_near_plane, m_far_plane);
}