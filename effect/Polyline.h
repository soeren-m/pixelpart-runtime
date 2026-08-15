#pragma once

#include "../types/Types.h"
#include <vector>

namespace pixelpart {
class Polyline {
public:
	Polyline() = default;
	Polyline(const std::vector<float3_t>& points);

	float3_t sample(float_t distance) const;

	void addPoint(const float3_t& point);
	void insertPoint(std::size_t index, const float3_t& point);
	void setPoint(std::size_t index, const float3_t& point);
	void removePoint(std::size_t index);
	void removePoints();

	const float3_t& point(std::size_t index) const;
	const std::vector<float3_t>& points() const;

	std::size_t pointCount() const;

	float_t length() const;

private:
	void refreshSegmentLengths();

	std::vector<float3_t> polylinePoints;
	std::vector<float_t> polylineCumulatedSegmentLengths;
};

void to_json(nlohmann::ordered_json& j, const Polyline& polyline);
void from_json(const nlohmann::ordered_json& j, Polyline& polyline);
}
