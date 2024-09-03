#include "ComputeOutputTarget.h"

namespace pixelpart {
ComputeOutputTarget::ComputeOutputTarget(Type targetType, uint32_t targetIndex) : type(targetType), index(targetIndex) {

}

void to_json(nlohmann::ordered_json& j, const ComputeOutputTarget& target) {
	j = nlohmann::ordered_json{
		{ "type", target.type },
		{ "index", target.index }
	};
}
void from_json(const nlohmann::ordered_json& j, ComputeOutputTarget& target) {
	target = ComputeOutputTarget(
		j.value("type", ComputeOutputTarget::result),
		j.value("index", 0u));
}
}