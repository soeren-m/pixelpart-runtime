#include "Polyline.h"
#include "../math/Geometry.h"
#include <algorithm>

namespace pixelpart {
Polyline::Polyline(const std::vector<float3_t>& points) : polylinePoints(points) {
	refreshSegmentLengths();
}

float3_t Polyline::sample(float_t distance) const {
	const float_t epsilon = 0.000001;

	distance = std::max(distance, 0.0);

	for(std::size_t index = 0; index + 1 < polylinePoints.size(); index++) {
		float_t segmentStart = polylineCumulatedSegmentLengths[index];
		float_t segmentEnd = polylineCumulatedSegmentLengths[index + 1];

		if(distance < segmentEnd) {
			float_t segmentLength = segmentEnd - segmentStart;
			float_t t = (distance - segmentStart) / std::max(segmentLength, epsilon);

			return polylinePoints[index] + t * (polylinePoints[index + 1] - polylinePoints[index]);
		}
	}

	return !polylinePoints.empty() ? polylinePoints.back() : float3_t(0.0, 0.0, 0.0);
}

void Polyline::addPoint(const float3_t& point) {
	polylinePoints.push_back(point);
	refreshSegmentLengths();
}
void Polyline::insertPoint(std::size_t index, const float3_t& point) {
	polylinePoints.insert(polylinePoints.begin() + index, point);
	refreshSegmentLengths();
}
void Polyline::setPoint(std::size_t index, const float3_t& point) {
	polylinePoints.at(index) = point;
	refreshSegmentLengths();
}
void Polyline::removePoint(std::size_t index) {
	polylinePoints.erase(polylinePoints.begin() + index);
	refreshSegmentLengths();
}
void Polyline::removePoints() {
	polylinePoints.clear();
	refreshSegmentLengths();
}

const float3_t& Polyline::point(std::size_t index) const {
	return polylinePoints.at(index);
}
const std::vector<float3_t>& Polyline::points() const {
	return polylinePoints;
}

std::size_t Polyline::pointCount() const {
	return polylinePoints.size();
}

float_t Polyline::length() const {
	return !polylineCumulatedSegmentLengths.empty() ? polylineCumulatedSegmentLengths.back() : 0.0;
}

void Polyline::refreshSegmentLengths() {
	polylineCumulatedSegmentLengths.resize(polylinePoints.size());
	if(polylineCumulatedSegmentLengths.empty()) {
		return;
	}

	polylineCumulatedSegmentLengths[0] = 0.0;

	for(std::size_t index = 1; index < polylinePoints.size(); index++) {
		float_t segmentLength = math::distance(polylinePoints[index], polylinePoints[index - 1]);

		polylineCumulatedSegmentLengths[index] = polylineCumulatedSegmentLengths[index - 1] + segmentLength;
	}
}

void to_json(nlohmann::ordered_json& j, const Polyline& polyline) {
	j = nlohmann::ordered_json(polyline.points());
}
void from_json(const nlohmann::ordered_json& j, Polyline& polyline) {
	polyline = Polyline(j.get<std::vector<float3_t>>());
}
}
