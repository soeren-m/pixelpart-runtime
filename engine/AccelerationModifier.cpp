#include "AccelerationModifier.h"
#include "../effect/ParticleType.h"
#include "../effect/Curve.h"
#include "../types/Types.h"
#include "../math/Geometry.h"

namespace pixelpart {
void AccelerationModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);

	const Curve<float_t>& particleAccelerationCurve = particleType.acceleration().resultCurve();
	const Curve<float_t>& particleRadialAccelerationCurve = particleType.radialAcceleration().resultCurve();
	float3_t emitterPosition = particleType.simulationSpace() == ParticleSimulationSpace::global
		? effect->sceneGraph().globalTransform(particleEmitterId, runtimeContext).position()
		: float3_t(0.0);

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t forwardDirection = math::safeNormalize(particles.velocity[p]);
		float3_t radialDirection = math::safeNormalize(emitterPosition - particles.position[p]);

		particles.force[p] = forwardDirection * particleAccelerationCurve.at(particles.life[p]);
		particles.force[p] += radialDirection * particleRadialAccelerationCurve.at(particles.life[p]);
	}
}

void AccelerationModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {

}
}
