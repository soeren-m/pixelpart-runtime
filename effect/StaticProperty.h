#pragma once

#include "ComputeGraph.h"
#include "OutputComputeNode.h"
#include "ComputeOutputOperation.h"
#include "../common/Types.h"
#include "../common/VariantValue.h"
#include "../json/json.hpp"

namespace pixelpart {
template <typename T>
class StaticProperty {
public:
	StaticProperty() {
		propertyComputeGraph.addNode<OutputComputeNode>();
		recalculateResult();
	}
	StaticProperty(const T& initialValue) : propertyBaseValue(initialValue) {
		propertyComputeGraph.addNode<OutputComputeNode>();
		recalculateResult();
	}
	StaticProperty(const T& initialValue, const ComputeGraph& graph, ComputeOutputOperation outputOp) :
		propertyBaseValue(initialValue),
		propertyComputeGraph(graph),
		outputOperation(outputOp) {
		if(propertyComputeGraph.nodes().size() == 0) {
			propertyComputeGraph.addNode<OutputComputeNode>();
		}

		recalculateResult();
	}

	T operator()() const {
		return computedValue;
	}

	T value() const {
		return computedValue;
	}

	void input(const ComputeGraph::InputSet& inputs) {
		if(propertyComputeGraph.empty()) {
			useGraphOutput = false;
			recalculateResult();

			return;
		}

		propertyComputeGraph.unlinkRemovedInputs(inputs);

		try {
			graphOutputValue = propertyComputeGraph.evaluate(inputs).at(0).template value<T>();

			useGraphOutput = true;
			recalculateResult();
		}
		catch(const ComputeGraph::EvaluationException&) {
			useGraphOutput = false;
			recalculateResult();
		}
	}

	void baseValue(T v) {
		propertyBaseValue = v;
		recalculateResult();
	}
	T baseValue() const {
		return propertyBaseValue;
	}

	void computeGraph(const ComputeGraph& graph) {
		propertyComputeGraph = graph;
	}
	const ComputeGraph& computeGraph() const {
		return propertyComputeGraph;
	}

	void computeOutputOperation(ComputeOutputOperation operation) {
		outputOperation = operation;
		recalculateResult();
	}
	ComputeOutputOperation computeOutputOperation() const {
		return outputOperation;
	}

private:
	void recalculateResult() {
		if(useGraphOutput) {
			computedValue = applyComputeOutputOperation(propertyBaseValue, graphOutputValue, outputOperation);
		}
		else {
			computedValue = propertyBaseValue;
		}
	}

	T propertyBaseValue = T();

	ComputeGraph propertyComputeGraph;
	ComputeOutputOperation outputOperation = ComputeOutputOperation::set;

	T computedValue = T();
	T graphOutputValue = T();
	bool useGraphOutput = false;
};

template <typename T>
void to_json(nlohmann::ordered_json& j, const StaticProperty<T>& property) {
	nlohmann::ordered_json jComputeGraph = nlohmann::ordered_json::object();
	if(!property.computeGraph().empty()) {
		jComputeGraph = property.computeGraph();
	}

	j = nlohmann::ordered_json{
		{ "value", property.baseValue() },
		{ "compute_graph", jComputeGraph },
		{ "compute_operation", property.computeOutputOperation() },
	};
}

template <typename T>
void from_json(const nlohmann::ordered_json& j, StaticProperty<T>& property) {
	property = StaticProperty<T>(
		j.value("value", T()),
		j.value("compute_graph", ComputeGraph()),
		j.value("compute_operation", ComputeOutputOperation::set));
}
}
