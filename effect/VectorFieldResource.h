#pragma once

#include "Resource.h"
#include "../common/Grid3d.h"

namespace pixelpart {
struct VectorFieldResource : public Resource {
	Grid3d<vec3_t> field;
};

void to_json(nlohmann::ordered_json& j, const VectorFieldResource& resource);
void from_json(const nlohmann::ordered_json& j, VectorFieldResource& resource);
}