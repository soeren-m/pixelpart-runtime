#include "PostProcessingEffectCollection.h"
#include "JSONUtil.h"

namespace pixelpart {
namespace {
std::string replace(std::string str, const std::string& to, const std::string& from) {
	if(from.empty()) {
		return str;
	}

	std::size_t pos = 0;
	while((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}

	return str;
}
}

std::string PostProcessingEffectCollection::build(const std::string& typeName) const {
	uint32_t typeIndex = nullId;
	for(uint32_t i = 0; i < static_cast<uint32_t>(effects.size()); i++) {
		if(effects[i].name == typeName) {
			typeIndex = i;
			break;
		}
	}

	if(typeIndex == nullId) {
		return std::string();
	}

	std::string code = shaderTemplate;
	code = replace(code, effects.at(typeIndex).code, "{0}");

	return code;
}

const PostProcessingEffectType* PostProcessingEffectCollection::getEffectType(const std::string& name) const {
	uint32_t typeIndex = nullId;
	for(uint32_t i = 0; i < effects.size(); i++) {
		if(effects[i].name == name) {
			typeIndex = i;
			break;
		}
	}

	if(typeIndex == nullId) {
		return nullptr;
	}

	return &(effects.at(typeIndex));
}

void to_json(nlohmann::ordered_json& j, const PostProcessingEffectCollection& effectCollection) {
	j = nlohmann::ordered_json{
		{ "effects", effectCollection.effects },
		{ "shader_template", effectCollection.shaderTemplate }
	};
}
void from_json(const nlohmann::ordered_json& j, PostProcessingEffectCollection& effectCollection) {
	effectCollection = PostProcessingEffectCollection();

	fromJson(effectCollection.effects, j, "effects");
	fromJson(effectCollection.shaderTemplate, j, "shader_template");
}
}