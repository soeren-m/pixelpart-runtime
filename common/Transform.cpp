#include "Transform.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "../glm/gtx/transform.hpp"
#include "../glm/gtx/euler_angles.hpp"

namespace pixelpart {
Transform::Transform(const mat4_t& matrix) : transformMatrix(matrix) {

}
Transform::Transform(const float3_t& position, const float3_t& rotation, const float3_t& scale) {
	mat4_t scaleMatrix = glm::scale(scale);
	mat4_t rotationMatrix = glm::yawPitchRoll(glm::radians(rotation.y), glm::radians(rotation.z), glm::radians(rotation.x));
	mat4_t translationMatrix = glm::translate(position);

	transformMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}

float3_t Transform::position() const {
	return float3_t(transformMatrix[3]);
}
float3_t Transform::rotation() const {
	mat4_t normalizedMatrix = transformMatrix;
	normalizedMatrix[0] = glm::normalize(transformMatrix[0]);
	normalizedMatrix[1] = glm::normalize(transformMatrix[1]);
	normalizedMatrix[2] = glm::normalize(transformMatrix[2]);

	float_t rx, ry, rz;
	glm::extractEulerAngleYXZ(normalizedMatrix, ry, rz, rx);

	return glm::degrees(float3_t(rx, ry, rz));
}
float3_t Transform::scale() const {
	return float3_t(
		glm::length(float3_t(transformMatrix[0])),
		glm::length(float3_t(transformMatrix[1])),
		glm::length(float3_t(transformMatrix[2])));
}

const mat4_t& Transform::matrix() const {
	return transformMatrix;
}

float4_t operator*(const Transform& transform, const float4_t& vec) {
	return transform.matrix() * vec;
}

Transform operator*(const Transform& lhs, const Transform& rhs) {
	return Transform(lhs.matrix() * rhs.matrix());
}

bool operator==(const Transform& lhs, const Transform& rhs) {
	return lhs.matrix() == rhs.matrix();
}
bool operator!=(const Transform& lhs, const Transform& rhs) {
	return lhs.matrix() != rhs.matrix();
}
}