#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Common.h"
#include "Geometry.h"
#include <cmath>
#include <algorithm>

namespace pixelpart {
namespace math {
template <typename TVec, typename T>
TVec stepInterpolation(const TVec& leftValue, const TVec& rightValue, T edge, T x) {
	return x < edge ? leftValue : rightValue;
}
template <typename T>
vector2<T> stepInterpolation(const vector2<T>& leftValue, const vector2<T>& rightValue, const vector2<T>& edge, const vector2<T>& x) {
	return vector2<T>(
		stepInterpolation(leftValue.x, rightValue.x, edge.x, x.x),
		stepInterpolation(leftValue.y, rightValue.y, edge.y, x.y));
}
template <typename T>
vector3<T> stepInterpolation(const vector3<T>& leftValue, const vector3<T>& rightValue, const vector3<T>& edge, const vector3<T>& x) {
	return vector3<T>(
		stepInterpolation(leftValue.x, rightValue.x, edge.x, x.x),
		stepInterpolation(leftValue.y, rightValue.y, edge.y, x.y),
		stepInterpolation(leftValue.z, rightValue.z, edge.z, x.z));
}
template <typename T>
vector4<T> stepInterpolation(const vector4<T>& leftValue, const vector4<T>& rightValue, const vector4<T>& edge, const vector4<T>& x) {
	return vector4<T>(
		stepInterpolation(leftValue.x, rightValue.x, edge.x, x.x),
		stepInterpolation(leftValue.y, rightValue.y, edge.y, x.y),
		stepInterpolation(leftValue.z, rightValue.z, edge.z, x.z),
		stepInterpolation(leftValue.w, rightValue.w, edge.w, x.w));
}

template <typename TVec, typename T>
TVec smoothstepInterpolation(const TVec& leftValue, const TVec& rightValue, T leftEdge, T rightEdge, T x) {
	T t = clamp((x - leftEdge) / (rightEdge - leftEdge), T(0), T(1));

	return t * t * (T(3) - T(2) * t) * (rightValue - leftValue) + leftValue;
}
template <typename TVec>
TVec smoothstepInterpolation(const TVec& leftValue, const TVec& rightValue, const TVec& leftEdge, const TVec& rightEdge, const TVec& x) {
	TVec t = clamp((x - leftEdge) / (rightEdge - leftEdge), TVec(0), TVec(1));

	return t * t * (TVec(3) - TVec(2) * t) * (rightValue - leftValue) + leftValue;
}

template <typename TVec, typename T>
TVec linearInterpolation(const TVec& leftValue, const TVec& rightValue, T x) {
	return leftValue * (T(1) - x) + rightValue * x;
}
template <typename TVec>
TVec linearInterpolation(const TVec& leftValue, const TVec& rightValue, const TVec& x) {
	return leftValue * (TVec(1) - x) + rightValue * x;
}

template <typename TVec, typename T>
TVec linearInterpolation(const TVec& leftValue, const TVec& rightValue, T leftEdge, T rightEdge, T x) {
	T t = (x - leftEdge) / (rightEdge - leftEdge);

	return leftValue * (T(1) - t) + rightValue * t;
}
template <typename TVec>
TVec linearInterpolation(const TVec& leftValue, const TVec& rightValue, const TVec& leftEdge, const TVec& rightEdge, const TVec& x) {
	TVec t = (x - leftEdge) / (rightEdge - leftEdge);

	return leftValue * (TVec(1) - t) + rightValue * t;
}

template <typename TVec, typename T>
TVec catmullRomSplineInterpolation(const TVec& left, const TVec& right, const TVec& leftControl, const TVec& rightControl, T leftEdge, T rightEdge, T x) {
	const T alpha = T(0.5);
	const T tension = T(0.0);
	const T epsilon = T(0.001);

	T a = clamp((x - leftEdge) / (rightEdge - leftEdge), T(0), T(1));

	T t0 = T(0);
	T t1 = t0 + std::pow(std::max(distance(leftControl, left), epsilon), alpha);
	T t2 = t1 + std::pow(std::max(distance(left, right), epsilon), alpha);
	T t3 = t2 + std::pow(std::max(distance(right, rightControl), epsilon), alpha);

	TVec m1 = (T(1) - tension) * (t2 - t1) * ((left - leftControl) / (t1 - t0) - (right - leftControl) / (t2 - t0) + (right - left) / (t2 - t1));
	TVec m2 = (T(1) - tension) * (t2 - t1) * ((right - left) / (t2 - t1) - (rightControl - left) / (t3 - t1) + (rightControl - right) / (t3 - t2));
	TVec seg0 = T(2) * (left - right) + m1 + m2;
	TVec seg1 = T(-3) * (left - right) - m1 - m1 - m2;
	TVec seg2 = m1;
	TVec seg3 = left;

	return seg0 * a * a * a +
		seg1 * a * a +
		seg2 * a +
		seg3;
}

template <typename TVec, typename T>
TVec cubicBezierInterpolation(const TVec& left, const TVec& right, const TVec& leftControl, const TVec& rightControl, T leftEdge, T rightEdge, T x) {
	T a = clamp((x - leftEdge) / (rightEdge - leftEdge), T(0), T(1));
	T inva = T(1) - a;

	return left * inva * inva * inva +
		leftControl * inva * inva * a * T(3) +
		rightControl * inva * a * a * T(3) +
		right * a * a * a;
}
}
}
