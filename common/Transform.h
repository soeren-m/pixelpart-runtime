#pragma once

#include "../common/Math.h"

namespace pixelpart {
class Transform {
public:
	Transform() = default;
	Transform(const mat4_t& matrix);
	Transform(const float3_t& position, const float3_t& orientation, const float3_t& size);

	float3_t position() const;
	float3_t orientation() const;
	float3_t size() const;

	const mat4_t& matrix() const;

private:
	mat4_t transformMatrix = mat4_t(1.0);
};

float4_t operator*(const Transform& transform, const float4_t& vec);

Transform operator*(const Transform& lhs, const Transform& rhs);

bool operator==(const Transform& lhs, const Transform& rhs);
bool operator!=(const Transform& lhs, const Transform& rhs);
}