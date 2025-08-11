#include "PointLightSource.h"

namespace pixelpart {
PointLightSource::PointLightSource(id_t ownId, id_t parentId) : LightSource(ownId, parentId) {

}

LightSourceType PointLightSource::lightSourceType() const {
	return LightSourceType::point;
}

Node* PointLightSource::cloneImpl() const {
	return new PointLightSource(*this);
}

void to_json(nlohmann::ordered_json& j, const PointLightSource& lightSource) {
	to_json(j, static_cast<const LightSource&>(lightSource));

	j.update(nlohmann::ordered_json{
		{ "light_source_type", LightSourceType::point }
	});
}
void from_json(const nlohmann::ordered_json& j, PointLightSource& lightSource) {
	lightSource = PointLightSource(j.at("id").get<id_t>());
	from_json(j, static_cast<LightSource&>(lightSource));
}
}
