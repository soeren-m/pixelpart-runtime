#include "LevelOfDetail.h"

namespace pixelpart {
LevelOfDetail::LevelOfDetail(float_t distance) : lodDistanceThreshold(distance) {

}

void LevelOfDetail::distanceThreshold(float_t distance) {
	lodDistanceThreshold = distance;
}
float_t LevelOfDetail::distanceThreshold() const {
	return lodDistanceThreshold;
}

void to_json(nlohmann::ordered_json& j, const LevelOfDetail& lod) {
	j = nlohmann::ordered_json{
		{ "distance_threshold", lod.distanceThreshold() }
	};
}
void from_json(const nlohmann::ordered_json& j, LevelOfDetail& lod) {
	lod = LevelOfDetail(j.value("distance_threshold", 0.0));
}
}
