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
	bool effect3d,
	pcg32& rng) {
	float3_t spawnDirection = worldUpVector3;
	switch(directionMode) {
		case ParticleEmitter::DirectionMode::outwards:
			spawnDirection = particleSpawnPosition != float3_t(0.0) ? math::normalize(particleSpawnPosition) : worldUpVector3;
			break;
		case ParticleEmitter::DirectionMode::inwards:
			spawnDirection = particleSpawnPosition != float3_t(0.0) ? math::normalize(-particleSpawnPosition) : worldUpVector3;
			break;
		case ParticleEmitter::DirectionMode::inherit:
			spawnDirection = parentDirection;
			break;
		case ParticleEmitter::DirectionMode::inherit_inverse:
			spawnDirection = -parentDirection;
			break;
		default:
			break;
	}

	matrix3_t directionMatrix = matrix3_t(math::yawPitchRollRotationMatrix(
		effect3d ? math::radians(direction.y + spread * rng.next(-0.5, 0.5)) : 0.0,
		effect3d ? math::radians(direction.z + spread * rng.next(-0.5, 0.5)) : 0.0,
		math::radians(direction.x + spread * rng.next(-0.5, 0.5))));

	return directionMatrix * spawnDirection;
}
}
