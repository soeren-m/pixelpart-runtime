#pragma once

#include "LightSource.h"

namespace pixelpart {
class PointLightSource : public LightSource {
public:
	PointLightSource() = default;
	PointLightSource(id_t ownId, id_t parentId = id_t());

	virtual LightSourceType lightSourceType() const override;

protected:
	virtual Node* cloneImpl() const override;
};

void to_json(nlohmann::ordered_json& j, const PointLightSource& lightSource);
void from_json(const nlohmann::ordered_json& j, PointLightSource& lightSource);
}