#include "ParticleAlignment.h"
#include "../effect/Coordinates.h"
#include "../math/Geometry.h"
#include "../math/MatrixCommon.h"
#include "../math/Transformation.h"

namespace pixelpart {
matrix3_t lookAtMatrix2d(const float2_t& direction) {
	float2_t front(0.0, 1.0);

	float_t len = math::length(direction);
	if(len > 1e-6) {
		front = direction / len;
	}

	return matrix3_t(
		float3_t(front.y, -front.x, 0.0),
		float3_t(front.x, front.y, 0.0),
		float3_t(0.0, 0.0, 1.0));
}

matrix3_t cameraAlignmentMatrix(const float3_t& cameraForward, const float3_t& cameraRight, const float3_t& cameraUp) {
	return matrix3_t(cameraRight, cameraUp, cameraForward);
}
matrix3_t motionAlignmentMatrix(const float3_t& direction) {
	return math::transpose(math::lookAtMatrix(direction, worldUpVector3));
}
matrix3_t motionAlignmentMatrix2d(const float3_t& direction) {
	return lookAtMatrix2d(float2_t(direction));
}
matrix3_t emissionAlignmentMatrix(const float3_t& particlePosition, const matrix4_t& emitterTransform) {
	return math::transpose(math::lookAtMatrix(math::extractTranslation(emitterTransform) - particlePosition, worldUpVector3));
}
matrix3_t emissionAlignmentMatrix2d(const float3_t& particlePosition, const matrix4_t& emitterTransform) {
	return lookAtMatrix2d(float2_t(math::extractTranslation(emitterTransform) - particlePosition));
}
matrix3_t emitterAlignmentMatrix(const matrix4_t& emitterTransform) {
	return matrix3_t(emitterTransform);
}
}
