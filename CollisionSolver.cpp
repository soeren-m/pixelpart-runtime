#include "CollisionSolver.h"

namespace pixelpart {
bool CollisionSolver::onSegment(const vec2d& l1, const vec2d& l2, const vec2d& p) {
	floatd A = glm::dot(l2 - l1, p - l1);
	floatd B = glm::dot(l2 - l1, l2 - l1);

	return (A >= 0.0) && (A < B);
}
bool CollisionSolver::closestPointOnSegment(const vec2d& l1, const vec2d& l2, const vec2d& p, vec2d& result) { 
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

	return onSegment(l1, l2, result);
}
bool CollisionSolver::intersectionSegmentSegment(const vec2d& p1, const vec2d& p2, const vec2d& q1, const vec2d& q2, vec2d& result) {
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

		return onSegment(p1, p2, result) && onSegment(q1, q2, result);
	}

	return false;
}

CollisionSolver::CollisionSolver() : grid(1, 1) {

}

void CollisionSolver::solve(uint32_t emitterId, ParticleDataPointer& particles, uint32_t p, floatd particleBounce, floatd particleFriction, floatd t, floatd dt) const {
	const auto toGridIndex = [this](const vec2d& position) -> GridIndex<int32_t> {
		return GridIndex<int32_t>{
			std::min(std::max(static_cast<int32_t>(std::floor((position.x - gridBottom.x) / gridCellDimension.x)), 0), static_cast<int32_t>(grid.getWidth()) - 1),
			std::min(std::max(static_cast<int32_t>(std::floor((position.y - gridBottom.y) / gridCellDimension.y)), 0), static_cast<int32_t>(grid.getHeight()) - 1) };
	};
	
	GridIndex<int32_t> startIndex = toGridIndex(particles.globalPosition[p]);
	GridIndex<int32_t> endIndex = toGridIndex(particles.globalPosition[p] + particles.velocity[p] * dt + particles.force[p] * dt * dt);

	if(startIndex.x > endIndex.x) {
		std::swap(startIndex.x, endIndex.x);
	}
	if(startIndex.y > endIndex.y) {
		std::swap(startIndex.y, endIndex.y);
	}
	
	std::unordered_set<uint32_t> potentialColliders;
	for(int32_t cy = startIndex.y; cy <= endIndex.y; cy++) {
		for(int32_t cx = startIndex.x; cx <= endIndex.x; cx++) {
			findPotentialColliders(potentialColliders, cx, cy, particles.size[p]);
		}
	}

	for(uint32_t i : potentialColliders) {
		const LineCollider& collider = colliders[i];
		if(!collider.emitterMask[emitterId] || t < collider.lifetimeStart || (t > collider.lifetimeStart + collider.lifetimeDuration && !collider.repeat)) {
			continue;
		}

		solve(particles, p, particleBounce, particleFriction, t, dt, collider);
	}
}
void CollisionSolver::solve(ParticleDataPointer& particles, uint32_t p, floatd particleBounce, floatd particleFriction, floatd t, floatd dt, const LineCollider& collider) const {
	vec2d closestPoint = vec2d(0.0);
	if(!closestPointOnSegment(collider.p1, collider.p2, particles.globalPosition[p], closestPoint)) {
		return;
	}

	const vec2d colliderToParticle = particles.globalPosition[p] - closestPoint;
	const floatd distanceSqr = glm::length2(colliderToParticle);
	const floatd particleRadius = std::max(std::min(particles.size[p].x, particles.size[p].y) * 0.5, 0.01);
	vec2d intersectionPoint = vec2d(0.0);

	if(distanceSqr <= particleRadius * particleRadius) {
		const floatd distance = std::sqrt(distanceSqr);
		const vec2d normal = glm::normalize(colliderToParticle);
		const vec2d reflectedVelocity = glm::reflect(particles.velocity[p], normal);
		const vec2d segmentVector = glm::normalize(collider.p2 - collider.p1);
		const floatd slideFactor = glm::dot((particles.force[p] != vec2d(0.0)) ? glm::normalize(particles.force[p]) : vec2d(0.0, 1.0), segmentVector) * glm::length(particles.force[p]);
		const floatd alpha = std::fmod(t - collider.lifetimeStart, collider.lifetimeDuration) / collider.lifetimeDuration;
		const floatd bounce = collider.bounce.get(alpha) * particleBounce;
		const floatd friction = std::min(collider.friction.get(alpha) * particleFriction, 1.0);
	
		particles.velocity[p] = reflectedVelocity * bounce;
		particles.velocity[p] += segmentVector * slideFactor * (1.0 - friction);
		particles.position[p] += reflectedVelocity * (particleRadius - distance);
	}
	else if(intersectionSegmentSegment(collider.p1, collider.p2, particles.globalPosition[p], particles.globalPosition[p] + particles.velocity[p] * dt + particles.force[p] * dt * dt, intersectionPoint)) {
		const vec2d normal = glm::normalize(colliderToParticle);
		const vec2d reflectedVelocity = glm::reflect(particles.velocity[p], normal);
		const floatd alpha = std::fmod(t - collider.lifetimeStart, collider.lifetimeDuration) / collider.lifetimeDuration;
		const floatd bounce = collider.bounce.get(alpha) * particleBounce;

		particles.velocity[p] = reflectedVelocity * bounce;
	}
}

void CollisionSolver::update(const std::vector<Collider>& allColliders) {
	colliders.clear();
	colliders.reserve(allColliders.size());
	for(const Collider& c : allColliders) {
		for(std::size_t i = 1; i < c.points.size(); i++) {
			colliders.push_back(LineCollider{
				c.emitterMask,
				c.lifetimeStart,
				c.lifetimeDuration,
				c.repeat,
				c.points[i - 1],
				c.points[i],
				c.bounce,
				c.friction	
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