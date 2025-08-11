#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "../effect/ParticleRuntimeId.h"
#include "ParticleCollection.h"

namespace pixelpart {
class ParticleRuntimeInstance {
public:
	ParticleRuntimeInstance(id_t emitterId, id_t typeId, std::uint32_t particleCapacity);

	void reset();

	id_t emitterId() const;
	id_t typeId() const;
	ParticleRuntimeId id() const;

	ParticleCollection& particles();
	const ParticleCollection& particles() const;

	float_t& emissionCount();
	float_t emissionCount() const;

	std::uint32_t& emitterGridIndex();
	std::uint32_t emitterGridIndex() const;

private:
	id_t instanceEmitterId;
	id_t instanceTypeId;

	ParticleCollection instanceParticles;
	float_t instanceEmissionCount = 0.0;
	std::uint32_t instanceEmitterGridIndex = 0;
};
}
