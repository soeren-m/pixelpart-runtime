#pragma once

#include "../types/Types.h"
#include "../json/json.hpp"

namespace pixelpart {
class LevelOfDetail {
public:
	LevelOfDetail() = default;
	LevelOfDetail(float_t distance);

	void distanceThreshold(float_t distance);
	float_t distanceThreshold() const;

private:
	float_t lodDistanceThreshold = 0.0;
};

void to_json(nlohmann::ordered_json& j, const LevelOfDetail& lod);
void from_json(const nlohmann::ordered_json& j, LevelOfDetail& lod);
}
