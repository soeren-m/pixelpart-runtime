#include "IntegrationModifier.h"
#include "../effect/ParticleType.h"
#include "../effect/ParticleEmitter.h"
#include "../effect/ParticleSimulationSpace.h"
#include "../types/Types.h"
#include "../math/MatrixCommon.h"
#include <algorithm>

namespace pixelpart {
void IntegrationModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);
	const ParticleEmitter& particleEmitter = effect->sceneGraph().at<ParticleEmitter>(particleEmitterId);
	
	float_t dt = runtimeContext.deltaTime();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		particles.velocity[p] += particles.force[p] * dt;
	}

	if(particleType.simulationSpace() == ParticleSimulationSpace::local) {
		matrix4_t parentTransform = effect->sceneGraph().globalTransform(particleEmitter.id(), runtimeContext).matrix();
		matrix3_t invParentTransform = matrix3_t(math::inverse(parentTransform));

		for(std::uint32_t p = 0; p < particleCount; p++) {
			particles.position[p] += (invParentTransform * particles.velocity[p]) * dt;
			particles.globalPosition[p] = float3_t(parentTransform * float4_t(particles.position[p], 1.0));
		}
	}
	else {
		for(std::uint32_t p = 0; p < particleCount; p++) {
			particles.position[p] += particles.velocity[p] * dt;
		}

		std::copy(particles.position, particles.position + particleCount, particles.globalPosition);
	}
}

void IntegrationModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {

}
}
