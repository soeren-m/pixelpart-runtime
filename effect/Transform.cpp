#include "Transform.h"
#include "../math/Common.h"
#include "../math/Geometry.h"
#include "../math/Trigonometry.h"
#include "../math/Transformation.h"

namespace pixelpart {
Transform::Transform(const matrix4_t& matrix) : transformMatrix(matrix) {

}
Transform::Transform(const float3_t& position, const float3_t& rotation, const float3_t& scale) {
	matrix4_t scaleMatrix = math::scalingMatrix(scale);
	matrix4_t rotationMatrix = math::yawPitchRollRotationMatrix(math::radians(rotation.y), math::radians(rotation.z), math::radians(rotation.x));
	matrix4_t translationMatrix = math::translationMatrix(position);

	transformMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}

float3_t Transform::position() const {
	return float3_t(transformMatrix[3].x, transformMatrix[3].y, transformMatrix[3].z);
}
float3_t Transform::rotation() const {
	constexpr float_t epsilon = 0.000001;

	float_t sx = math::length(transformMatrix[0]);
	float_t sy = math::length(transformMatrix[1]);
	float_t sz = math::length(transformMatrix[2]);
	if(sx < epsilon || sy < epsilon || sz < epsilon) {
		sx = 1.0;
		sy = 1.0;
		sz = 1.0;
	}

	matrix4_t normalizedMatrix = transformMatrix;
	normalizedMatrix[0] = transformMatrix[0] / sx;
	normalizedMatrix[1] = transformMatrix[1] / sy;
	normalizedMatrix[2] = transformMatrix[2] / sz;

	float_t roll, yaw, pitch;
	math::extractYawPitchRoll(normalizedMatrix, yaw, pitch, roll);

	return math::degrees(float3_t(roll, yaw, pitch));
}
float3_t Transform::scale() const {
	return float3_t(
		math::length(float3_t(transformMatrix[0].x, transformMatrix[0].y, transformMatrix[0].z)),
		math::length(float3_t(transformMatrix[1].x, transformMatrix[1].y, transformMatrix[1].z)),
		math::length(float3_t(transformMatrix[2].x, transformMatrix[2].y, transformMatrix[2].z)));
}

const matrix4_t& Transform::matrix() const {
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
