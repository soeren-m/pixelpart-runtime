#pragma once

#include "Resource.h"
#include "../shader/ShaderGraph.h"

namespace pixelpart {
struct MaterialResource : public Resource {
	ShaderGraph shaderGraph;
};

void to_json(nlohmann::ordered_json& j, const MaterialResource& resource);
void from_json(const nlohmann::ordered_json& j, MaterialResource& resource);
}