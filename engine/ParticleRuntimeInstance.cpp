#include "ParticleRuntimeInstance.h"

namespace pixelpart {
ParticleRuntimeInstance::ParticleRuntimeInstance(id_t emitterId, id_t typeId, std::uint32_t particleCapacity) :
	instanceEmitterId(emitterId), instanceTypeId(typeId), instanceParticles(particleCapacity) {

}

void ParticleRuntimeInstance::reset() {
	instanceParticles.clear();
	instanceEmissionCount = 0.0;
	instanceEmitterGridIndex = 0;
}

id_t ParticleRuntimeInstance::emitterId() const {
	return instanceEmitterId;
}
id_t ParticleRuntimeInstance::typeId() const {
	return instanceTypeId;
}
ParticleRuntimeId ParticleRuntimeInstance::id() const {
	return ParticleRuntimeId(instanceEmitterId, instanceTypeId);
}

ParticleCollection& ParticleRuntimeInstance::particles() {
	return instanceParticles;
}
const ParticleCollection& ParticleRuntimeInstance::particles() const {
	return instanceParticles;
}

float_t& ParticleRuntimeInstance::emissionCount() {
	return instanceEmissionCount;
}
float_t ParticleRuntimeInstance::emissionCount() const {
	return instanceEmissionCount;
}

std::uint32_t& ParticleRuntimeInstance::emitterGridIndex() {
	return instanceEmitterGridIndex;
}
std::uint32_t ParticleRuntimeInstance::emitterGridIndex() const {
	return instanceEmitterGridIndex;
}
}
