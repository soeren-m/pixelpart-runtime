#pragma once

#include "Types.h"
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
		cache.resize(minCacheSize);
	}
	Curve(CurveInterpolation interp) : interpolation(interp) {
		cache.resize(minCacheSize);
	}
	Curve(float_t initialPosition, const T& initialValue, CurveInterpolation interp = CurveInterpolation::linear) : Curve(interp) {
		addPoint(initialPosition, initialValue);
	}
	Curve(const T& initialValue, CurveInterpolation interp = CurveInterpolation::linear) : Curve(interp) {
		addPoint(0.5, initialValue);
	}
	Curve(const std::vector<Point>& pointList, CurveInterpolation interp = CurveInterpolation::linear) : Curve(interp) {
		setPoints(pointList);
	}

	T operator()(float_t position = 0.0) const {
		return get(position);
	}

	T get(float_t position = 0.0) const {
		return cache[static_cast<std::size_t>(std::min(std::max(position, 0.0), 1.0) * static_cast<float_t>(cache.size() - 1))];
	}

	void setPoints(const std::vector<Point>& pointList) {
		points = pointList;
		refreshCache();
	}
	void setPoints(const Point* pointList, std::size_t numPoints) {
		points = pointList != nullptr && numPoints > 0u
			? std::vector<Point>(pointList, pointList + numPoints)
			: std::vector<Point>();
		refreshCache();
	}

	template <typename IntT>
	void setPointsOrdered(const float_t* positionList, const T* valueList, const IntT* order, IntT numPoints) {
		points.clear();
		if(positionList && valueList && order && numPoints > 0) {
			points.resize(numPoints);
			for(IntT i = 0; i < numPoints; i++) {
				points[i] = Point{ positionList[i], valueList[order[i]] };
			}
		}

		refreshCache();
	}

	void addPoint(float_t position, const T& value) {
		points.push_back(Point{ std::min(std::max(position, 0.0), 1.0), value });
		refreshCache();
	}
	void removePoint(std::size_t index) {
		if(index >= points.size()) {
			return;
		}

		points.erase(points.begin() + index);
		refreshCache();
	}
	void setPoint(std::size_t index, const T& value) {
		if(index >= points.size()) {
			return;
		}

		points[index].value = value;
		refreshCache();
	}
	void setPointPosition(std::size_t index, float_t position) {
		if(index >= points.size()) {
			return;
		}

		points[index].position = std::min(std::max(position, 0.0), 1.0);
		refreshCache();
	}
	void clear() {
		points.clear();
		refreshCache();
	}

	bool containsPoints() const {
		return !points.empty();
	}
	std::size_t getNumPoints() const {
		return points.size();
	}
	std::vector<Point>& getPoints() {
		return points;
	}
	const std::vector<Point>& getPoints() const {
		return points;
	}
	const Point& getPoint(std::size_t index) const {
		return points.at(index);
	}

	std::ptrdiff_t getPointIndex(float_t position, float_t epsilon = 0.001) const {
		if(points.empty()) {
			return -1;
		}

		typename std::vector<Point>::const_iterator it = std::min_element(points.begin(), points.end(), [position](const Point& p1, const Point& p2) {
			return std::abs(p1.position - position) < std::abs(p2.position - position);
		});

		if(std::abs(it->position - position) > epsilon) {
			return -1;
		}

		return it - points.begin();
	}

	void setInterpolation(CurveInterpolation method) {
		bool needToRefreshCache = method != interpolation;
		interpolation = method;

		if(needToRefreshCache) {
			refreshCache();
		}
	}
	CurveInterpolation getInterpolation() const {
		return interpolation;
	}

	void enableAdaptiveCache() {
		adaptiveCache = true;
		refreshCache();
	}
	void enableFixedCache(std::size_t size) {
		adaptiveCache = false;
		cache.resize(std::max(size, static_cast<std::size_t>(1u)));
		refreshCache();
	}
	const std::vector<T>& getCache() const {
		return cache;
	}

	void refreshCache() {
		std::sort(points.begin(), points.end(), [](const Point& p1, const Point& p2) {
			return p1.position < p2.position;
		});

		if(adaptiveCache) {
			if(cache.size() < maxCacheSize && points.size() > cache.size() / 16u) {
				cache.resize(cache.size() * 2u);
			}
			else if(cache.size() > minCacheSize && points.size() < cache.size() / 32u) {
				cache.resize(cache.size() / 2u);
			}
		}

		if(points.empty()) {
			std::fill(cache.begin(), cache.end(), T(0));
		}
		else if(points.size() == 1u) {
			std::fill(cache.begin(), cache.end(), points.back().value);
		}
		else if(interpolation == CurveInterpolation::none) {
			for(std::size_t i = 0u, k0 = 0u; i < cache.size(); i++) {
				float_t position = static_cast<float_t>(i) / static_cast<float_t>(cache.size() - 1u);
				std::size_t k = findIndex(position, k0);

				if(k == invalidIndex) {
					cache[i] = points.front().value;
				}
				else if(k + 1u == points.size()) {
					cache[i] = points.back().value;
				}
				else {
					cache[i] = (std::abs(position - points[k].position) < std::abs(position - points[k + 1u].position))
						? points[k].value
						: points[k + 1u].value;

					k0 = k;
				}
			}
		}
		else if(interpolation == CurveInterpolation::linear) {
			for(std::size_t i = 0u, k0 = 0u; i < cache.size(); i++) {
				float_t position = static_cast<float_t>(i) / static_cast<float_t>(cache.size() - 1u);
				std::size_t k = findIndex(position, k0);

				if(k == invalidIndex) {
					cache[i] = points.front().value;
				}
				else if(k + 1u == points.size()) {
					cache[i] = points.back().value;
				}
				else {
					float_t t = (position - points[k].position) / (points[k + 1u].position - points[k].position);
					cache[i] =
						(1.0 - t) * points[k].value +
						t * points[k + 1u].value;

					k0 = k;
				}
			}
		}
		else if(interpolation == CurveInterpolation::spline) {
			for(std::size_t i = 0u, k0 = 0u; i < cache.size(); i++) {
				float_t position = static_cast<float_t>(i) / static_cast<float_t>(cache.size() - 1u);
				std::size_t k = findIndex(position, k0);

				if(k == invalidIndex) {
					cache[i] = points.front().value;
				}
				else if(k + 1u == points.size()) {
					cache[i] = points.back().value;
				}
				else {
					const float_t alpha = 0.5;
					const float_t tension = 0.0;

					Point p0 = (k > 0u) ? points[k - 1u] : Point{ -0.1, points[k].value };
					Point p1 = points[k];
					Point p2 = points[k + 1u];
					Point p3 = (k + 2u < points.size()) ? points[k + 2u] : Point{ +1.1, points[k + 1u].value };

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

					cache[i] =
						a * t * t * t +
						b * t * t +
						c * t +
						d;

					k0 = k;
				}
			}
		}
	}

private:
	std::size_t findIndex(float_t position, std::size_t indexStart) {
		for(std::size_t i = indexStart; i < points.size(); i++) {
			if(points[i].position >= position) {
				return (i > 0u) ? (i - 1u) : invalidIndex;
			}
		}

		return points.size() - 1u;
	}

	CurveInterpolation interpolation = CurveInterpolation::linear;
	bool adaptiveCache = true;

	std::vector<Point> points;
	std::vector<T> cache;
};

template <typename T, typename BinOp>
T accumulateCurveValues(const Curve<T>& curve, BinOp operation, T value = T(0)) {
	for(const T& currentValue : curve.getCache()) {
		value = operation(value, currentValue);
	}

	return value;
}

template <typename T>
void flattenCurve(Curve<T>& curve, const T& value) {
	for(typename Curve<T>::Point& point : curve.getPoints()) {
		point.value = value;
	}

	curve.refreshCache();
}

template <typename T>
void moveCurve(Curve<T>& curve, const T& delta) {
	for(typename Curve<T>::Point& point : curve.getPoints()) {
		point.value += delta;
	}

	curve.refreshCache();
}

template <typename T>
void scaleCurve(Curve<T>& curve, const T& factor) {
	for(typename Curve<T>::Point& point : curve.getPoints()) {
		point.value *= factor;
	}

	curve.refreshCache();
}

template <typename T>
void shiftCurve(Curve<T>& curve, float_t delta) {
	for(typename Curve<T>::Point& point : curve.getPoints()) {
		point.position = std::min(std::max(point.position + delta, 0.0), 1.0);
	}

	curve.refreshCache();
}

template <typename T>
void shiftCurveByFactor(Curve<T>& curve, float_t factor) {
	for(typename Curve<T>::Point& point : curve.getPoints()) {
		point.position = std::min(std::max(point.position * factor, 0.0), 1.0);
	}

	curve.refreshCache();
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
	for(const typename Curve<T>::Point& point : curve.getPoints()) {
		nlohmann::ordered_json jPoint;
		to_json<T>(jPoint, point);

		jPointList.push_back(jPoint);
	}

	j = nlohmann::ordered_json{
		{ "interpolation", curve.getInterpolation() },
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

	curve = Curve<T>();
	curve.setPoints(points);

	if(j.contains("interpolation")) {
		curve.setInterpolation(j.at("interpolation").get<CurveInterpolation>());
	}
}
}