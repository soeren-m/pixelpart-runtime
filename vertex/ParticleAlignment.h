#pragma once

#include "../types/Types.h"

namespace pixelpart {
matrix3_t cameraAlignmentMatrix(const float3_t& cameraForward, const float3_t& cameraRight, const float3_t& cameraUp);
matrix3_t motionAlignmentMatrix(const float3_t& direction);
matrix3_t motionAlignmentMatrix2d(const float3_t& direction);
matrix3_t emissionAlignmentMatrix(const float3_t& particlePosition, const matrix4_t& emitterTransform);
matrix3_t emissionAlignmentMatrix2d(const float3_t& particlePosition, const matrix4_t& emitterTransform);
matrix3_t emitterAlignmentMatrix(const matrix4_t& emitterTransform);
}
