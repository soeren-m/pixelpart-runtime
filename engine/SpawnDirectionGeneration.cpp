#include "SpawnDirectionGeneration.h"
#include "../effect/Coordinates.h"
#include "../math/Geometry.h"
#include "../math/Trigonometry.h"
#include "../math/Transformation.h"

namespace pixelpart {
float3_t generateParticleSpawnDirection(
	ParticleEmitter::DirectionMode directionMode,
	const float3_t& direction, float_t spread,
	const float3_t& parentDirection,
	const float3_t& particleSpawnPosition,
	const matrix3_t& emitterRotation,
	bool effect3d,
	pcg32& rng) {
	matrix3_t directionMatrix = matrix3_t(math::yawPitchRollRotationMatrix(
		effect3d ? math::radians(direction.y + spread * rng.next(-0.5, 0.5)) : 0.0,
		effect3d ? math::radians(direction.z + spread * rng.next(-0.5, 0.5)) : 0.0,
		math::radians(direction.x + spread * rng.next(-0.5, 0.5))));

	switch(directionMode) {
		case ParticleEmitter::DirectionMode::outwards:
			return directionMatrix *
				((particleSpawnPosition != float3_t(0.0)) ? math::normalize(particleSpawnPosition) : worldUpVector3);
		case ParticleEmitter::DirectionMode::inwards:
			return directionMatrix *
				((particleSpawnPosition != float3_t(0.0)) ? math::normalize(-particleSpawnPosition) : worldUpVector3);
		case ParticleEmitter::DirectionMode::inherit:
			return directionMatrix * parentDirection;
		case ParticleEmitter::DirectionMode::inherit_inverse:
			return directionMatrix * -parentDirection;
		default:
			return emitterRotation * directionMatrix * worldUpVector3;
	}
}
}
