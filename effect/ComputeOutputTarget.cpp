#include "ComputeOutputTarget.h"
#include "../common/Json.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ComputeOutputTarget& target) {
	j = nlohmann::ordered_json{
		{ "type", target.type },
		{ "index", target.index }
	};
}
void from_json(const nlohmann::ordered_json& j, ComputeOutputTarget& target) {
	target = ComputeOutputTarget();

	fromJson(target.type, j, "type");
	fromJson(target.index, j, "index");
}
}