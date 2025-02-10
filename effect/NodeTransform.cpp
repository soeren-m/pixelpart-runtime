#include "NodeTransform.h"
#include "../glm/gtx/transform.hpp"
#include "../glm/gtx/euler_angles.hpp"

namespace pixelpart {
NodeTransform::NodeTransform(const mat4_t& matrix) : transformMatrix(matrix) {

}
NodeTransform::NodeTransform(const float3_t& position, const float3_t& orientation, const float3_t& size) {
	mat4_t scaleMatrix = glm::scale(size);
	mat4_t rotationMatrix = glm::yawPitchRoll(glm::radians(orientation.y), glm::radians(orientation.z), glm::radians(orientation.x));
	mat4_t translationMatrix = glm::translate(position);

	transformMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}

float3_t NodeTransform::position() const {
	return float3_t(transformMatrix[3]);
}
float3_t NodeTransform::orientation() const {
	mat4_t normalizedMatrix = transformMatrix;
	normalizedMatrix[0] = glm::normalize(transformMatrix[0]);
	normalizedMatrix[1] = glm::normalize(transformMatrix[1]);
	normalizedMatrix[2] = glm::normalize(transformMatrix[2]);

	float_t rx, ry, rz;
	glm::extractEulerAngleYXZ(normalizedMatrix, ry, rz, rx);

	return glm::degrees(float3_t(rx, ry, rz));
}
float3_t NodeTransform::size() const {
	return float3_t(
		glm::length(float3_t(transformMatrix[0])),
		glm::length(float3_t(transformMatrix[1])),
		glm::length(float3_t(transformMatrix[2])));
}

const mat4_t& NodeTransform::matrix() const {
	return transformMatrix;
}

float4_t operator*(const NodeTransform& transform, const float4_t& vec) {
	return transform.matrix() * vec;
}

NodeTransform operator*(const NodeTransform& lhs, const NodeTransform& rhs) {
	return NodeTransform(lhs.matrix() * rhs.matrix());
}

bool operator==(const NodeTransform& lhs, const NodeTransform& rhs) {
	return lhs.matrix() == rhs.matrix();
}
bool operator!=(const NodeTransform& lhs, const NodeTransform& rhs) {
	return lhs.matrix() != rhs.matrix();
}
}