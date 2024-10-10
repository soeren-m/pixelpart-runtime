#pragma once

#include "Types.h"
#include "../json/json.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

namespace pixelpart {
enum class CurveInterpolation : uint32_t {
	none = 0,
	linear = 1,
	spline = 2
};

template <typename T>
class Curve {
public:
	struct Point {
		float_t position;
		T value;
	};

	static constexpr uint32_t minCacheSize = 256u;
	static constexpr uint32_t maxCacheSize = 1024u;
	static constexpr uint32_t invalidIndex = 0xFFFFFFFFu;

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

	T operator()(float_t position = 0.0) const {
		return at(position);
	}

	T at(float_t position = 0.0) const {
		return valueCache[
			static_cast<std::size_t>(std::min(std::max(position, 0.0), 1.0) * static_cast<float_t>(valueCache.size() - 1u))];
	}

	void points(const std::vector<Point>& pointList) {
		curvePoints = pointList;
		updateCache();
	}
	void points(const Point* pointList, std::size_t count) {
		curvePoints = pointList != nullptr && count > 0u
			? std::vector<Point>(pointList, pointList + count)
			: std::vector<Point>();
		updateCache();
	}

	template <typename IntT>
	void orderedPoints(const float_t* positionList, const T* valueList, const IntT* order, IntT count) {
		curvePoints.clear();
		if(positionList && valueList && order && count > 0) {
			curvePoints.resize(count);
			for(IntT i = 0; i < count; i++) {
				curvePoints[i] = Point{ positionList[i], valueList[order[i]] };
			}
		}

		updateCache();
	}

	void addPoint(float_t position, const T& value) {
		curvePoints.push_back(Point{ std::min(std::max(position, 0.0), 1.0), value });
		updateCache();
	}
	void removePoint(std::size_t index) {
		if(index >= curvePoints.size()) {
			return;
		}

		curvePoints.erase(curvePoints.begin() + index);
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
	void clear() {
		curvePoints.clear();
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

	std::ptrdiff_t pointIndex(float_t position, float_t epsilon = 0.001) const {
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
		valueCache.resize(std::max(size, static_cast<std::size_t>(1u)));
		updateCache();
	}
	const std::vector<T>& cache() const {
		return valueCache;
	}

private:
	void updateCache() {
		std::sort(curvePoints.begin(), curvePoints.end(), [](const Point& p1, const Point& p2) {
			return p1.position < p2.position;
		});

		if(adaptiveCache) {
			if(valueCache.size() < maxCacheSize && curvePoints.size() > valueCache.size() / 16u) {
				valueCache.resize(valueCache.size() * 2u);
			}
			else if(valueCache.size() > minCacheSize && curvePoints.size() < valueCache.size() / 32u) {
				valueCache.resize(valueCache.size() / 2u);
			}
		}

		if(curvePoints.empty()) {
			std::fill(valueCache.begin(), valueCache.end(), T(0));
		}
		else if(curvePoints.size() == 1u) {
			std::fill(valueCache.begin(), valueCache.end(), curvePoints.back().value);
		}
		else if(curveInterpolation == CurveInterpolation::none) {
			for(std::size_t i = 0u, k0 = 0u; i < valueCache.size(); i++) {
				float_t position = static_cast<float_t>(i) / static_cast<float_t>(valueCache.size() - 1u);
				std::size_t k = findIndex(position, k0);

				if(k == invalidIndex) {
					valueCache[i] = curvePoints.front().value;
				}
				else if(k + 1u == curvePoints.size()) {
					valueCache[i] = curvePoints.back().value;
				}
				else {
					valueCache[i] = (std::abs(position - curvePoints[k].position) < std::abs(position - curvePoints[k + 1u].position))
						? curvePoints[k].value
						: curvePoints[k + 1u].value;

					k0 = k;
				}
			}
		}
		else if(curveInterpolation == CurveInterpolation::linear) {
			for(std::size_t i = 0u, k0 = 0u; i < valueCache.size(); i++) {
				float_t position = static_cast<float_t>(i) / static_cast<float_t>(valueCache.size() - 1u);
				std::size_t k = findIndex(position, k0);

				if(k == invalidIndex) {
					valueCache[i] = curvePoints.front().value;
				}
				else if(k + 1u == curvePoints.size()) {
					valueCache[i] = curvePoints.back().value;
				}
				else {
					float_t t = (position - curvePoints[k].position) / (curvePoints[k + 1u].position - curvePoints[k].position);
					valueCache[i] =
						(1.0 - t) * curvePoints[k].value +
						t * curvePoints[k + 1u].value;

					k0 = k;
				}
			}
		}
		else if(curveInterpolation == CurveInterpolation::spline) {
			for(std::size_t i = 0u, k0 = 0u; i < valueCache.size(); i++) {
				float_t position = static_cast<float_t>(i) / static_cast<float_t>(valueCache.size() - 1u);
				std::size_t k = findIndex(position, k0);

				if(k == invalidIndex) {
					valueCache[i] = curvePoints.front().value;
				}
				else if(k + 1u == curvePoints.size()) {
					valueCache[i] = curvePoints.back().value;
				}
				else {
					const float_t alpha = 0.5;
					const float_t tension = 0.0;

					Point p0 = (k > 0u) ? curvePoints[k - 1u] : Point{ -0.1, curvePoints[k].value };
					Point p1 = curvePoints[k];
					Point p2 = curvePoints[k + 1u];
					Point p3 = (k + 2u < curvePoints.size()) ? curvePoints[k + 2u] : Point{ +1.1, curvePoints[k + 1u].value };

					float_t t = (position - p1.position) / (p2.position - p1.position);
					float_t t0 = 0.0;
					float_t t1 = t0 + std::pow(p1.position - p0.position, alpha);
					float_t t2 = t1 + std::pow(p2.position - p1.position, alpha);
					float_t t3 = t2 + std::pow(p3.position - p2.position, alpha);

					T m1 = (1.0 - tension) * (t2 - t1) * ((p1.value - p0.value) / (t1 - t0) - (p2.value - p0.value) / (t2 - t0) + (p2.value - p1.value) / (t2 - t1));
					T m2 = (1.0 - tension) * (t2 - t1) * ((p2.value - p1.value) / (t2 - t1) - (p3.value - p1.value) / (t3 - t1) + (p3.value - p2.value) / (t3 - t2));
					T a = +2.0 * (p1.value - p2.value) + m1 + m2;
					T b = -3.0 * (p1.value - p2.value) - m1 - m1 - m2;
					T c = m1;
					T d = p1.value;

					valueCache[i] =
						a * t * t * t +
						b * t * t +
						c * t +
						d;

					k0 = k;
				}
			}
		}
	}

	std::size_t findIndex(float_t position, std::size_t indexStart) {
		for(std::size_t i = indexStart; i < curvePoints.size(); i++) {
			if(curvePoints[i].position >= position) {
				return (i > 0u) ? (i - 1u) : invalidIndex;
			}
		}

		return curvePoints.size() - 1u;
	}

	CurveInterpolation curveInterpolation = CurveInterpolation::linear;
	bool adaptiveCache = true;

	std::vector<Point> curvePoints;
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

	return Curve<T>(points, curve.interpolation());
}

template <typename T>
Curve<T> moveCurve(const Curve<T>& curve, const T& delta) {
	std::vector<typename Curve<T>::Point> points = curve.points();
	for(typename Curve<T>::Point& point : points) {
		point.value += delta;
	}

	return Curve<T>(points, curve.interpolation());
}

template <typename T>
Curve<T> scaleCurve(const Curve<T>& curve, const T& factor) {
	std::vector<typename Curve<T>::Point> points = curve.points();
	for(typename Curve<T>::Point& point : points) {
		point.value *= factor;
	}

	return Curve<T>(points, curve.interpolation());
}

template <typename T>
Curve<T> shiftCurve(const Curve<T>& curve, float_t delta) {
	std::vector<typename Curve<T>::Point> points = curve.points();
	for(typename Curve<T>::Point& point : points) {
		point.position = std::min(std::max(point.position + delta, 0.0), 1.0);
	}

	return Curve<T>(points, curve.interpolation());
}

template <typename T>
Curve<T> shiftCurveByFactor(const Curve<T>& curve, float_t factor) {
	std::vector<typename Curve<T>::Point> points = curve.points();
	for(typename Curve<T>::Point& point : points) {
		point.position = std::min(std::max(point.position * factor, 0.0), 1.0);
	}

	return Curve<T>(points, curve.interpolation());
}

NLOHMANN_JSON_SERIALIZE_ENUM(CurveInterpolation, {
	{ CurveInterpolation::none, "none" },
	{ CurveInterpolation::linear, "linear" },
	{ CurveInterpolation::spline, "spline" }
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
		{ "points", jPointList }
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

	curve = Curve<T>(points, j.value("interpolation", CurveInterpolation::linear));
}
}