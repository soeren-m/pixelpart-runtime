#pragma once

#include "../common/Types.h"
#include "../common/Curve.h"
#include "../common/VariantValue.h"
#include "../computegraph/ComputeGraph.h"
#include "../computegraph/OutputComputeNode.h"
#include "ComputeOutputOperation.h"
#include "ComputeOutputTarget.h"
#include "../json/json.hpp"
#include <vector>
#include <unordered_map>

namespace pixelpart {
template <typename T>
class AnimatedProperty {
public:
	AnimatedProperty() {
		propertyComputeGraph.addNode<OutputComputeNode>();
		refresh();
	}
	AnimatedProperty(const T& initialValue) : propertyCurve(initialValue) {
		propertyComputeGraph.addNode<OutputComputeNode>();
		refresh();
	}
	AnimatedProperty(float_t initialPosition, const T& initialValue) : propertyCurve(initialPosition, initialValue) {
		propertyComputeGraph.addNode<OutputComputeNode>();
		refresh();
	}
	AnimatedProperty(const Curve<T>& initialCurve, const ComputeGraph& graph, ComputeOutputOperation outputOp, ComputeOutputTarget outputTgt) :
		propertyCurve(initialCurve),
		propertyComputeGraph(graph),
		outputOperation(outputOp),
		outputTarget(outputTgt) {
		refresh();
	}

	T operator()(float_t position = 0.0) const {
		return computedCurve.at(position);
	}

	T at(float_t position = 0.0) const {
		return computedCurve.at(position);
	}

	void refresh(const ComputeGraph::InputSet& inputs) {
		if(propertyComputeGraph.empty()) {
			useGraphOutput = false;
			refresh();

			return;
		}

		propertyComputeGraph.unlinkRemovedInputs(inputs);

		try {
			graphOutputValue = propertyComputeGraph.evaluate(inputs).at(0u).template value<T>();

			useGraphOutput = true;
			refresh();
		}
		catch(const ComputeGraph::EvaluationException&) {
			useGraphOutput = false;
			refresh();
		}
	}

	void curve(const Curve<T>& curve) {
		propertyCurve = curve;
		refresh();
	}
	const Curve<T>& curve() const {
		return propertyCurve;
	}
	const Curve<T>& getComputedCurve() const {
		return computedCurve;
	}

	void computeGraph(const ComputeGraph& graph) {
		propertyComputeGraph = graph;
	}
	const ComputeGraph& computeGraph() const {
		return propertyComputeGraph;
	}

	void computeOutputOperation(ComputeOutputOperation operation) {
		outputOperation = operation;
		refresh();
	}
	ComputeOutputOperation computeOutputOperation() const {
		return outputOperation;
	}

	void computeOutputTarget(ComputeOutputTarget target) {
		outputTarget = target;
		refresh();
	}
	ComputeOutputTarget computeOutputTarget() const {
		return outputTarget;
	}

	void setPoints(const std::vector<typename Curve<T>::Point>& pointList) {
		propertyCurve.setPoints(pointList);
		refresh();
	}
	void setPoints(const typename Curve<T>::Point* pointList, std::size_t numPoints) {
		propertyCurve.setPoints(pointList, numPoints);
		refresh();
	}
	template <typename IntT>
	void setPointsOrdered(const float_t* positionList, const T* valueList, const IntT* order, IntT numPoints) {
		propertyCurve.setPointsOrdered(positionList, valueList, order, numPoints);
		refresh();
	}

	void addPoint(float_t position, const T& value) {
		propertyCurve.addPoint(position, value);
		refresh();
	}
	void removePoint(std::size_t index) {
		propertyCurve.removePoint(index);
		refresh();
	}
	void setPoint(std::size_t index, const T& value) {
		propertyCurve.setPoint(index, value);
		refresh();
	}
	void setPointPosition(std::size_t index, float_t position) {
		propertyCurve.setPointPosition(index, position);
		refresh();
	}
	void clear() {
		propertyCurve.clear();
		refresh();
	}

	bool containsPoints() const {
		return propertyCurve.containsPoints();
	}
	std::size_t pointCount() const {
		return propertyCurve.pointCount();
	}
	/*std::vector<typename Curve<T>::Point>& points() {
		return propertyCurve.points();
	}*/
	const std::vector<typename Curve<T>::Point>& points() const {
		return propertyCurve.points();
	}
	const typename Curve<T>::Point& point(std::size_t index) const {
		return propertyCurve.point(index);
	}

	std::ptrdiff_t pointIndex(float_t position, float_t epsilon = 0.001) const {
		return propertyCurve.pointIndex(position, epsilon);
	}

	void interpolation(CurveInterpolation method) {
		propertyCurve.interpolation(method);
		refresh();
	}
	CurveInterpolation interpolation() const {
		return propertyCurve.interpolation();
	}

	void enableAdaptiveCache() {
		propertyCurve.enableAdaptiveCache();
		refresh();
	}
	void enableFixedCache(std::size_t size) {
		propertyCurve.enableFixedCache(size);
		refresh();
	}
	const std::vector<T>& cache() const {
		return propertyCurve.cache();
	}

	void refreshCache() {
		propertyCurve.refreshCache();
		refresh();
	}

private:
	void refresh() {
		computedCurve = propertyCurve;

		if(useGraphOutput) {
			if(outputTarget.type == ComputeOutputTarget::keyframe) {
				switch(outputOperation) {
					case ComputeOutputOperation::add:
						computedCurve.setPoint(outputTarget.index, propertyCurve.point(outputTarget.index).value + graphOutputValue);
						break;
					case ComputeOutputOperation::multiply:
						computedCurve.setPoint(outputTarget.index, propertyCurve.point(outputTarget.index).value * graphOutputValue);
						break;
					default:
						computedCurve.setPoint(outputTarget.index, graphOutputValue);
						break;
				}
			}
			else {
				switch(outputOperation) {
					case ComputeOutputOperation::add:
						moveCurve(computedCurve, graphOutputValue);
						break;
					case ComputeOutputOperation::multiply:
						scaleCurve(computedCurve, graphOutputValue);
						break;
					default:
						flattenCurve(computedCurve, graphOutputValue);
						break;
				}
			}
		}
	}

	Curve<T> propertyCurve;

	ComputeGraph propertyComputeGraph;
	ComputeOutputOperation outputOperation = ComputeOutputOperation::set;
	ComputeOutputTarget outputTarget = ComputeOutputTarget();

	Curve<T> computedCurve;
	T graphOutputValue = T();
	bool useGraphOutput = false;
};

template <typename T>
void to_json(nlohmann::ordered_json& j, const AnimatedProperty<T>& property) {
	j = nlohmann::ordered_json{
		{ "curve", property.curve() },
		{ "compute_graph", property.computeGraph() },
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