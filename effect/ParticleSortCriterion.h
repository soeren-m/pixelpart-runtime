#pragma once

#include <cstdint>
#include "../json/json.hpp"

namespace pixelpart {
enum class ParticleSortCriterion : uint32_t {
	none = 0,
	age = 1,
	distance = 2
};

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleSortCriterion, {
	{ ParticleSortCriterion::none, "none" },
	{ ParticleSortCriterion::age, "age" },
	{ ParticleSortCriterion::distance, "distance" }
})
}