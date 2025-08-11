#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class ComputeOutputOperation : std::uint32_t {
	set = 0,
	add = 1,
	multiply = 2
};

template <typename T>
T applyComputeOutputOperation(T v1, T v2, ComputeOutputOperation op) {
	switch(op) {
		case ComputeOutputOperation::add:
			return v1 + v2;
		case ComputeOutputOperation::multiply:
			return v1 * v2;
		default:
			return v2;
	}
}

bool applyComputeOutputOperation(bool v1, bool v2, ComputeOutputOperation op);

NLOHMANN_JSON_SERIALIZE_ENUM(ComputeOutputOperation, {
	{ ComputeOutputOperation::set, "set" },
	{ ComputeOutputOperation::add, "add" },
	{ ComputeOutputOperation::multiply, "multiply" }
})
}
