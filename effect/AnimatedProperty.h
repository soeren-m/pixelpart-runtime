#pragma once

#include "ComputeOutputOperation.h"
#include "ComputeOutputTarget.h"
#include "ComputeGraph.h"
#include "OutputComputeNode.h"
#include "../common/Types.h"
#include "../common/Curve.h"
#include "../common/VariantValue.h"
#include "../json/json.hpp"
#include <optional>
#include <vector>

namespace pixelpart {
template <typename T>
class AnimatedProperty {
public:
	AnimatedProperty() {
		propertyComputeGraph.addNode<OutputComputeNode>();
		recalculateResult();
	}
	AnimatedProperty(const T& initialValue) : propertyCurve(initialValue) {
		propertyComputeGraph.addNode<OutputComputeNode>();
		recalculateResult();
	}
	AnimatedProperty(float_t initialPosition, const T& initialValue) : propertyCurve(initialPosition, initialValue) {
		propertyComputeGraph.addNode<OutputComputeNode>();
		recalculateResult();
	}
	AnimatedProperty(const Curve<T>& initialCurve, const ComputeGraph& graph, ComputeOutputOperation outputOp, ComputeOutputTarget outputTgt) :
		propertyCurve(initialCurve),
		propertyComputeGraph(graph),
		propertyOutputOperation(outputOp),
		propertyOutputTarget(outputTgt) {
		if(propertyComputeGraph.nodes().size() == 0) {
			propertyComputeGraph.addNode<OutputComputeNode>();
		}

		recalculateResult();
	}

	T operator()(float_t position = 0.0) const {
		return propertyComputedCurve.at(position);
	}

	T at(float_t position = 0.0) const {
		return propertyComputedCurve.at(position);
	}

	void applyInputs(const ComputeGraph::InputSet& inputs) {
		if(propertyComputeGraph.empty()) {
			propertyGraphOutputValue = std::nullopt;
			recalculateResult();

			return;
		}

		propertyComputeGraph.unlinkRemovedInputs(inputs);

		try {
			propertyGraphOutputValue = propertyComputeGraph.evaluate(inputs).at(0).template value<T>();
		}
		catch(const ComputeGraph::EvaluationException&) {
			propertyGraphOutputValue = std::nullopt;
		}

		recalculateResult();
	}

	void curve(const Curve<T>& curve) {
		propertyCurve = curve;
		recalculateResult();
	}
	const Curve<T>& curve() const {
		return propertyCurve;
	}
	const Curve<T>& resultCurve() const {
		return propertyComputedCurve;
	}

	void keyframes(const std::vector<typename Curve<T>::Point>& pointList) {
		propertyCurve.points(pointList);
		recalculateResult();
	}
	void keyframes(const typename Curve<T>::Point* pointList, std::size_t count) {
		propertyCurve.points(pointList, count);
		recalculateResult();
	}

	void addKeyframe(float_t position, const T& value) {
		propertyCurve.addPoint(position, value);
		recalculateResult();
	}
	void removeKeyframe(std::size_t index) {
		propertyCurve.removePoint(index);
		recalculateResult();
	}
	void setKeyframeValue(std::size_t index, const T& value) {
		propertyCurve.setPointValue(index, value);
		recalculateResult();
	}
	void setKeyframePosition(std::size_t index, float_t position) {
		propertyCurve.setPointPosition(index, position);
		recalculateResult();
	}
	void clearKeyframes() {
		propertyCurve.clear();
		recalculateResult();
	}

	std::size_t keyframeCount() const {
		return propertyCurve.pointCount();
	}

	const typename Curve<T>::Point& keyframe(std::size_t index) const {
		return propertyCurve.point(index);
	}
	const std::vector<typename Curve<T>::Point>& keyframes() const {
		return propertyCurve.points();
	}

	std::optional<std::size_t> keyframeIndex(float_t position, float_t epsilon = 0.001) const {
		return propertyCurve.pointIndex(position, epsilon);
	}

	template <typename UnaryFunc>
	void forEachKeyframe(UnaryFunc func) {
		propertyCurve.forEachPoint(func);
		recalculateResult();
	}

	void keyframeInterpolation(CurveInterpolation method) {
		propertyCurve.interpolation(method);
		recalculateResult();
	}
	CurveInterpolation keyframeInterpolation() const {
		return propertyCurve.interpolation();
	}

	void computeGraph(const ComputeGraph& graph) {
		propertyComputeGraph = graph;
	}
	const ComputeGraph& computeGraph() const {
		return propertyComputeGraph;
	}

	void computeOutputOperation(ComputeOutputOperation operation) {
		propertyOutputOperation = operation;
		recalculateResult();
	}
	ComputeOutputOperation computeOutputOperation() const {
		return propertyOutputOperation;
	}

	void computeOutputTarget(ComputeOutputTarget target) {
		propertyOutputTarget = target;
		recalculateResult();
	}
	ComputeOutputTarget computeOutputTarget() const {
		return propertyOutputTarget;
	}

private:
	void recalculateResult() {
		propertyComputedCurve = propertyCurve;

		if(propertyGraphOutputValue) {
			if(propertyOutputTarget.type == ComputeOutputTarget::keyframe) {
				switch(propertyOutputOperation) {
					case ComputeOutputOperation::add:
						propertyComputedCurve.setPointValue(propertyOutputTarget.index, propertyCurve.point(propertyOutputTarget.index).value + propertyGraphOutputValue.value());
						break;
					case ComputeOutputOperation::multiply:
						propertyComputedCurve.setPointValue(propertyOutputTarget.index, propertyCurve.point(propertyOutputTarget.index).value * propertyGraphOutputValue.value());
						break;
					default:
						propertyComputedCurve.setPointValue(propertyOutputTarget.index, propertyGraphOutputValue.value());
						break;
				}
			}
			else {
				switch(propertyOutputOperation) {
					case ComputeOutputOperation::add:
						propertyComputedCurve = moveCurve(propertyComputedCurve, propertyGraphOutputValue.value());
						break;
					case ComputeOutputOperation::multiply:
						propertyComputedCurve = scaleCurve(propertyComputedCurve, propertyGraphOutputValue.value());
						break;
					default:
						propertyComputedCurve = flattenCurve(propertyComputedCurve, propertyGraphOutputValue.value());
						break;
				}
			}
		}
	}

	Curve<T> propertyCurve;

	ComputeGraph propertyComputeGraph;
	ComputeOutputOperation propertyOutputOperation = ComputeOutputOperation::set;
	ComputeOutputTarget propertyOutputTarget = ComputeOutputTarget();

	Curve<T> propertyComputedCurve;
	std::optional<T> propertyGraphOutputValue;
};

template <typename T>
void to_json(nlohmann::ordered_json& j, const AnimatedProperty<T>& property) {
	nlohmann::ordered_json jComputeGraph = nlohmann::ordered_json::object();
	if(!property.computeGraph().empty()) {
		jComputeGraph = property.computeGraph();
	}

	j = nlohmann::ordered_json{
		{ "curve", property.curve() },
		{ "compute_graph", jComputeGraph },
		{ "compute_operation", property.computeOutputOperation() },
		{ "compute_target", property.computeOutputTarget() }
	};
}

template <typename T>
void from_json(const nlohmann::ordered_json& j, AnimatedProperty<T>& property) {
	property = AnimatedProperty<T>(
		j.value("curve", Curve<T>()),
		j.value("compute_graph", ComputeGraph()),
		j.value("compute_operation", ComputeOutputOperation::set),
		j.value("compute_target", ComputeOutputTarget()));
}
}
