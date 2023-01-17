#include "CollisionSolver.h"

namespace pixelpart {
bool CollisionSolver::isOnSegment(const vec2d& l1, const vec2d& l2, const vec2d& p) {
	floatd A = glm::dot(l2 - l1, p - l1);
	floatd B = glm::dot(l2 - l1, l2 - l1);

	return (A >= 0.0) && (A < B);
}
bool CollisionSolver::getClosestPointOnSegment(const vec2d& l1, const vec2d& l2, const vec2d& p, vec2d& result) {
	floatd A1 = l2.y - l1.y;
	floatd B1 = l1.x - l2.x;
	floatd C1 = (l2.y - l1.y) * l1.x + (l1.x - l2.x) * l1.y;
	floatd C2 = -B1 * p.x + A1 * p.y;
	floatd det = A1 * A1 - -B1 * B1;
	result = (det != 0.0)
		? vec2d(
			(A1 * C1 - B1 * C2) / det,
			(A1 * C2 - -B1 * C1) / det)
		: p;

	return isOnSegment(l1, l2, result);
}
bool CollisionSolver::getIntersectionSegmentSegment(const vec2d& p1, const vec2d& p2, const vec2d& q1, const vec2d& q2, vec2d& result) {
	floatd A1 = p2.y - p1.y;
	floatd B1 = p1.x - p2.x;
	floatd C1 = A1 * p1.x + B1 * p1.y;
	floatd A2 = q2.y - q1.y;
	floatd B2 = q1.x - q2.x;
	floatd C2 = A2 * q1.x + B2 * q1.y;
	floatd det = A1 * B2 - A2 * B1;

	if(det != 0.0) {
		result = vec2d(
			(B2 * C1 - B1 * C2) / det,
			(A1 * C2 - A2 * C1) / det);

		return isOnSegment(p1, p2, result) && isOnSegment(q1, q2, result);
	}

	return false;
}

CollisionSolver::CollisionSolver() : grid(1, 1) {

}

void CollisionSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t particleIndex, floatd particleBounce, floatd particleFriction, floatd t, floatd dt) const {
	GridIndex<int32_t> startIndex = toGridIndex(particles.globalPosition[particleIndex]);
	GridIndex<int32_t> endIndex = toGridIndex(particles.globalPosition[particleIndex] + particles.velocity[particleIndex] * dt + particles.force[particleIndex] * dt * dt);

	if(startIndex.x > endIndex.x) {
		std::swap(startIndex.x, endIndex.x);
	}
	if(startIndex.y > endIndex.y) {
		std::swap(startIndex.y, endIndex.y);
	}

	std::unordered_set<uint32_t> potentialColliders;
	for(int32_t cy = startIndex.y; cy <= endIndex.y; cy++) {
		for(int32_t cx = startIndex.x; cx <= endIndex.x; cx++) {
			findPotentialColliders(potentialColliders, cx, cy, particles.size[particleIndex]);
		}
	}

	for(uint32_t i : potentialColliders) {
		const LineCollider& collider = colliders[i];
		if(collider.exclusionSet[particleType.id] ||
			t < collider.lifetimeStart ||
			t > collider.lifetimeStart + collider.lifetimeDuration && !collider.repeat) {
			continue;
		}

		solve(particleType, particles, particleIndex, particleBounce, particleFriction, t, dt, collider);
	}
}
void CollisionSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t particleIndex, floatd particleBounce, floatd particleFriction, floatd t, floatd dt, const LineCollider& collider) const {
	vec2d closestPoint = vec2d(0.0);
	if(!getClosestPointOnSegment(collider.p1, collider.p2, particles.globalPosition[particleIndex], closestPoint)) {
		return;
	}

	vec2d globalParticlePosition = vec2d(particles.globalPosition[particleIndex]);
	vec2d globalParticleVelocity = vec2d(particles.velocity[particleIndex]);
	vec2d globalParticleForce = vec2d(particles.force[particleIndex]);
	vec2d colliderToParticle = globalParticlePosition - closestPoint;
	floatd distanceSqr = glm::length2(colliderToParticle);
	floatd particleRadius = std::max(std::min(particles.size[particleIndex].x, particles.size[particleIndex].y) * 0.5, 0.01);
	vec2d intersectionPoint = vec2d(0.0);

	if(distanceSqr <= particleRadius * particleRadius) {
		floatd distance = std::sqrt(distanceSqr);
		vec2d normal = glm::normalize(colliderToParticle);
		vec2d reflectedVelocity = glm::reflect(globalParticleVelocity, normal);
		vec2d segmentVector = glm::normalize(collider.p2 - collider.p1);
		floatd slideFactor = glm::dot((globalParticleForce != vec2d(0.0)) ? glm::normalize(globalParticleForce) : vec2d(0.0, 1.0), segmentVector) * glm::length(globalParticleForce) * distance / particleRadius;
		floatd alpha = std::fmod(t - collider.lifetimeStart, collider.lifetimeDuration) / collider.lifetimeDuration;
		floatd bounce = collider.bounce.get(alpha) * particleBounce;
		floatd friction = std::min(collider.friction.get(alpha) * particleFriction, 1.0);

		particles.velocity[particleIndex] = vec3d(reflectedVelocity * bounce, particles.velocity[particleIndex].z);
		particles.velocity[particleIndex] += vec3d(segmentVector * slideFactor * (1.0 - friction), 0.0);
		particles.position[particleIndex] += vec3d(normal * (particleRadius - distance), 0.0);
	}
	else if(getIntersectionSegmentSegment(collider.p1, collider.p2, globalParticlePosition, globalParticlePosition + globalParticleVelocity * dt + globalParticleForce * dt * dt, intersectionPoint)) {
		vec2d normal = glm::normalize(colliderToParticle);
		vec2d reflectedVelocity = glm::reflect(globalParticleVelocity, normal);
		floatd alpha = std::fmod(t - collider.lifetimeStart, collider.lifetimeDuration) / collider.lifetimeDuration;
		floatd bounce = collider.bounce.get(alpha) * particleBounce;

		particles.velocity[particleIndex] = vec3d(reflectedVelocity * bounce, particles.velocity[particleIndex].z);
	}
}

void CollisionSolver::update(const Effect* effect) {
	colliders.clear();

	if(!effect) {
		return;
	}

	colliders.reserve(effect->colliders.getCount());
	for(const Collider& collider : effect->colliders) {
		std::bitset<256> exclusionSet;
		for(uint32_t particleTypeId : collider.exclusionList) {
			exclusionSet.set(particleTypeId);
		}

		for(std::size_t i = 1; i < collider.points.size(); i++) {
			colliders.push_back(LineCollider{
				exclusionSet,
				collider.lifetimeStart,
				collider.lifetimeDuration,
				collider.repeat,
				vec2d(collider.points[i - 1]),
				vec2d(collider.points[i]),
				collider.bounce,
				collider.friction
			});
		}
	}

	gridBottom = vec2d(+FLT_MAX, +FLT_MAX);
	gridTop = vec2d(-FLT_MAX, -FLT_MAX);
	for(const LineCollider& collider : colliders) {
		gridBottom = glm::min(gridBottom, glm::min(collider.p1, collider.p2));
		gridTop = glm::max(gridTop, glm::max(collider.p1, collider.p2));
	}

	gridBottom -= vec2d(gridPadding);
	gridTop += vec2d(gridPadding);

	uint32_t gridNumColumns = std::max(gridCellCountFactor * static_cast<uint32_t>((gridTop.x - gridBottom.x) * std::sqrt(static_cast<floatd>(colliders.size()) / ((gridTop.x - gridBottom.x) * (gridTop.y - gridBottom.y)))), 1U);
	uint32_t gridNumRows = std::max(gridCellCountFactor * static_cast<uint32_t>((gridTop.y - gridBottom.y) * std::sqrt(static_cast<floatd>(colliders.size()) / ((gridTop.x - gridBottom.x) * (gridTop.y - gridBottom.y)))), 1U);
	grid.resize(gridNumColumns, gridNumRows);
	grid.clear();

	gridCellDimension = (gridTop - gridBottom) / vec2d(gridNumColumns, gridNumRows);

	for(uint32_t i = 0; i < colliders.size(); i++) {
		const LineCollider& collider = colliders[i];
		vec2d direction = glm::normalize(collider.p2 - collider.p1);
		vec2d p1Grid = collider.p1 - gridBottom;
		vec2d p2Grid = collider.p2 - gridBottom;
		vec2d delta = vec2d(0.0);
		floatd tX = 0.0;
		floatd tY = 0.0;

		if(direction.x < 0.0) {
			delta.x = -gridCellDimension.x / direction.x;
			tX = (std::floor(p1Grid.x / gridCellDimension.x) * gridCellDimension.x - p1Grid.x) / direction.x;
		}
		else {
			delta.x = +gridCellDimension.x / direction.x;
			tX = ((std::floor(p1Grid.x / gridCellDimension.x) + 1) * gridCellDimension.x - p1Grid.x) / direction.x;
		}
		if(direction.y < 0.0) {
			delta.y = -gridCellDimension.y / direction.y;
			tY = (std::floor(p1Grid.y / gridCellDimension.y) * gridCellDimension.y - p1Grid.y) / direction.y;
		}
		else {
			delta.y = +gridCellDimension.y / direction.y;
			tY = ((std::floor(p1Grid.y / gridCellDimension.y) + 1) * gridCellDimension.y - p1Grid.y) / direction.y;
		}

		int32_t cX = std::min(std::max(static_cast<int32_t>(std::floor(p1Grid.x / gridCellDimension.x)), 0), static_cast<int32_t>(gridNumColumns) - 1);
		int32_t cY = std::min(std::max(static_cast<int32_t>(std::floor(p1Grid.y / gridCellDimension.y)), 0), static_cast<int32_t>(gridNumRows) - 1);
		int32_t exitCX = std::min(std::max(static_cast<int32_t>(std::floor(p2Grid.x / gridCellDimension.x)), 0), static_cast<int32_t>(gridNumColumns) - 1);
		int32_t exitCY = std::min(std::max(static_cast<int32_t>(std::floor(p2Grid.y / gridCellDimension.y)), 0), static_cast<int32_t>(gridNumRows) - 1);

		while(grid.contains(cX, cY)) {
			grid(cX, cY).push_back(i);

			if(tX < tY) {
				tX += delta.x;
				cX += (direction.x < 0.0) ? -1 : +1;
			}
			else {
				tY += delta.y;
				cY += (direction.y < 0.0) ? -1 : +1;
			}

			if(cX == exitCX && cY == exitCY) {
				break;
			}
		}
	}
}

void CollisionSolver::setGridCellCountFactor(uint32_t factor) {
	gridCellCountFactor = factor;
}
void CollisionSolver::setGridPadding(floatd padding) {
	gridPadding = padding;
}

GridIndex<int32_t> CollisionSolver::toGridIndex(const vec3d& position) const {
	int32_t x = std::min(std::max(static_cast<int32_t>(std::floor((position.x - gridBottom.x) / gridCellDimension.x)), 0), static_cast<int32_t>(grid.getWidth()) - 1);
	int32_t y = std::min(std::max(static_cast<int32_t>(std::floor((position.y - gridBottom.y) / gridCellDimension.y)), 0), static_cast<int32_t>(grid.getHeight()) - 1);

	return GridIndex<int32_t>{ x, y };
}

void CollisionSolver::findPotentialColliders(std::unordered_set<uint32_t>& potentialColliders, int32_t cx, int32_t cy, const vec2d& size) const {
	int32_t radiusX = 1 + static_cast<int32_t>(size.x / gridCellDimension.x);
	int32_t radiusY = 1 + static_cast<int32_t>(size.y / gridCellDimension.y);

	for(int32_t y = cy - radiusY; y <= cy + radiusY && y < static_cast<int32_t>(grid.getHeight()); y++) {
		for(int32_t x = cx - radiusX; x <= cx + radiusX && x < static_cast<int32_t>(grid.getWidth()); x++) {
			if(x < 0 || y < 0) {
				continue;
			}

			for(uint32_t i : grid(x, y)) {
				potentialColliders.insert(i);
			}
		}
	}
}
}