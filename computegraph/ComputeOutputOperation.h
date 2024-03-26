#pragma once

#include "../common/Types.h"

namespace pixelpart {
// TODO: does not belong in this folder
enum class ComputeOutputOperation : uint32_t {
	set = 0,
	add = 1,
	multiply = 2
};

NLOHMANN_JSON_SERIALIZE_ENUM(ComputeOutputOperation, {
	{ ComputeOutputOperation::set, "set" },
	{ ComputeOutputOperation::add, "add" },
	{ ComputeOutputOperation::multiply, "multiply" }
})
}