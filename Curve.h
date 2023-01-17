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
		floatd position;
		T value;
	};

	static constexpr uint32_t minCacheSize = 256;
	static constexpr uint32_t maxCacheSize = 1024;
	static constexpr uint32_t invalidIndex = 0xFFFFFFFF;

	Curve() {
		cache.resize(minCacheSize);
	}
	Curve(CurveInterpolation interp) : interpolation(interp) {
		cache.resize(minCacheSize);
	}
	Curve(floatd initialPosition, const T& initialValue, CurveInterpolation interp = CurveInterpolation::linear) : Curve(interp) {
		addPoint(initialPosition, initialValue);
	}
	Curve(const T& initialValue, CurveInterpolation interp = CurveInterpolation::linear) : Curve(interp) {
		addPoint(0.5, initialValue);
	}
	Curve(const std::vector<Point>& pointList, CurveInterpolation interp = CurveInterpolation::linear) : Curve(interp) {
		setPoints(pointList);
	}

	T operator()(floatd position = 0.0) const {
		return get(position);
	}

	T get(floatd position = 0.0) const {
		return cache[static_cast<std::size_t>(std::min(std::max(position, 0.0), 1.0) * static_cast<floatd>(cache.size() - 1))];
	}

	void setPoints(const std::vector<Point>& pointList) {
		points = pointList;
		refreshCache();
	}
	void setPoints(const Point* pointList, std::size_t numPoints) {
		points = (pointList && numPoints > 0) ? std::vector<Point>(pointList, pointList + numPoints) : std::vector<Point>();
		refreshCache();
	}

	template <typename IntT>
	void setPointsOrdered(const floatd* positionList, const T* valueList, const IntT* order, IntT numPoints) {
		points.clear();
		if(positionList && valueList && order && numPoints > 0) {
			points.resize(numPoints);
			for(IntT i = 0; i < numPoints; i++) {
				points[i] = Point{ positionList[i], valueList[order[i]] };
			}
		}

		refreshCache();
	}

	void addPoint(floatd position, const T& value) {
		points.push_back(Point{ std::min(std::max(position, 0.0), 1.0), value });
		refreshCache();
	}
	void removePoint(floatd position) {
		points.remove_if([position](const Point& m) { return m.position == position; });
		refreshCache();
	}
	void removePoint(std::size_t index) {
		if(index >= points.size()) {
			return;
		}

		points.erase(points.begin() + index);
		refreshCache();
	}
	void clear() {
		points.clear();
		refreshCache();
	}

	T setPoint(std::size_t index, const T& value) {
		if(index < points.size()) {
			T result = value;
			points[index].value = result;
			refreshCache();

			return result;
		}

		return T(0);
	}
	T movePoint(std::size_t index, const T& delta) {
		if(index < points.size()) {
			T result = points[index].value + delta;
			points[index].value = result;
			refreshCache();

			return result;
		}

		return T(0);
	}
	floatd shiftPoint(std::size_t index, floatd delta) {
		if(index < points.size()) {
			floatd result = std::min(std::max(points[index].position + delta, 0.0), 1.0);
			points[index].position = result;
			refreshCache();

			return result;
		}

		return 0.0;
	}

	void move(const T& delta) {
		for(Point& point : points) {
			point.value += delta;
		}
		refreshCache();
	}
	void moveByFactor(const T& factor) {
		for(Point& point : points) {
			point.value *= factor;
		}
		refreshCache();
	}
	void shift(floatd delta) {
		for(Point& point : points) {
			point.position = std::min(std::max(point.position + delta, 0.0), 1.0);
		}
		refreshCache();
	}
	void shiftByFactor(floatd factor) {
		for(Point& point : points) {
			point.position = std::min(std::max(point.position * factor, 0.0), 1.0);
		}
		refreshCache();
	}

	template <typename BinOp>
	T accumulate(BinOp operation, T value = T(0)) const {
		for(const T& currentValue : cache) {
			value = operation(value, currentValue);
		}

		return value;
	}

	int32_t getIndex(floatd position) const {
		if(points.empty()) {
			return -1;
		}

		return std::min_element(points.begin(), points.end(), [position](const Point& p1, const Point& p2) {
					return std::abs(p1.position - position) < std::abs(p2.position - position);
				}) - points.begin();
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
		cache.resize(std::max(size, static_cast<std::size_t>(1U)));
		refreshCache();
	}
	const std::vector<T>& getCache() const {
		return cache;
	}
	const T* getCacheData() const {
		return cache.data();
	}
	std::size_t getCacheSize() const {
		return cache.size();
	}

	void refreshCache() {
		std::sort(points.begin(), points.end(), [](const Point& p1, const Point& p2) {
			return p1.position < p2.position;
		});

		if(adaptiveCache) {
			if(cache.size() < maxCacheSize && points.size() > cache.size() / 16) {
				cache.resize(cache.size() * 2);
			}
			else if(cache.size() > minCacheSize && points.size() < cache.size() / 32) {
				cache.resize(cache.size() / 2);
			}
		}

		if(points.empty()) {
			std::fill(cache.begin(), cache.end(), T(0));
		}
		else if(points.size() == 1) {
			std::fill(cache.begin(), cache.end(), points.back().value);
		}
		else if(interpolation == CurveInterpolation::none) {
			for(std::size_t i = 0, k0 = 0; i < cache.size(); i++) {
				floatd position = static_cast<floatd>(i) / static_cast<floatd>(cache.size() - 1);
				std::size_t k = findIndex(position, k0);

				if(k == invalidIndex) {
					cache[i] = points.front().value;
				}
				else if(k + 1 == points.size()) {
					cache[i] = points.back().value;
				}
				else {
					cache[i] = (std::abs(position - points[k].position) < std::abs(position - points[k + 1].position))
						? points[k].value
						: points[k + 1].value;

					k0 = k;
				}
			}
		}
		else if(interpolation == CurveInterpolation::linear) {
			for(std::size_t i = 0, k0 = 0; i < cache.size(); i++) {
				floatd position = static_cast<floatd>(i) / static_cast<floatd>(cache.size() - 1);
				std::size_t k = findIndex(position, k0);

				if(k == invalidIndex) {
					cache[i] = points.front().value;
				}
				else if(k + 1 == points.size()) {
					cache[i] = points.back().value;
				}
				else {
					floatd t = (position - points[k].position) / (points[k + 1].position - points[k].position);
					cache[i] =
						(1.0 - t) * points[k].value +
						t * points[k + 1].value;

					k0 = k;
				}
			}
		}
		else if(interpolation == CurveInterpolation::spline) {
			for(std::size_t i = 0, k0 = 0; i < cache.size(); i++) {
				floatd position = static_cast<floatd>(i) / static_cast<floatd>(cache.size() - 1);
				std::size_t k = findIndex(position, k0);

				if(k == invalidIndex) {
					cache[i] = points.front().value;
				}
				else if(k + 1 == points.size()) {
					cache[i] = points.back().value;
				}
				else {
					const floatd alpha = 0.5;
					const floatd tension = 0.0;

					Point p0 = (k > 0) ? points[k - 1] : Point{ -0.1, points[k].value };
					Point p1 = points[k];
					Point p2 = points[k + 1];
					Point p3 = (k + 2 < points.size()) ? points[k + 2] : Point{ +1.1, points[k + 1].value };

					floatd t = (position - p1.position) / (p2.position - p1.position);
					floatd t0 = 0.0;
					floatd t1 = t0 + std::pow(p1.position - p0.position, alpha);
					floatd t2 = t1 + std::pow(p2.position - p1.position, alpha);
					floatd t3 = t2 + std::pow(p3.position - p2.position, alpha);

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
	std::size_t findIndex(floatd position, std::size_t indexStart) {
		for(std::size_t i = indexStart; i < points.size(); i++) {
			if(points[i].position >= position) {
				return (i > 0) ? (i - 1) : invalidIndex;
			}
		}

		return points.size() - 1;
	}

	CurveInterpolation interpolation = CurveInterpolation::linear;
	bool adaptiveCache = true;

	std::vector<Point> points;
	std::vector<T> cache;
};

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