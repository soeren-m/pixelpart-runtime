#include "ForceSolver.h"

namespace pixelpart {
ForceSolver::ForceSolver() {

}

void ForceSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t particleIndex, floatd particleWeight, floatd t, floatd dt) const {
	for(std::size_t f = 0; f < forceFields.size(); f++) {
		const ForceField& forceField = forceFields[f];
		if(forceFieldExclusionSets[f][particleType.id] ||
			t < forceField.lifetimeStart ||
			t > forceField.lifetimeStart + forceField.lifetimeDuration && !forceField.repeat) {
			continue;
		}

		solve(particleType, particles, particleIndex, particleWeight, t, dt, forceField);
	}
}

void ForceSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t particleIndex, floatd particleWeight, floatd t, floatd dt, const ForceField& forceField) const {
	floatd alpha = std::fmod(t - forceField.lifetimeStart, forceField.lifetimeDuration) / forceField.lifetimeDuration;
	vec3d forceFieldCenter = forceField.position.get(alpha);
	vec3d forceFieldSize = forceField.size.get(alpha) * 0.5;
	vec3d forceFieldOrientation = glm::radians(forceField.orientation.get(alpha));
	vec3d forceDirection = glm::radians(forceField.direction.get(alpha));
	floatd forceStrength = forceField.strength.get(alpha);

	vec3d forceVector = vec3d(0.0);
	int32_t gridCellX = 0;
	int32_t gridCellY = 0;
	int32_t gridCellZ = 0;

	switch(forceField.type) {
		case ForceField::Type::point: {
			floatd distance = glm::distance(forceFieldCenter, particles.globalPosition[particleIndex]);

			if(distance < forceFieldSize.x || forceFieldSize.x < 0.0) {
				gridCellX = (forceFieldSize.x > 0.0)
					? static_cast<int32_t>((particles.globalPosition[particleIndex].x - (forceFieldCenter.x - forceFieldSize.x)) / (forceFieldSize.x * 2.0) * static_cast<floatd>(forceField.gridSize[0]))
					: 0;
				gridCellY = (forceFieldSize.x > 0.0)
					? static_cast<int32_t>((particles.globalPosition[particleIndex].y - (forceFieldCenter.y - forceFieldSize.x)) / (forceFieldSize.x * 2.0) * static_cast<floatd>(forceField.gridSize[1]))
					: 0;
				gridCellZ = (forceFieldSize.x > 0.0)
					? static_cast<int32_t>((particles.globalPosition[particleIndex].z - (forceFieldCenter.z - forceFieldSize.x)) / (forceFieldSize.x * 2.0) * static_cast<floatd>(forceField.gridSize[2]))
					: 0;

				forceVector = (distance > 0.0001)
					? glm::normalize(forceFieldCenter - particles.globalPosition[particleIndex]) / std::max(distance, forceFieldSize.x * 0.01)
					: vec3d(0.0);
			}

			break;
		}

		case ForceField::Type::area: {
			vec3d rotatedParticlePosition = forceFieldCenter + vec3d(
				glm::yawPitchRoll(forceFieldOrientation.y, forceFieldOrientation.z, forceFieldOrientation.x) *
				vec4d(particles.globalPosition[particleIndex] - forceFieldCenter, 1.0));

			if(((rotatedParticlePosition.x > forceFieldCenter.x - forceFieldSize.x && rotatedParticlePosition.x < forceFieldCenter.x + forceFieldSize.x) || forceFieldSize.x < 0.0) &&
			((rotatedParticlePosition.y > forceFieldCenter.y - forceFieldSize.y && rotatedParticlePosition.y < forceFieldCenter.y + forceFieldSize.y) || forceFieldSize.y < 0.0) &&
			((rotatedParticlePosition.z > forceFieldCenter.z - forceFieldSize.z && rotatedParticlePosition.z < forceFieldCenter.z + forceFieldSize.z) || forceFieldSize.z < 0.0)) {
				gridCellX = (forceFieldSize.x > 0.0)
					? static_cast<int32_t>((rotatedParticlePosition.x - (forceFieldCenter.x - forceFieldSize.x)) / (forceFieldSize.x * 2.0) * static_cast<floatd>(forceField.gridSize[0]))
					: 0;
				gridCellY = (forceFieldSize.y > 0.0)
					? static_cast<int32_t>((rotatedParticlePosition.y - (forceFieldCenter.y - forceFieldSize.y)) / (forceFieldSize.y * 2.0) * static_cast<floatd>(forceField.gridSize[1]))
					: 0;
				gridCellZ = (forceFieldSize.z > 0.0)
					? static_cast<int32_t>((rotatedParticlePosition.z - (forceFieldCenter.z - forceFieldSize.z)) / (forceFieldSize.z * 2.0) * static_cast<floatd>(forceField.gridSize[2]))
					: 0;

				forceVector = vec3d(glm::yawPitchRoll(forceDirection.y, forceDirection.z, forceDirection.x) * worldUpVector4);
			}

			break;
		}

		default: {
			break;
		}
	}

	uint32_t gridCellIndex = gridCellZ * forceField.gridSize[1] * forceField.gridSize[0] + gridCellY * forceField.gridSize[0] + gridCellX;
	vec3d gridDirectionOffset = glm::radians(forceField.directionVariance * forceField.directionGrid[gridCellIndex]);
	floatd gridStrengthOffset = forceField.strengthVariance * forceField.strengthGrid[gridCellIndex];

	forceVector = vec3d(glm::yawPitchRoll(gridDirectionOffset.y, gridDirectionOffset.z, gridDirectionOffset.x) * vec4d(forceVector, 0.0));
	forceVector *= forceStrength + gridStrengthOffset;
	forceVector *= particleWeight;
	particles.force[particleIndex] += forceVector;
}

void ForceSolver::update(const Effect* effect) {
	if(!effect) {
		forceFields.clear();
		forceFieldExclusionSets.clear();
		return;
	}

	forceFields = effect->forceFields.get();

	forceFieldExclusionSets.resize(forceFields.size());
	for(std::size_t f = 0; f < forceFields.size(); f++) {
		forceFieldExclusionSets[f].reset();
		for(uint32_t particleTypeId : forceFields[f].exclusionList) {
			forceFieldExclusionSets[f].set(particleTypeId);
		}
	}
}
}