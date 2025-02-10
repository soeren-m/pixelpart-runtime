#pragma once

#include "../common/Math.h"

namespace pixelpart {
class NodeTransform {
public:
	NodeTransform() = default;
	NodeTransform(const mat4_t& matrix);
	NodeTransform(const float3_t& position, const float3_t& orientation, const float3_t& size);

	float3_t position() const;
	float3_t orientation() const;
	float3_t size() const;

	const mat4_t& matrix() const;

private:
	mat4_t transformMatrix = mat4_t(1.0);
};

float4_t operator*(const NodeTransform& transform, const float4_t& vec);

NodeTransform operator*(const NodeTransform& lhs, const NodeTransform& rhs);

bool operator==(const NodeTransform& lhs, const NodeTransform& rhs);
bool operator!=(const NodeTransform& lhs, const NodeTransform& rhs);
}