#pragma once

#include "../effect/AlignmentMode.h"
#include "../types/Types.h"

namespace pixelpart {
matrix3_t cameraAlignmentMatrix(matrix4_t invViewMatrix);
matrix3_t motionAlignmentMatrix(float3_t direction);
matrix3_t motionAlignmentMatrix2d(float3_t direction);
matrix3_t emissionAlignmentMatrix(float3_t particlePosition, matrix4_t emitterTransform);
matrix3_t emissionAlignmentMatrix2d(float3_t particlePosition, matrix4_t emitterTransform);
matrix3_t emitterAlignmentMatrix(matrix4_t emitterTransform);
}
