#pragma once

#include "../common/Math.h"

namespace pixelpart {
class NodeTransform {
public:
	NodeTransform() = default;

	float3_t position() const;
	float3_t orientation() const;
	float3_t size() const;

private:
	mat4_t matrix;
};
}