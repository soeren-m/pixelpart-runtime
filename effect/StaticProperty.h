#pragma once

#include "../common/Types.h"
#include "../common/VariantValue.h"
#include "../computegraph/ComputeGraph.h"
#include "../computegraph/OutputComputeNode.h"
#include "ComputeOutputOperation.h"
#include "../json/json.hpp"
#include <unordered_map>

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
		return resultValue;
	}

	T get() const {
		return resultValue;
	}

	void refresh(const ComputeGraph::InputSet& inputs) {
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
			resultValue = applyComputeOutputOperation(value, graphOutputValue, computeOutputOperation);
		}
		else {
			resultValue = value;
		}
	}

	void baseValue(T v) {
		value = v;
		refresh();
	}
	T baseValue() const {
		return value;
	}

	void computeGraph(const ComputeGraph& graph) {
		computeGraph = graph;
	}
	const ComputeGraph& computeGraph() const {
		return computeGraph;
	}

	void computeOutputOperation(ComputeOutputOperation operation) {
		computeOutputOperation = operation;
		refresh();
	}
	ComputeOutputOperation computeOutputOperation() const {
		return computeOutputOperation;
	}

private:
	T value = T();

	ComputeGraph computeGraph;
	ComputeOutputOperation computeOutputOperation = ComputeOutputOperation::set;

	T resultValue = T();
	T graphOutputValue = T();
	bool useGraphOutput = false;
};

template <typename T>
void to_json(nlohmann::ordered_json& j, const StaticProperty<T>& property) {
	j = nlohmann::ordered_json{
		{ "value", property.getValue() },
		{ "compute_graph", property.computeGraph() },
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