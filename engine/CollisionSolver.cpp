#include "CollisionSolver.h"

namespace pixelpart {
CollisionSolver::ColliderSegment::ColliderSegment(const Collider& collider) :
	lifetimeStart(collider.lifetimeStart), lifetimeDuration(collider.lifetimeDuration), repeat(collider.repeat), killOnContact(collider.killOnContact),
	bounce(collider.bounce), friction(collider.friction) {

}

CollisionSolver::LineColliderSegment::LineColliderSegment(const Collider& collider, std::size_t segmentIndex) : ColliderSegment(collider) {
	for(uint32_t particleTypeId : collider.exclusionList) {
		exclusionSet.set(particleTypeId);
	}

	startPoint = vec2d(collider.points.at(segmentIndex));
	endPoint = vec2d(collider.points.at(segmentIndex + 1u));
}

CollisionSolver::PlaneColliderSegment::PlaneColliderSegment(const Collider& collider, std::size_t segmentIndex) : ColliderSegment(collider) {
	for(uint32_t particleTypeId : collider.exclusionList) {
		exclusionSet.set(particleTypeId);
	}

	center = (collider.points[segmentIndex + 1u] + collider.points[segmentIndex]) * 0.5;
	rightVector = collider.points[segmentIndex + 1u] - center;

	normal = vec3d(1.0, 0.0, 0.0);
	if(std::abs(rightVector.x) > 0.001) {
		normal = glm::normalize(vec3d(-rightVector.y / rightVector.x, 1.0, 0.0));
	}
	else if(std::abs(rightVector.z) > 0.001) {
		normal = glm::normalize(vec3d(0.0, 1.0, -rightVector.y / rightVector.z));
	}

	upVector = glm::cross(glm::normalize(rightVector), normal) * collider.width * 0.5;

	normal = glm::rotate(normal, glm::radians(collider.orientation), rightVector);
	upVector = glm::rotate(upVector, glm::radians(collider.orientation), rightVector);
}

CollisionSolver::CollisionSolver() : grid(1, 1) {

}

void CollisionSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, floatd t, floatd dt) const {
	if(!planeColliders.empty()) {
		for(uint32_t p = 0; p < numParticles; p++) {
			for(const PlaneColliderSegment& collider : planeColliders) {
				if(collider.exclusionSet[particleType.id] ||
					t < collider.lifetimeStart ||
					t > collider.lifetimeStart + collider.lifetimeDuration && !collider.repeat) {
					continue;
				}

				solve(particleType, particles, p, t, dt, collider);
			}
		}
	}
	else if(!lineColliders.empty()) {
		for(uint32_t p = 0; p < numParticles; p++) {
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
				const LineColliderSegment& collider = lineColliders[i];
				if(collider.exclusionSet[particleType.id] ||
					t < collider.lifetimeStart ||
					t > collider.lifetimeStart + collider.lifetimeDuration && !collider.repeat) {
					continue;
				}

				solve(particleType, particles, p, t, dt, collider);
			}
		}
	}
}

void CollisionSolver::update(const Effect* effect) {
	lineColliders.clear();
	planeColliders.clear();
	grid.clear();

	if(!effect) {
		return;
	}

	if(effect->is3d) {
		planeColliders.reserve(effect->colliders.getCount());
		for(const Collider& collider : effect->colliders) {
			for(std::size_t i = 0u; i + 1u < collider.points.size(); i++) {
				planeColliders.emplace_back(collider, i);
			}
		}
	}
	else {
		lineColliders.reserve(effect->colliders.getCount());
		for(const Collider& collider : effect->colliders) {
			for(std::size_t i = 0u; i + 1u < collider.points.size(); i++) {
				lineColliders.emplace_back(collider, i);
			}
		}

		gridBottom = vec2d(+FLT_MAX, +FLT_MAX);
		gridTop = vec2d(-FLT_MAX, -FLT_MAX);
		for(const LineColliderSegment& collider : lineColliders) {
			gridBottom = glm::min(gridBottom, glm::min(collider.startPoint, collider.endPoint));
			gridTop = glm::max(gridTop, glm::max(collider.startPoint, collider.endPoint));
		}

		gridBottom -= vec2d(gridPadding);
		gridTop += vec2d(gridPadding);

		uint32_t gridNumColumns = std::max(gridCellCountFactor * static_cast<uint32_t>((gridTop.x - gridBottom.x) * std::sqrt(static_cast<floatd>(lineColliders.size()) / ((gridTop.x - gridBottom.x) * (gridTop.y - gridBottom.y)))), 1U);
		uint32_t gridNumRows = std::max(gridCellCountFactor * static_cast<uint32_t>((gridTop.y - gridBottom.y) * std::sqrt(static_cast<floatd>(lineColliders.size()) / ((gridTop.x - gridBottom.x) * (gridTop.y - gridBottom.y)))), 1U);
		grid.resize(gridNumColumns, gridNumRows);
		grid.clear();

		gridCellDimension = (gridTop - gridBottom) / vec2d(gridNumColumns, gridNumRows);

		for(uint32_t i = 0; i < lineColliders.size(); i++) {
			const LineColliderSegment& collider = lineColliders[i];
			vec2d direction = glm::normalize(collider.endPoint - collider.startPoint);
			vec2d p1Grid = collider.startPoint - gridBottom;
			vec2d p2Grid = collider.endPoint - gridBottom;
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
}

void CollisionSolver::setGridCellCountFactor(uint32_t factor) {
	gridCellCountFactor = factor;
}
void CollisionSolver::setGridPadding(floatd padding) {
	gridPadding = padding;
}

CollisionSolver::Intersection::Intersection() {

}
CollisionSolver::Intersection::Intersection(const vec3d& p) : hit(true), point(p) {

}

bool CollisionSolver::isPointOnLineSegment(const vec2d& p, const vec2d& l1, const vec2d& l2) {
	floatd A = glm::dot(l2 - l1, p - l1);
	floatd B = glm::dot(l2 - l1, l2 - l1);

	return A >= 0.0 && A < B;
}
bool CollisionSolver::isPointOnLineSegment(const vec3d& p, const vec3d& l1, const vec3d& l2) {
	floatd A = glm::dot(l2 - l1, p - l1);
	floatd B = glm::dot(l2 - l1, l2 - l1);

	return A >= 0.0 && A < B;
}
bool CollisionSolver::isPointOnCollider(const vec3d& p, const PlaneColliderSegment& collider) {
	vec3d vertices[4] = {
		collider.center - collider.rightVector - collider.upVector,
		collider.center + collider.rightVector - collider.upVector,
		collider.center - collider.rightVector + collider.upVector,
		collider.center + collider.rightVector + collider.upVector,
	};

	vec3d V1ToV0 = vertices[0] - vertices[1];
	vec3d V1ToV3 = vertices[3] - vertices[1];
	vec3d V2ToV0 = vertices[0] - vertices[2];
	vec3d V2ToV3 = vertices[3] - vertices[2];
	vec3d V1ToP = p - vertices[1];
	vec3d V2ToP = p - vertices[2];

	return
		glm::dot(V1ToP, V1ToV0) > 0.0 &&
		glm::dot(V1ToP, V1ToV3) > 0.0 &&
		glm::dot(V2ToP, V2ToV0) > 0.0 &&
		glm::dot(V2ToP, V2ToV3) > 0.0;
}

vec2d CollisionSolver::calculateClosestPointOnLine(const vec2d& p, const LineColliderSegment& collider) {
	floatd A1 = collider.endPoint.y - collider.startPoint.y;
	floatd B1 = collider.startPoint.x - collider.endPoint.x;
	floatd C1 = (collider.endPoint.y - collider.startPoint.y) * collider.startPoint.x + (collider.startPoint.x - collider.endPoint.x) * collider.startPoint.y;
	floatd C2 = -B1 * p.x + A1 * p.y;
	floatd det = A1 * A1 - -B1 * B1;

	return (det != 0.0)
		? vec2d(
			(A1 * C1 - B1 * C2) / det,
			(A1 * C2 - -B1 * C1) / det)
		: p;
}
vec3d CollisionSolver::calculateClosestPointOnPlane(const vec3d& p, const PlaneColliderSegment& collider) {
	floatd signedDistance = glm::dot(collider.normal, p - collider.center);

	return p - collider.normal * signedDistance;
}
CollisionSolver::Intersection CollisionSolver::calculateRayColliderIntersection(const LineColliderSegment& collider, const vec2d& rayOrigin, const vec2d& rayEnd) {
	floatd A1 = collider.endPoint.y - collider.startPoint.y;
	floatd B1 = collider.startPoint.x - collider.endPoint.x;
	floatd C1 = A1 * collider.startPoint.x + B1 * collider.startPoint.y;
	floatd A2 = rayEnd.y - rayOrigin.y;
	floatd B2 = rayOrigin.x - rayEnd.x;
	floatd C2 = A2 * rayOrigin.x + B2 * rayOrigin.y;
	floatd det = A1 * B2 - A2 * B1;

	if(std::abs(det) < 0.00001) {
		return Intersection();
	}

	vec2d point = vec2d(
		(B2 * C1 - B1 * C2) / det,
		(A1 * C2 - A2 * C1) / det);

	if(!isPointOnLineSegment(point, collider.startPoint, collider.endPoint) || !isPointOnLineSegment(point, rayOrigin, rayEnd)) {
		return Intersection();
	}

	return Intersection(vec3d(point, 0.0));
}
CollisionSolver::Intersection CollisionSolver::calculateRayColliderIntersection(const PlaneColliderSegment& collider, const vec3d& rayOrigin, const vec3d& rayEnd) {
	vec3d rayDirection = glm::normalize(rayEnd - rayOrigin);

	floatd det = glm::dot(collider.normal, rayDirection);
	if(std::abs(det) < 0.00001) {
		return Intersection();
	}

	floatd t = -glm::dot(collider.normal, rayOrigin - collider.center) / det;
	vec3d point = rayOrigin + rayDirection * t;

	if(!isPointOnCollider(point, collider) || !isPointOnLineSegment(point, rayOrigin, rayEnd)) {
		return Intersection();
	}

	return Intersection(point);
}

void CollisionSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t p, floatd t, floatd dt, const LineColliderSegment& collider) const {
	vec2d closestPoint = calculateClosestPointOnLine(particles.globalPosition[p], collider);
	if(!isPointOnLineSegment(closestPoint, collider.startPoint, collider.endPoint)) {
		return;
	}

	floatd colliderLife = std::fmod(t - collider.lifetimeStart, collider.lifetimeDuration) / collider.lifetimeDuration;

	vec2d globalParticlePosition = vec2d(particles.globalPosition[p]);
	vec2d globalParticleVelocity = vec2d(particles.velocity[p]);
	vec2d globalParticleForce = vec2d(particles.force[p]);
	vec2d colliderToParticle = globalParticlePosition - closestPoint;
	floatd distanceSqr = glm::length2(colliderToParticle);
	floatd particleRadius = std::max(std::min(
			particles.size[p].x,
			particles.size[p].y) * 0.5, 0.01);

	if(distanceSqr <= particleRadius * particleRadius) {
		if(collider.killOnContact) {
			particles.life[p] = 1.0;
		}
		else {
			floatd distance = std::sqrt(distanceSqr);
			vec2d normal = glm::normalize(colliderToParticle);
			vec2d reflectedVelocity = glm::reflect(globalParticleVelocity, normal);
			vec2d segmentVector = glm::normalize(collider.endPoint - collider.startPoint);
			floatd slideFactor = glm::dot((globalParticleForce != vec2d(0.0)) ? glm::normalize(globalParticleForce) : vec2d(0.0, 1.0), segmentVector) * glm::length(globalParticleForce) * distance / particleRadius;
			floatd bounce = collider.bounce.get(colliderLife) * particleType.bounce.get(particles.life[p]);
			floatd friction = std::min(collider.friction.get(colliderLife) * particleType.friction.get(particles.life[p]), 1.0);

			particles.velocity[p] = vec3d(reflectedVelocity * bounce, particles.velocity[p].z);
			particles.velocity[p] += vec3d(segmentVector * slideFactor * (1.0 - friction), 0.0);
			particles.position[p] += vec3d(normal * (particleRadius - distance), 0.0);
		}
	}
	else {
		Intersection intersection = calculateRayColliderIntersection(collider, globalParticlePosition, globalParticlePosition + globalParticleVelocity * dt + globalParticleForce * dt * dt);

		if(intersection.hit) {
			if(collider.killOnContact) {
				particles.life[p] = 1.0;
			}
			else {
				vec2d normal = glm::normalize(colliderToParticle);
				vec2d reflectedVelocity = glm::reflect(globalParticleVelocity, normal);
				floatd bounce = collider.bounce.get(colliderLife) * particleType.bounce.get(particles.life[p]);

				particles.velocity[p] = vec3d(reflectedVelocity * bounce, particles.velocity[p].z);
			}
		}
	}
}
void CollisionSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t p, floatd t, floatd dt, const PlaneColliderSegment& collider) const {
	vec3d closestPoint = calculateClosestPointOnPlane(particles.globalPosition[p], collider);
	if(!isPointOnCollider(closestPoint, collider)) {
		return;
	}

	floatd colliderLife = std::fmod(t - collider.lifetimeStart, collider.lifetimeDuration) / collider.lifetimeDuration;
	vec3d globalParticlePosition = particles.globalPosition[p];
	vec3d globalParticleVelocity = particles.velocity[p];
	vec3d globalParticleForce = particles.force[p];
	vec3d normalizedParticleForce = (globalParticleForce != vec3d(0.0))
		? glm::normalize(globalParticleForce)
		: vec3d(0.0, 1.0, 0.0);
	vec3d colliderToParticle = globalParticlePosition - closestPoint;
	vec3d colliderNormal = glm::normalize(colliderToParticle);
	floatd distanceSqr = glm::length2(colliderToParticle);
	floatd particleRadius = std::max(std::min(
		particles.size[p].x, std::min(
			particles.size[p].y,
			particles.size[p].z)) * 0.5, 0.01);

	if(distanceSqr <= particleRadius * particleRadius) {
		if(collider.killOnContact) {
			particles.life[p] = 1.0;
		}
		else {
			floatd distance = std::sqrt(distanceSqr);
			vec3d reflectedVelocity = glm::reflect(globalParticleVelocity, colliderNormal);
			vec3d slideVector = glm::normalize(calculateClosestPointOnPlane(closestPoint + normalizedParticleForce, collider) - closestPoint);
			floatd slideFactor = glm::dot(normalizedParticleForce, slideVector) * glm::length(globalParticleForce) * distance / particleRadius;
			floatd bounce = collider.bounce.get(colliderLife) * particleType.bounce.get(particles.life[p]);
			floatd friction = std::min(collider.friction.get(colliderLife) * particleType.friction.get(particles.life[p]), 1.0);

			particles.velocity[p] = reflectedVelocity * bounce;
			particles.velocity[p] += slideVector * slideFactor * (1.0 - friction);
			particles.position[p] += colliderNormal * (particleRadius - distance);
		}
	}
	else {
		Intersection intersection = calculateRayColliderIntersection(collider, globalParticlePosition, globalParticlePosition + globalParticleVelocity * dt + globalParticleForce * dt * dt);

		if(intersection.hit) {
			if(collider.killOnContact) {
				particles.life[p] = 1.0;
			}
			else {
				vec3d reflectedVelocity = glm::reflect(globalParticleVelocity, colliderNormal);
				floatd bounce = collider.bounce.get(colliderLife) * particleType.bounce.get(particles.life[p]);

				particles.velocity[p] = reflectedVelocity * bounce;
			}
		}
	}
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