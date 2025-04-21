#pragma once

#include "Types.h"
#include "Math.h"
#include "../json/json.hpp"
#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>

namespace pixelpart {
enum class CurveInterpolation : uint32_t {
	none = 0,
	linear = 1,
	spline = 2,
	bezier = 3
};

template <typename T>
class Curve {
public:
	struct Point {
		float_t position;
		T value;
	};

	static constexpr std::size_t minCacheSize = 256;
	static constexpr std::size_t maxCacheSize = 1024;
	static constexpr std::size_t invalidIndex = 0xFFFFFFFF;

	Curve() {
		valueCache.resize(minCacheSize);
	}
	Curve(CurveInterpolation interpolation) : curveInterpolation(interpolation) {
		valueCache.resize(minCacheSize);
	}
	Curve(float_t initialPosition, const T& initialValue, CurveInterpolation interpolation = CurveInterpolation::linear) : Curve(interpolation) {
		addPoint(initialPosition, initialValue);
	}
	Curve(const T& initialValue, CurveInterpolation interpolation = CurveInterpolation::linear) : Curve(interpolation) {
		addPoint(0.5, initialValue);
	}
	Curve(const std::vector<Point>& pointList, CurveInterpolation interpolation = CurveInterpolation::linear) : Curve(interpolation) {
		points(pointList);
	}
	Curve(const std::vector<Point>& pointList, const std::vector<T>& controlPointList, CurveInterpolation interpolation = CurveInterpolation::linear) : Curve(interpolation) {
		curvePoints = pointList;
		curveControlPoints = controlPointList;
		curveControlPoints.resize(curvePoints.size() * 2, T(0));

		updateCache();
	}

	T operator()(float_t position = 0.0) const {
		return at(position);
	}

	T at(float_t position = 0.0) const {
		position = std::min(std::max(position, 0.0), 1.0);

		return valueCache[
			static_cast<std::size_t>(position * static_cast<float_t>(valueCache.size() - 1))];
	}

	void points(const std::vector<Point>& pointList) {
		curvePoints = pointList;
		curveControlPoints.clear();
		curveControlPoints.resize(curvePoints.size() * 2, T(0));

		updateCache();
	}
	void points(const Point* pointList, std::size_t count) {
		curvePoints.clear();
		curveControlPoints.clear();
		if(pointList != nullptr && count > 0) {
			curvePoints = std::vector<Point>(pointList, pointList + count);
			curveControlPoints.resize(curvePoints.size() * 2, T(0));
		}

		updateCache();
	}

	void addPoint(float_t position, const T& value) {
		position = std::min(std::max(position, 0.0), 1.0);

		curvePoints.push_back(Point{ position, value });
		curveControlPoints.push_back(T(0));
		curveControlPoints.push_back(T(0));

		updateCache();
	}
	void removePoint(std::size_t index) {
		if(index >= curvePoints.size()) {
			return;
		}

		curvePoints.erase(curvePoints.begin() + index);
		curveControlPoints.erase(curveControlPoints.begin() + index * 2);
		curveControlPoints.erase(curveControlPoints.begin() + index * 2);

		updateCache();
	}
	void setPointValue(std::size_t index, const T& value) {
		if(index >= curvePoints.size()) {
			return;
		}

		curvePoints[index].value = value;
		updateCache();
	}
	void setPointPosition(std::size_t index, float_t position) {
		if(index >= curvePoints.size()) {
			return;
		}

		curvePoints[index].position = std::min(std::max(position, 0.0), 1.0);
		updateCache();
	}
	void setControlPointValue(std::size_t index, const T& value) {
		if(index >= curveControlPoints.size()) {
			return;
		}

		curveControlPoints[index] = value;
		updateCache();
	}
	void clear() {
		curvePoints.clear();
		curveControlPoints.clear();

		updateCache();
	}

	bool empty() const {
		return curvePoints.empty();
	}

	std::size_t pointCount() const {
		return curvePoints.size();
	}
	const Point& point(std::size_t index) const {
		return curvePoints.at(index);
	}
	const std::vector<Point>& points() const {
		return curvePoints;
	}

	std::size_t controlPointCount() const {
		return curveControlPoints.size();
	}
	const T& controlPoint(std::size_t index) const {
		return curveControlPoints.at(index);
	}
	const std::vector<T>& controlPoints() const {
		return curveControlPoints;
	}

	ptrdiff_t pointIndex(float_t position, float_t epsilon = 0.001) const {
		if(curvePoints.empty()) {
			return -1;
		}

		typename std::vector<Point>::const_iterator it = std::min_element(curvePoints.begin(), curvePoints.end(), [position](const Point& p1, const Point& p2) {
			return std::abs(p1.position - position) < std::abs(p2.position - position);
		});

		if(std::abs(it->position - position) > epsilon) {
			return -1;
		}

		return it - curvePoints.begin();
	}

	template <typename UnaryFunc>
	void forEachPoint(UnaryFunc func) {
		for(auto it = curvePoints.begin(); it != curvePoints.end(); it++) {
			func(*it);
		}

		updateCache();
	}

	void interpolation(CurveInterpolation method) {
		if(method == curveInterpolation) {
			return;
		}

		curveInterpolation = method;
		updateCache();
	}
	CurveInterpolation interpolation() const {
		return curveInterpolation;
	}

	void enableAdaptiveCache() {
		adaptiveCache = true;
		updateCache();
	}
	void enableFixedCache(std::size_t size) {
		adaptiveCache = false;
		valueCache.resize(std::max(size, static_cast<std::size_t>(1)));
		updateCache();
	}
	const std::vector<T>& cache() const {
		return valueCache;
	}

private:
	void updateCache() {
		std::vector<std::size_t> sortIndices(curvePoints.size());
		std::iota(sortIndices.begin(), sortIndices.end(), 0);
		std::stable_sort(sortIndices.begin(), sortIndices.end(), [this](std::size_t i1, std::size_t i2) {
			return curvePoints[i1].position < curvePoints[i2].position;
		});

		std::vector<Point> updatedPoints(curvePoints.size());
		std::vector<T> updatedControlPoints(curveControlPoints.size());
		for(std::size_t index = 0; index < sortIndices.size(); index++) {
			std::size_t sortIndex = sortIndices[index];

			updatedPoints[index] = curvePoints[sortIndex];
			updatedControlPoints[index * 2 + 0] = curveControlPoints[sortIndex * 2 + 0];
			updatedControlPoints[index * 2 + 1] = curveControlPoints[sortIndex * 2 + 1];
		}

		curvePoints = std::move(updatedPoints);
		curveControlPoints = std::move(updatedControlPoints);

		if(adaptiveCache) {
			if(valueCache.size() < maxCacheSize && curvePoints.size() > valueCache.size() / 16) {
				valueCache.resize(valueCache.size() * 2);
			}
			else if(valueCache.size() > minCacheSize && curvePoints.size() < valueCache.size() / 32) {
				valueCache.resize(valueCache.size() / 2);
			}
		}

		if(curvePoints.empty()) {
			std::fill(valueCache.begin(), valueCache.end(), T(0));
			return;
		}
		else if(curvePoints.size() == 1) {
			std::fill(valueCache.begin(), valueCache.end(), curvePoints.back().value);
			return;
		}

		switch(curveInterpolation) {
			case CurveInterpolation::none:
				fillCacheNone();
				break;
			case CurveInterpolation::linear:
				fillCacheLinear();
				break;
			case CurveInterpolation::spline:
				fillCacheSpline();
				break;
			case CurveInterpolation::bezier:
				fillCacheBezier();
				break;
			default:
				break;
		}
	}

	void fillCacheNone() {
		for(std::size_t cacheIndex = 0, pointStartIndex = 0; cacheIndex < valueCache.size(); cacheIndex++) {
			float_t position = static_cast<float_t>(cacheIndex) / static_cast<float_t>(valueCache.size() - 1);
			std::size_t pointIndex = findIndex(position, pointStartIndex);

			if(pointIndex == invalidIndex) {
				valueCache[cacheIndex] = curvePoints.front().value;
				continue;
			}
			else if(pointIndex + 1 == curvePoints.size()) {
				valueCache[cacheIndex] = curvePoints.back().value;
				continue;
			}

			const Point& p1 = curvePoints[pointIndex];
			const Point& p2 = curvePoints[pointIndex + 1];

			valueCache[cacheIndex] = std::abs(position - p1.position) < std::abs(position - p2.position)
				? p1.value
				: p2.value;

			pointStartIndex = pointIndex;
		}
	}

	void fillCacheLinear() {
		for(std::size_t cacheIndex = 0, pointStartIndex = 0; cacheIndex < valueCache.size(); cacheIndex++) {
			float_t position = static_cast<float_t>(cacheIndex) / static_cast<float_t>(valueCache.size() - 1);
			std::size_t pointIndex = findIndex(position, pointStartIndex);

			if(pointIndex == invalidIndex) {
				valueCache[cacheIndex] = curvePoints.front().value;
				continue;
			}
			else if(pointIndex + 1 == curvePoints.size()) {
				valueCache[cacheIndex] = curvePoints.back().value;
				continue;
			}

			const Point& p1 = curvePoints[pointIndex];
			const Point& p2 = curvePoints[pointIndex + 1];
			float_t t = (position - p1.position) / (p2.position - p1.position);

			valueCache[cacheIndex] = t * p2.value + (1.0 - t) * p1.value;

			pointStartIndex = pointIndex;
		}
	}

	void fillCacheSpline() {
		const float_t alpha = 0.5;
		const float_t tension = 0.0;
		const float_t epsilon = 0.001;

		for(std::size_t cacheIndex = 0, pointStartIndex = 0; cacheIndex < valueCache.size(); cacheIndex++) {
			float_t position = static_cast<float_t>(cacheIndex) / static_cast<float_t>(valueCache.size() - 1);
			std::size_t pointIndex = findIndex(position, pointStartIndex);

			if(pointIndex == invalidIndex) {
				valueCache[cacheIndex] = curvePoints.front().value;
				continue;
			}
			else if(pointIndex + 1 == curvePoints.size()) {
				valueCache[cacheIndex] = curvePoints.back().value;
				continue;
			}

			const Point& p0 = curvePoints[pointIndex > 0 ? pointIndex - 1 : pointIndex];
			const Point& p1 = curvePoints[pointIndex];
			const Point& p2 = curvePoints[pointIndex + 1];
			const Point& p3 = curvePoints[pointIndex + 2 < curvePoints.size() ? pointIndex + 2 : pointIndex + 1];

			float_t t0 = 0.0;
			float_t t1 = t0 + std::pow(std::max(glm::distance(p0.value, p1.value), epsilon), alpha);
			float_t t2 = t1 + std::pow(std::max(glm::distance(p1.value, p2.value), epsilon), alpha);
			float_t t3 = t2 + std::pow(std::max(glm::distance(p2.value, p3.value), epsilon), alpha);

			T m1 = (1.0 - tension) * (t2 - t1) * ((p1.value - p0.value) / (t1 - t0) - (p2.value - p0.value) / (t2 - t0) + (p2.value - p1.value) / (t2 - t1));
			T m2 = (1.0 - tension) * (t2 - t1) * ((p2.value - p1.value) / (t2 - t1) - (p3.value - p1.value) / (t3 - t1) + (p3.value - p2.value) / (t3 - t2));
			T a = +2.0 * (p1.value - p2.value) + m1 + m2;
			T b = -3.0 * (p1.value - p2.value) - m1 - m1 - m2;
			T c = m1;
			T d = p1.value;

			float_t t = (position - p1.position) / (p2.position - p1.position);

			valueCache[cacheIndex] =
				a * t * t * t +
				b * t * t +
				c * t +
				d;

			pointStartIndex = pointIndex;
		}
	}

	void fillCacheBezier() {
		for(std::size_t cacheIndex = 0, pointStartIndex = 0; cacheIndex < valueCache.size(); cacheIndex++) {
			float_t position = static_cast<float_t>(cacheIndex) / static_cast<float_t>(valueCache.size() - 1);
			std::size_t pointIndex = findIndex(position, pointStartIndex);

			if(pointIndex == invalidIndex) {
				valueCache[cacheIndex] = curvePoints.front().value;
				continue;
			}
			else if(pointIndex + 1 == curvePoints.size()) {
				valueCache[cacheIndex] = curvePoints.back().value;
				continue;
			}

			const Point& p1 = curvePoints[pointIndex];
			const Point& p2 = curvePoints[pointIndex + 1];
			T c1 = p1.value + curveControlPoints[pointIndex * 2 + 1];
			T c2 = p2.value + curveControlPoints[pointIndex * 2 + 2];

			float_t t = (position - p1.position) / (p2.position - p1.position);

			valueCache[cacheIndex] =
				p1.value * (1.0 - t) * (1.0 - t) * (1.0 - t) +
				c1 * (1.0 - t) * (1.0 - t) * t * 3.0 +
				c2 * (1.0 - t) * t * t * 3.0 +
				p2.value * t * t * t;

			pointStartIndex = pointIndex;
		}
	}

	std::size_t findIndex(float_t position, std::size_t indexStart) const {
		for(std::size_t index = indexStart; index < curvePoints.size(); index++) {
			if(curvePoints[index].position >= position) {
				return index > 0 ? index - 1 : invalidIndex;
			}
		}

		return curvePoints.size() - 1;
	}

	CurveInterpolation curveInterpolation = CurveInterpolation::linear;
	bool adaptiveCache = true;

	std::vector<Point> curvePoints;
	std::vector<T> curveControlPoints;

	std::vector<T> valueCache;
};

template <typename T, typename BinOp>
T accumulateCurveValues(const Curve<T>& curve, BinOp operation, T value = T(0)) {
	for(const T& currentValue : curve.cache()) {
		value = operation(value, currentValue);
	}

	return value;
}

template <typename T>
Curve<T> flattenCurve(const Curve<T>& curve, const T& value) {
	std::vector<typename Curve<T>::Point> points = curve.points();
	for(typename Curve<T>::Point& point : points) {
		point.value = value;
	}

	return Curve<T>(points, curve.controlPoints(), curve.interpolation());
}

template <typename T>
Curve<T> moveCurve(const Curve<T>& curve, const T& delta) {
	std::vector<typename Curve<T>::Point> points = curve.points();
	for(typename Curve<T>::Point& point : points) {
		point.value += delta;
	}

	return Curve<T>(points, curve.controlPoints(), curve.interpolation());
}

template <typename T>
Curve<T> scaleCurve(const Curve<T>& curve, const T& factor) {
	std::vector<typename Curve<T>::Point> points = curve.points();
	for(typename Curve<T>::Point& point : points) {
		point.value *= factor;
	}

	return Curve<T>(points, curve.controlPoints(), curve.interpolation());
}

template <typename T>
Curve<T> shiftCurve(const Curve<T>& curve, float_t delta) {
	std::vector<typename Curve<T>::Point> points = curve.points();
	for(typename Curve<T>::Point& point : points) {
		point.position = std::min(std::max(point.position + delta, 0.0), 1.0);
	}

	return Curve<T>(points, curve.controlPoints(), curve.interpolation());
}

template <typename T>
Curve<T> scaleShiftCurve(const Curve<T>& curve, float_t factor) {
	std::vector<typename Curve<T>::Point> points = curve.points();
	for(typename Curve<T>::Point& point : points) {
		point.position = std::min(std::max(point.position * factor, 0.0), 1.0);
	}

	return Curve<T>(points, curve.controlPoints(), curve.interpolation());
}

NLOHMANN_JSON_SERIALIZE_ENUM(CurveInterpolation, {
	{ CurveInterpolation::none, "none" },
	{ CurveInterpolation::linear, "linear" },
	{ CurveInterpolation::spline, "spline" },
	{ CurveInterpolation::bezier, "bezier" }
})

template <typename T>
void to_json(nlohmann::ordered_json& j, const typename Curve<T>::Point& point) {
	j = nlohmann::ordered_json::array({
		point.position,
		point.value
	});
}
template <typename T>
void from_json(const nlohmann::ordered_json& j, typename Curve<T>::Point& point) {
	point.position = j.at(0);
	point.value = j.at(1);
}

template <typename T>
void to_json(nlohmann::ordered_json& j, const Curve<T>& curve) {
	nlohmann::ordered_json jPointList = nlohmann::ordered_json::array();
	for(const typename Curve<T>::Point& point : curve.points()) {
		nlohmann::ordered_json jPoint;
		to_json<T>(jPoint, point);

		jPointList.push_back(jPoint);
	}

	j = nlohmann::ordered_json{
		{ "interpolation", curve.interpolation() },
		{ "points", jPointList },
		{ "control_points", curve.controlPoints() }
	};
}
template <typename T>
void from_json(const nlohmann::ordered_json& j, Curve<T>& curve) {
	std::vector<typename Curve<T>::Point> points;

	const nlohmann::ordered_json& jPointList = j.at("points");
	for(const nlohmann::ordered_json& jPoint : jPointList) {
		typename Curve<T>::Point point;
		from_json<T>(jPoint, point);

		points.push_back(point);
	}

	curve = Curve<T>(
		points,
		j.value("control_points", std::vector<T>()),
		j.value("interpolation", CurveInterpolation::linear));
}
}