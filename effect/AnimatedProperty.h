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

// TODO

namespace pixelpart {
template <typename T>
class AnimatedProperty {
public:
	AnimatedProperty() {
		computeGraph.addNode<OutputComputeNode>();
		refresh();
	}
	AnimatedProperty(const T& initialValue) : propertyCurve(initialValue) {
		computeGraph.addNode<OutputComputeNode>();
		refresh();
	}
	AnimatedProperty(float_t initialPosition, const T& initialValue) : propertyCurve(initialPosition, initialValue) {
		computeGraph.addNode<OutputComputeNode>();
		refresh();
	}
	AnimatedProperty(const Curve<T>& initialCurve, const ComputeGraph& graph, ComputeOutputOperation outputOperation, ComputeOutputTarget outputTarget) :
		propertyCurve(initialCurve),
		computeGraph(graph),
		computeOutputOperation(outputOperation),
		computeOutputTarget(outputTarget) {
		refresh();
	}

	T operator()(float_t position = 0.0) const {
		return computedCurve.at(position);
	}

	T at(float_t position = 0.0) const {
		return computedCurve.at(position);
	}

	void refresh(const std::unordered_map<uint32_t, VariantValue>& inputs) {
		if(computeGraph.isEmpty()) {
			useGraphOutput = false;
			refresh();

			return;
		}

		computeGraph.unlinkRemovedInputs(inputs);

		try {
			graphOutputValue = computeGraph.evaluate(inputs).at(0u).template get<T>();

			useGraphOutput = true;
			refresh();
		}
		catch(const ComputeGraph::EvaluationException&) {
			useGraphOutput = false;
			refresh();
		}
	}

	void refresh() {
		computedCurve = propertyCurve;

		if(useGraphOutput) {
			if(computeOutputTarget.type == ComputeOutputTarget::keyframe) {
				switch(computeOutputOperation) {
					case ComputeOutputOperation::add:
						computedCurve.setPoint(computeOutputTarget.index, propertyCurve.getPoint(computeOutputTarget.index).value + graphOutputValue);
						break;
					case ComputeOutputOperation::multiply:
						computedCurve.setPoint(computeOutputTarget.index, propertyCurve.getPoint(computeOutputTarget.index).value * graphOutputValue);
						break;
					default:
						computedCurve.setPoint(computeOutputTarget.index, graphOutputValue);
						break;
				}
			}
			else {
				switch(computeOutputOperation) {
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

	void setCurve(const Curve<T>& c) {
		propertyCurve = c;
		refresh();
	}
	Curve<T>& getCurve() {
		return propertyCurve;
	}
	const Curve<T>& getCurve() const {
		return propertyCurve;
	}
	const Curve<T>& getComputedCurve() const {
		return computedCurve;
	}

	void setComputeGraph(const ComputeGraph& graph) {
		computeGraph = graph;
	}
	void setComputeOutputOperation(ComputeOutputOperation operation) {
		computeOutputOperation = operation;
		refresh();
	}
	void setComputeOutputTarget(ComputeOutputTarget target) {
		computeOutputTarget = target;
		refresh();
	}
	ComputeGraph& getComputeGraph() {
		return computeGraph;
	}
	const ComputeGraph& getComputeGraph() const {
		return computeGraph;
	}
	ComputeOutputOperation getComputeOutputOperation() const {
		return computeOutputOperation;
	}
	ComputeOutputTarget getComputeOutputTarget() const {
		return computeOutputTarget;
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
	std::size_t getNumPoints() const {
		return propertyCurve.getNumPoints();
	}
	std::vector<typename Curve<T>::Point>& getPoints() {
		return propertyCurve.getPoints();
	}
	const std::vector<typename Curve<T>::Point>& getPoints() const {
		return propertyCurve.getPoints();
	}
	const typename Curve<T>::Point& getPoint(std::size_t index) const {
		return propertyCurve.getPoint(index);
	}

	std::ptrdiff_t getPointIndex(float_t position, float_t epsilon = 0.001) const {
		return propertyCurve.getPointIndex(position, epsilon);
	}

	void setInterpolation(CurveInterpolation method) {
		propertyCurve.setInterpolation(method);
		refresh();
	}
	CurveInterpolation getInterpolation() const {
		return propertyCurve.getInterpolation();
	}

	void enableAdaptiveCache() {
		propertyCurve.enableAdaptiveCache();
		refresh();
	}
	void enableFixedCache(std::size_t size) {
		propertyCurve.enableFixedCache(size);
		refresh();
	}
	const std::vector<T>& getCache() const {
		return propertyCurve.getCache();
	}

	void refreshCache() {
		propertyCurve.refreshCache();
		refresh();
	}

private:
	Curve<T> propertyCurve;

	ComputeGraph computeGraph;
	ComputeOutputOperation computeOutputOperation = ComputeOutputOperation::set;
	ComputeOutputTarget computeOutputTarget = ComputeOutputTarget();

	Curve<T> computedCurve;
	T graphOutputValue = T();
	bool useGraphOutput = false;
};

template <typename T>
void to_json(nlohmann::ordered_json& j, const AnimatedProperty<T>& property) {
	j = nlohmann::ordered_json{
		{ "curve", property.getCurve() },
		{ "compute_graph", property.getComputeGraph() },
		{ "compute_operation", property.getComputeOutputOperation() },
		{ "compute_target", property.getComputeOutputTarget() }
	};
}

template <typename T>
void from_json(const nlohmann::ordered_json& j, AnimatedProperty<T>& property) {
	Curve<T> propertyCurve;
	fromJson(propertyCurve, j, "curve");

	ComputeGraph graph;
	fromJson(graph, j, "compute_graph");

	ComputeOutputOperation outputOperation = ComputeOutputOperation::set;
	fromJson(outputOperation, j, "compute_operation");

	ComputeOutputTarget outputTarget;
	fromJson(outputTarget, j, "compute_target");

	property = AnimatedProperty<T>(propertyCurve, graph, outputOperation, outputTarget);
}
}