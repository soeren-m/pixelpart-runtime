#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class NodeType {
	group,
	particle_emitter,
	force_field,
	collider,
	light_source
};

NLOHMANN_JSON_SERIALIZE_ENUM(NodeType, {
	{ NodeType::group, "group" },
	{ NodeType::particle_emitter, "particle_emitter" },
	{ NodeType::force_field, "force_field" },
	{ NodeType::collider, "collider" },
	{ NodeType::light_source, "light_source" }
})
}
