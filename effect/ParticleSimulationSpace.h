#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class ParticleSimulationSpace : std::uint32_t {
	global = 0,
	local = 1
};

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleSimulationSpace, {
	{ ParticleSimulationSpace::global, "global" },
	{ ParticleSimulationSpace::local, "local" }
})
}
