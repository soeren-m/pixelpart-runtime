#pragma once

#include "ComputeOutputOperation.h"
#include "../computegraph/ComputeGraph.h"
#include "../computegraph/OutputComputeNode.h"
#include "../common/Json.h"

namespace pixelpart {
template <typename T>
class StaticProperty {
public:
	StaticProperty() {
		computeGraph.addNode<OutputComputeNode>();
		refresh();
	}
	StaticProperty(const T& initialValue) : value(initialValue) {
		computeGraph.addNode<OutputComputeNode>();
		refresh();
	}
	StaticProperty(const T& initialValue, const ComputeGraph& graph, ComputeOutputOperation outputOperation) :
		value(initialValue),
		computeGraph(graph),
		computeOutputOperation(outputOperation) {
		refresh();
	}

	T operator()() const {
		return computedValue;
	}

	T get() const {
		return computedValue;
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
		if(useGraphOutput) {
			computedValue = applyComputeOutputOperation(value, graphOutputValue, computeOutputOperation);
		}
		else {
			computedValue = value;
		}
	}

	void setValue(T v) {
		value = v;
		refresh();
	}
	T getValue() const {
		return value;
	}

	void setComputeGraph(const ComputeGraph& graph) {
		computeGraph = graph;
	}
	void setComputeOutputOperation(ComputeOutputOperation operation) {
		computeOutputOperation = operation;
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

private:
	T value = T();

	ComputeGraph computeGraph;
	ComputeOutputOperation computeOutputOperation = ComputeOutputOperation::set;

	T computedValue = T();
	T graphOutputValue = T();
	bool useGraphOutput = false;
};

template <typename T>
void to_json(nlohmann::ordered_json& j, const StaticProperty<T>& property) {
	j = nlohmann::ordered_json{
		{ "value", property.getValue() },
		{ "compute_graph", property.getComputeGraph() },
		{ "compute_operation", property.getComputeOutputOperation() },
	};
}

template <typename T>
void from_json(const nlohmann::ordered_json& j, StaticProperty<T>& property) {
	T value = T();
	fromJson(value, j, "value");

	ComputeGraph graph;
	fromJson(graph, j, "compute_graph");

	ComputeOutputOperation outputOperation = ComputeOutputOperation::set;
	fromJson(outputOperation, j, "compute_operation");

	property = StaticProperty<T>(value, graph, outputOperation);
}
}