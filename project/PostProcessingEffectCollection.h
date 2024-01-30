#pragma once

#include "PostProcessingEffectType.h"

namespace pixelpart {
class PostProcessingEffectCollection {
public:
	PostProcessingEffectCollection();
	PostProcessingEffectCollection(const std::vector<PostProcessingEffectType>& effectTypes, const std::string& shTemplate);

	std::string build(const std::string& typeName) const;

	const PostProcessingEffectType* getEffectType(const std::string& typeName) const;
	const std::vector<PostProcessingEffectType>& getEffectTypes() const;

	const std::string& getShaderTemplate() const;

private:
	static std::string replace(std::string str, const std::string& to, const std::string& from);

	std::vector<PostProcessingEffectType> effects;
	std::string shaderTemplate;
};

void to_json(nlohmann::ordered_json& j, const PostProcessingEffectCollection& effectCollection);
void from_json(const nlohmann::ordered_json& j, PostProcessingEffectCollection& effectCollection);
}