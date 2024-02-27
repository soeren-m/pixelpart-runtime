#pragma once

#include "PostProcessingEffectType.h"

namespace pixelpart {
class PostProcessingEffectCollection {
public:
	PostProcessingEffectCollection();
	PostProcessingEffectCollection(const std::vector<PostProcessingEffectType>& effectTypes);

	const PostProcessingEffectType* getEffectType(const std::string& typeName) const;
	const std::vector<PostProcessingEffectType>& getEffectTypes() const;

private:
	std::vector<PostProcessingEffectType> effects;
};

void to_json(nlohmann::ordered_json& j, const PostProcessingEffectCollection& effectCollection);
void from_json(const nlohmann::ordered_json& j, PostProcessingEffectCollection& effectCollection);
}