#pragma once

#include "Curve.h"

namespace pixelpart {
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
void shiftCurve(Curve<T>& curve, floatd delta) {
	for(typename Curve<T>::Point& point : curve.getPoints()) {
		point.position = std::min(std::max(point.position + delta, 0.0), 1.0);
	}

	curve.refreshCache();
}

template <typename T>
void shiftCurveByFactor(Curve<T>& curve, floatd factor) {
	for(typename Curve<T>::Point& point : curve.getPoints()) {
		point.position = std::min(std::max(point.position * factor, 0.0), 1.0);
	}

	curve.refreshCache();
}
}