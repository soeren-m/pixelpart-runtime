#pragma once

#include "ComputeGraph.h"
#include "OutputComputeNode.h"
#include "ComputeOutputOperation.h"
#include "../common/Types.h"
#include "../common/VariantValue.h"
#include "../json/json.hpp"
#include <optional>

namespace pixelpart {
template <typename T>
class StaticProperty {
public:
	StaticProperty() {
		propertyComputeGraph.addNode<OutputComputeNode>();
		recalculateResult();
	}
	StaticProperty(const T& baseValue) : propertyBaseValue(baseValue) {
		propertyComputeGraph.addNode<OutputComputeNode>();
		recalculateResult();
	}
	StaticProperty(const T& baseValue, const ComputeGraph& graph, ComputeOutputOperation outputOp) :
		propertyBaseValue(baseValue),
		propertyComputeGraph(graph),
		propertyOutputOperation(outputOp) {
		if(propertyComputeGraph.nodes().size() == 0) {
			propertyComputeGraph.addNode<OutputComputeNode>();
		}

		recalculateResult();
	}

	T operator()() const {
		return propertyComputedValue;
	}

	T value() const {
		return propertyComputedValue;
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
		propertyOutputOperation = operation;
		recalculateResult();
	}
	ComputeOutputOperation computeOutputOperation() const {
		return propertyOutputOperation;
	}

private:
	void recalculateResult() {
		if(propertyGraphOutputValue) {
			propertyComputedValue = applyComputeOutputOperation(propertyBaseValue, propertyGraphOutputValue.value(), propertyOutputOperation);
		}
		else {
			propertyComputedValue = propertyBaseValue;
		}
	}

	T propertyBaseValue = T();

	ComputeGraph propertyComputeGraph;
	ComputeOutputOperation propertyOutputOperation = ComputeOutputOperation::set;

	T propertyComputedValue = T();
	std::optional<T> propertyGraphOutputValue;
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
