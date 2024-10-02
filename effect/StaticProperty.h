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
		propertyComputeGraph.addNode<OutputComputeNode>();
		refresh();
	}
	StaticProperty(const T& initialValue) : propertyBaseValue(initialValue) {
		propertyComputeGraph.addNode<OutputComputeNode>();
		refresh();
	}
	StaticProperty(const T& initialValue, const ComputeGraph& graph, ComputeOutputOperation outputOp) :
		propertyBaseValue(initialValue),
		propertyComputeGraph(graph),
		outputOperation(outputOp) {
		refresh();
	}

	T operator()() const {
		return computedValue;
	}

	T value() const {
		return computedValue;
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

	void baseValue(T v) {
		propertyBaseValue = v;
		refresh();
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
		refresh();
	}
	ComputeOutputOperation computeOutputOperation() const {
		return outputOperation;
	}

private:
	void refresh() {
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