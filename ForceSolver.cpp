#include "ForceSolver.h"

namespace pixelpart {
ForceSolver::ForceSolver() {

}

void ForceSolver::solve(uint32_t emitterId, ParticleDataPointer& particles, uint32_t p, floatd particleWeight, floatd t, floatd dt) const {
	for(const ForceField& force : forces) {
		if(!force.emitterMask[emitterId] || t < force.lifetimeStart || (t > force.lifetimeStart + force.lifetimeDuration && !force.repeat)) {
			continue;
		}

		solve(particles, p, particleWeight, t, dt, force);
	}
}

void ForceSolver::solve(ParticleDataPointer& particles, uint32_t p, floatd particleWeight, floatd t, floatd dt, const ForceField& force) const {
	const floatd alpha = std::fmod(t - force.lifetimeStart, force.lifetimeDuration) / force.lifetimeDuration;
	const vec2d forceCenter = force.motionPath.get(alpha);
	const vec2d forceSize = vec2d(force.width.get(alpha), force.height.get(alpha)) * 0.5;
	const floatd forceOrientation = force.orientation.get(alpha);
	const floatd forceDirection = force.direction.get(alpha);
	const floatd forceStrength = force.strength.get(alpha);
	
	vec2d forceVector = vec2d(0.0);
	int32_t gridCellX = 0;
	int32_t gridCellY = 0;

	switch(force.type) {
		case ForceField::Type::point: {
			floatd d = std::max(glm::distance(forceCenter, particles.globalPosition[p]), forceSize.x * 0.01);

			if((d < forceSize.x || forceSize.x < 0.0) && forceCenter != particles.globalPosition[p]) {
				gridCellX = (forceSize.x > 0.0)
					? static_cast<int32_t>((particles.globalPosition[p].x - (forceCenter.x - forceSize.x)) / (forceSize.x * 2.0) * static_cast<floatd>(force.gridSize[0]))
					: 0;
				gridCellY = (forceSize.x > 0.0)
					? static_cast<int32_t>((particles.globalPosition[p].y - (forceCenter.y - forceSize.x)) / (forceSize.x * 2.0) * static_cast<floatd>(force.gridSize[1]))
					: 0;

				forceVector = glm::normalize(forceCenter - particles.globalPosition[p]) / d;
			}

			break;
		}

		case ForceField::Type::area: {
			vec2d rotatedPosition = forceCenter + glm::rotate(particles.globalPosition[p] - forceCenter, -glm::radians(forceOrientation));

			if(((rotatedPosition.x > forceCenter.x - forceSize.x && rotatedPosition.x < forceCenter.x + forceSize.x) || forceSize.x < 0.0) &&
			((rotatedPosition.y > forceCenter.y - forceSize.y && rotatedPosition.y < forceCenter.y + forceSize.y) || forceSize.y < 0.0)) {
				gridCellX = (forceSize.x > 0.0)
					? static_cast<int32_t>((rotatedPosition.x - (forceCenter.x - forceSize.x)) / (forceSize.x * 2.0) * static_cast<floatd>(force.gridSize[0]))
					: 0;
				gridCellY = (forceSize.y > 0.0)
					? static_cast<int32_t>((rotatedPosition.y - (forceCenter.y - forceSize.y)) / (forceSize.y * 2.0) * static_cast<floatd>(force.gridSize[1]))
					: 0;

				forceVector = glm::rotate(vec2d(0.0, 1.0), glm::radians(forceDirection));
			}

			break;
		}

		default: {
			break;
		}
	}

	forceVector = glm::rotate(forceVector, glm::radians(force.directionVariance * force.directionGrid[gridCellY * force.gridSize[0] + gridCellX]));
	forceVector *= forceStrength + force.strengthVariance * force.strengthGrid[gridCellY * force.gridSize[0] + gridCellX];
	forceVector *= particleWeight;
	particles.force[p] += forceVector;
}

void ForceSolver::update(const std::vector<ForceField>& allForces) {
	forces = allForces;
}
}