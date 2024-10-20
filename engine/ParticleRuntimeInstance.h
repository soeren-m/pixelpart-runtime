#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "ParticleCollection.h"

namespace pixelpart {
class ParticleRuntimeInstance {
public:

	id_t emitterId() const;
	id_t typeId() const;

	ParticleCollection& particles();
	const ParticleCollection& particles() const;

	float_t& emissionCount();

private:
	id_t particleEmitterId;
	id_t particleTypeId;

	ParticleCollection particles;
	float_t particleEmissionCount = 0.0;
};
}