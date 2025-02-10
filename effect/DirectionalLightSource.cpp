#include "DirectionalLightSource.h"

namespace pixelpart {
DirectionalLightSource::DirectionalLightSource(id_t ownId, id_t parentId) : LightSource(ownId, parentId) {

}

LightSourceType DirectionalLightSource::lightSourceType() const {
	return LightSourceType::directional;
}

Node* DirectionalLightSource::cloneImpl() const {
	return new DirectionalLightSource(*this);
}

void to_json(nlohmann::ordered_json& j, const DirectionalLightSource& lightSource) {
	to_json(j, static_cast<const LightSource&>(lightSource));

	j.update(nlohmann::ordered_json{
		{ "light_source_type", LightSourceType::directional }
	});
}
void from_json(const nlohmann::ordered_json& j, DirectionalLightSource& lightSource) {
	lightSource = DirectionalLightSource(j.at("id").get<id_t>());
	from_json(j, static_cast<LightSource&>(lightSource));
}
}