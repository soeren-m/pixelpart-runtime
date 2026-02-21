#pragma once

#include "Types.h"

namespace pixelpart {
class Transform {
public:
	Transform() = default;
	Transform(const matrix4_t& matrix);
	Transform(const float3_t& position, const float3_t& rotation, const float3_t& scale);

	float3_t position() const;
	float3_t rotation() const;
	float3_t scale() const;

	const matrix4_t& matrix() const;

private:
	matrix4_t transformMatrix = matrix4_t(1.0);
};

float4_t operator*(const Transform& transform, const float4_t& vec);

Transform operator*(const Transform& lhs, const Transform& rhs);

bool operator==(const Transform& lhs, const Transform& rhs);
bool operator!=(const Transform& lhs, const Transform& rhs);
}
