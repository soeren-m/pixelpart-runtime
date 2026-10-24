#pragma once

#include "../effect/AlignmentMode.h"
#include "../types/Types"

// TODO

namespace pixelpart {
matrix3_t cameraAlignmentMatrix(matrix4_t invViewMatrix) {
	float3_t right = math::normalize(invViewMatrix[0].xyz);
	float3_t up = math::normalize(invViewMatrix[1].xyz);
	float3_t forward = math::normalize(invViewMatrix[2].xyz);

	matrix3_t billboardMatrix = matrix3_t(1.0);
	billboardMatrix[0] = right;
	billboardMatrix[1] = up;
	billboardMatrix[2] = forward;

	return billboardMatrix;
}
matrix3_t motionAlignmentMatrix(float3_t direction)  {
	return lookAt(direction);
}
matrix3_t motionAlignmentMatrix2d(float3_t direction) {
	return lookAt2d(direction.xy);
}
matrix3_t emissionAlignmentMatrix(float3_t particlePosition, matrix4_t emitterTransform) {
	return lookAt(extractTranslation(emitterTransform) - particlePosition);
}
matrix3_t emissionAlignmentMatrix2d(float3_t particlePosition, matrix4_t emitterTransform) {
	return lookAt2d(vec2(extractTranslation(emitterTransform) - particlePosition));
}
matrix3_t emitterAlignmentMatrix(matrix4_t emitterTransform) {
	return matrix3_t(emitterTransform);
}
}
