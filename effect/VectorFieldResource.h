#pragma once

#include "Resource.h"
#include "../common/Math.h"
#include "../common/Grid3d.h"
#include "../json/json.hpp"

namespace pixelpart {
class VectorFieldResource : public Resource {
public:
	VectorFieldResource() = default;
	VectorFieldResource(const std::string& name, const Grid3d<float3_t>& field);

	Grid3d<float3_t>& field();
	const Grid3d<float3_t>& field() const;

private:
	Grid3d<float3_t> fieldData;
};

void to_json(nlohmann::ordered_json& j, const VectorFieldResource& resource);
void from_json(const nlohmann::ordered_json& j, VectorFieldResource& resource);
}