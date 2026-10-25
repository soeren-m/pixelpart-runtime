#include "ParticleAlignment.h"
#include "../effect/Coordinates.h"
#include "../math/Geometry.h"
#include "../math/MatrixCommon.h"
#include "../math/Transformation.h"

namespace pixelpart {
matrix3_t lookAtMatrix2d(float2_t direction) {
	float2_t front = float2_t(0.0, 1.0);

	float_t len = math::length(direction);
	if(len > 0.001) {
		front = direction / len;
	}

	return matrix3_t(
		float3_t(front.y, -front.x, 0.0),
		float3_t(front.x, front.y, 0.0),
		float3_t(0.0, 0.0, 1.0));
}

matrix3_t cameraAlignmentMatrix(matrix4_t invViewMatrix) {
	float3_t right = math::normalize(float3_t(invViewMatrix[0]));
	float3_t up = math::normalize(float3_t(invViewMatrix[1]));
	float3_t forward = math::normalize(float3_t(invViewMatrix[2]));

	matrix3_t billboardMatrix = matrix3_t(1.0);
	billboardMatrix[0] = right;
	billboardMatrix[1] = up;
	billboardMatrix[2] = forward;

	return billboardMatrix;
}
matrix3_t motionAlignmentMatrix(float3_t direction) {
	return math::transpose(math::lookAtMatrix(direction, worldUpVector3)); // TODO: transpose?
}
matrix3_t motionAlignmentMatrix2d(float3_t direction) {
	return lookAtMatrix2d(float2_t(direction));
}
matrix3_t emissionAlignmentMatrix(float3_t particlePosition, matrix4_t emitterTransform) {
	return math::transpose(math::lookAtMatrix(math::extractTranslation(emitterTransform) - particlePosition, worldUpVector3));
}
matrix3_t emissionAlignmentMatrix2d(float3_t particlePosition, matrix4_t emitterTransform) {
	return lookAtMatrix2d(float2_t(math::extractTranslation(emitterTransform) - particlePosition));
}
matrix3_t emitterAlignmentMatrix(matrix4_t emitterTransform) {
	return matrix3_t(emitterTransform);
}
}
