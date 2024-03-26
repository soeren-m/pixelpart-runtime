#pragma once

#include "ComputeOutputOperation.h"
#include "ComputeOutputTarget.h"
#include "../computegraph/ComputeGraph.h"
#include "../computegraph/OutputComputeNode.h"
#include "../common/Curve.h"

namespace pixelpart {
template <typename T>
class AnimatedProperty {
public:
	AnimatedProperty() {
		computeGraph.addNode<OutputComputeNode>();
		refresh();
	}
	AnimatedProperty(const T& initialValue) : curve(initialValue) {
		computeGraph.addNode<OutputComputeNode>();
		refresh();
	}
	AnimatedProperty(float_t initialPosition, const T& initialValue) : curve(initialPosition, initialValue) {
		computeGraph.addNode<OutputComputeNode>();
		refresh();
	}
	AnimatedProperty(const Curve<T>& initialCurve, const ComputeGraph& graph, ComputeOutputOperation outputOperation, ComputeOutputTarget outputTarget) :
		curve(initialCurve),
		computeGraph(graph),
		computeOutputOperation(outputOperation),
		computeOutputTarget(outputTarget) {
		refresh();
	}

	T operator()(float_t position = 0.0) const {
		return computedCurve.get(position);
	}

	T get(float_t position = 0.0) const {
		return computedCurve.get(position);
	}

	void refresh(const std::unordered_map<uint32_t, VariantValue>& inputs) {
		if(computeGraph.isEmpty()) {
			useGraphOutput = false;
			refresh();

			return;
		}

		computeGraph.unlinkRemovedInputs(inputs);

		try {
			graphOutputValue = computeGraph.evaluate(inputs).at(0u).get<T>();

			useGraphOutput = true;
			refresh();
		}
		catch(const ComputeGraph::EvaluationException&) {
			useGraphOutput = false;
			refresh();
		}
	}

	void refresh() {
		computedCurve = curve;

		if(useGraphOutput) {
			if(computeOutputTarget.type == ComputeOutputTarget::keyframe) {
				switch(computeOutputOperation) {
					case ComputeOutputOperation::add:
						computedCurve.setPoint(computeOutputTarget.index, curve.getPoint(computeOutputTarget.index).value + graphOutputValue);
						break;
					case ComputeOutputOperation::multiply:
						computedCurve.setPoint(computeOutputTarget.index, curve.getPoint(computeOutputTarget.index).value * graphOutputValue);
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
		curve = c;
		refresh();
	}
	Curve<T>& getCurve() {
		return curve;
	}
	const Curve<T>& getCurve() const {
		return curve;
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
		curve.setPoints(pointList);
		refresh();
	}
	void setPoints(const typename Curve<T>::Point* pointList, std::size_t numPoints) {
		curve.setPoints(pointList, numPoints);
		refresh();
	}
	template <typename IntT>
	void setPointsOrdered(const float_t* positionList, const T* valueList, const IntT* order, IntT numPoints) {
		curve.setPointsOrdered(positionList, valueList, order, numPoints);
		refresh();
	}

	void addPoint(float_t position, const T& value) {
		curve.addPoint(position, value);
		refresh();
	}
	void removePoint(std::size_t index) {
		curve.removePoint(index);
		refresh();
	}
	void setPoint(std::size_t index, const T& value) {
		curve.setPoint(index, value);
		refresh();
	}
	void setPointPosition(std::size_t index, float_t position) {
		curve.setPointPosition(index, position);
		refresh();
	}
	void clear() {
		curve.clear();
		refresh();
	}

	bool containsPoints() const {
		return curve.containsPoints();
	}
	std::size_t getNumPoints() const {
		return curve.getNumPoints();
	}
	std::vector<typename Curve<T>::Point>& getPoints() {
		return curve.getPoints();
	}
	const std::vector<typename Curve<T>::Point>& getPoints() const {
		return curve.getPoints();
	}
	const typename Curve<T>::Point& getPoint(std::size_t index) const {
		return curve.getPoint(index);
	}

	std::ptrdiff_t getPointIndex(float_t position, float_t epsilon = 0.001) const {
		return curve.getPointIndex(position, epsilon);
	}

	void setInterpolation(CurveInterpolation method) {
		curve.setInterpolation(method);
		refresh();
	}
	CurveInterpolation getInterpolation() const {
		return curve.getInterpolation();
	}

	void enableAdaptiveCache() {
		curve.enableAdaptiveCache();
		refresh();
	}
	void enableFixedCache(std::size_t size) {
		curve.enableFixedCache(size);
		refresh();
	}
	const std::vector<T>& getCache() const {
		return curve.getCache();
	}

	void refreshCache() {
		curve.refreshCache();
		refresh();
	}

private:
	Curve<T> curve;

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
	Curve<T> curve;
	if(j.contains("curve")) {
		curve = j.at("curve").get<Curve<T>>();
	}

	ComputeGraph graph;
	if(j.contains("compute_graph")) {
		graph = j.at("compute_graph").get<ComputeGraph>();
	}

	ComputeOutputOperation outputOperation = ComputeOutputOperation::set;
	if(j.contains("compute_operation")) {
		outputOperation = j.at("compute_operation").get<ComputeOutputOperation>();
	}

	ComputeOutputTarget outputTarget;
	if(j.contains("compute_target")) {
		outputTarget = j.at("compute_target").get<ComputeOutputTarget>();
	}

	property = AnimatedProperty<T>(curve, graph, outputOperation, outputTarget);
}
}