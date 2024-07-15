#include "CollisionSolver.h"
#include "../glm/glm/gtx/norm.hpp"
#include "../glm/glm/gtx/rotate_vector.hpp"

namespace pixelpart {
CollisionSolver::ColliderSegment::ColliderSegment(const Collider& collider) :
	lifetimeStart(collider.lifetimeStart), lifetimeDuration(collider.lifetimeDuration), repeat(collider.repeat),
	killOnContact(collider.killOnContact.get()),
	bounce(collider.bounce.getComputedCurve()),
	friction(collider.friction.getComputedCurve()) {

}

CollisionSolver::LineColliderSegment::LineColliderSegment(const Collider& collider, std::size_t segmentIndex) : ColliderSegment(collider) {
	for(id_t particleTypeId : collider.exclusionList) {
		exclusionSet.set(particleTypeId);
	}

	startPoint = vec2_t(collider.points.at(segmentIndex));
	endPoint = vec2_t(collider.points.at(segmentIndex + 1u));
}

CollisionSolver::PlaneColliderSegment::PlaneColliderSegment(const Collider& collider, std::size_t segmentIndex) : ColliderSegment(collider) {
	for(id_t particleTypeId : collider.exclusionList) {
		exclusionSet.set(particleTypeId);
	}

	center = (collider.points[segmentIndex + 1u] + collider.points[segmentIndex]) * 0.5;
	rightVector = collider.points[segmentIndex + 1u] - center;

	normal = vec3_t(1.0, 0.0, 0.0);
	if(std::abs(rightVector.x) > 0.001) {
		normal = glm::normalize(vec3_t(-rightVector.y / rightVector.x, 1.0, 0.0));
	}
	else if(std::abs(rightVector.z) > 0.001) {
		normal = glm::normalize(vec3_t(0.0, 1.0, -rightVector.y / rightVector.z));
	}

	upVector = glm::cross(glm::normalize(rightVector), normal) * collider.width.get() * 0.5;

	normal = glm::rotate(normal, glm::radians(collider.orientation.get()), rightVector);
	upVector = glm::rotate(upVector, glm::radians(collider.orientation.get()), rightVector);
}

CollisionSolver::CollisionSolver() : grid(1u, 1u) {

}

void CollisionSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleWritePtr particles, uint32_t numParticles, float_t t, float_t dt) const {
	if(!planeColliders.empty()) {
		for(uint32_t p = 0u; p < numParticles; p++) {
			for(const PlaneColliderSegment& collider : planeColliders) {
				if(collider.exclusionSet[particleType.id] ||
					(t < collider.lifetimeStart) ||
					(t > collider.lifetimeStart + collider.lifetimeDuration && !collider.repeat)) {
					continue;
				}

				solve(particleType, particles, p, t, dt, collider);
			}
		}
	}
	else if(!lineColliders.empty()) {
		for(uint32_t p = 0u; p < numParticles; p++) {
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
					findPotentialColliders(potentialColliders, cx, cy, particles.size[p] * particleType.physicalSize.get(particles.life[p]));
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

void CollisionSolver::refresh(const Effect& effect) {
	lineColliders.clear();
	planeColliders.clear();
	grid.clear();

	if(effect.is3d) {
		planeColliders.reserve(effect.colliders.getCount());
		for(const Collider& collider : effect.colliders) {
			for(std::size_t i = 0u; i + 1u < collider.points.size(); i++) {
				planeColliders.emplace_back(collider, i);
			}
		}
	}
	else if(effect.colliders.getCount() > 0) {
		lineColliders.reserve(effect.colliders.getCount());
		for(const Collider& collider : effect.colliders) {
			for(std::size_t i = 0u; i + 1u < collider.points.size(); i++) {
				lineColliders.emplace_back(collider, i);
			}
		}

		gridBottom = vec2_t(+FLT_MAX, +FLT_MAX);
		gridTop = vec2_t(-FLT_MAX, -FLT_MAX);
		for(const LineColliderSegment& collider : lineColliders) {
			gridBottom = glm::min(gridBottom, glm::min(collider.startPoint, collider.endPoint));
			gridTop = glm::max(gridTop, glm::max(collider.startPoint, collider.endPoint));
		}

		gridBottom -= vec2_t(gridPadding);
		gridTop += vec2_t(gridPadding);

		uint32_t gridNumColumns = std::max(gridCellCountFactor * static_cast<uint32_t>(
			(gridTop.x - gridBottom.x) * std::sqrt(static_cast<float_t>(lineColliders.size()) / ((gridTop.x - gridBottom.x) * (gridTop.y - gridBottom.y)))),
				1u);
		uint32_t gridNumRows = std::max(gridCellCountFactor * static_cast<uint32_t>(
			(gridTop.y - gridBottom.y) * std::sqrt(static_cast<float_t>(lineColliders.size()) / ((gridTop.x - gridBottom.x) * (gridTop.y - gridBottom.y)))),
				1u);
		grid.resize(gridNumColumns, gridNumRows);
		grid.clear();

		gridCellDimension = (gridTop - gridBottom) / vec2_t(gridNumColumns, gridNumRows);

		for(uint32_t i = 0u; i < lineColliders.size(); i++) {
			const LineColliderSegment& collider = lineColliders[i];
			vec2_t direction = glm::normalize(collider.endPoint - collider.startPoint);
			vec2_t p1Grid = collider.startPoint - gridBottom;
			vec2_t p2Grid = collider.endPoint - gridBottom;
			vec2_t delta = vec2_t(0.0);
			float_t tX = 0.0;
			float_t tY = 0.0;

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
void CollisionSolver::setGridPadding(float_t padding) {
	gridPadding = padding;
}

CollisionSolver::Intersection::Intersection() {

}
CollisionSolver::Intersection::Intersection(const vec3_t& p) : hit(true), point(p) {

}

bool CollisionSolver::isPointOnLineSegment(const vec2_t& p, const vec2_t& l1, const vec2_t& l2) {
	float_t A = glm::dot(l2 - l1, p - l1);
	float_t B = glm::dot(l2 - l1, l2 - l1);

	return A >= 0.0 && A < B;
}
bool CollisionSolver::isPointOnLineSegment(const vec3_t& p, const vec3_t& l1, const vec3_t& l2) {
	float_t A = glm::dot(l2 - l1, p - l1);
	float_t B = glm::dot(l2 - l1, l2 - l1);

	return A >= 0.0 && A < B;
}
bool CollisionSolver::isPointOnCollider(const vec3_t& p, const PlaneColliderSegment& collider) {
	vec3_t vertices[4] = {
		collider.center - collider.rightVector - collider.upVector,
		collider.center + collider.rightVector - collider.upVector,
		collider.center - collider.rightVector + collider.upVector,
		collider.center + collider.rightVector + collider.upVector,
	};

	vec3_t V1ToV0 = vertices[0] - vertices[1];
	vec3_t V1ToV3 = vertices[3] - vertices[1];
	vec3_t V2ToV0 = vertices[0] - vertices[2];
	vec3_t V2ToV3 = vertices[3] - vertices[2];
	vec3_t V1ToP = p - vertices[1];
	vec3_t V2ToP = p - vertices[2];

	return
		glm::dot(V1ToP, V1ToV0) > 0.0 &&
		glm::dot(V1ToP, V1ToV3) > 0.0 &&
		glm::dot(V2ToP, V2ToV0) > 0.0 &&
		glm::dot(V2ToP, V2ToV3) > 0.0;
}

vec2_t CollisionSolver::calculateClosestPointOnLine(const vec2_t& p, const LineColliderSegment& collider) {
	float_t A1 = collider.endPoint.y - collider.startPoint.y;
	float_t B1 = collider.startPoint.x - collider.endPoint.x;
	float_t C1 = (collider.endPoint.y - collider.startPoint.y) * collider.startPoint.x + (collider.startPoint.x - collider.endPoint.x) * collider.startPoint.y;
	float_t C2 = -B1 * p.x + A1 * p.y;
	float_t det = A1 * A1 - -B1 * B1;

	return (det != 0.0)
		? vec2_t(
			(A1 * C1 - B1 * C2) / det,
			(A1 * C2 - -B1 * C1) / det)
		: p;
}
vec3_t CollisionSolver::calculateClosestPointOnPlane(const vec3_t& p, const PlaneColliderSegment& collider) {
	float_t signedDistance = glm::dot(collider.normal, p - collider.center);

	return p - collider.normal * signedDistance;
}
CollisionSolver::Intersection CollisionSolver::calculateRayColliderIntersection(const LineColliderSegment& collider, const vec2_t& rayOrigin, const vec2_t& rayEnd) {
	float_t A1 = collider.endPoint.y - collider.startPoint.y;
	float_t B1 = collider.startPoint.x - collider.endPoint.x;
	float_t C1 = A1 * collider.startPoint.x + B1 * collider.startPoint.y;
	float_t A2 = rayEnd.y - rayOrigin.y;
	float_t B2 = rayOrigin.x - rayEnd.x;
	float_t C2 = A2 * rayOrigin.x + B2 * rayOrigin.y;
	float_t det = A1 * B2 - A2 * B1;

	if(std::abs(det) < 0.00001) {
		return Intersection();
	}

	vec2_t point = vec2_t(
		(B2 * C1 - B1 * C2) / det,
		(A1 * C2 - A2 * C1) / det);

	if(!isPointOnLineSegment(point, collider.startPoint, collider.endPoint) || !isPointOnLineSegment(point, rayOrigin, rayEnd)) {
		return Intersection();
	}

	return Intersection(vec3_t(point, 0.0));
}
CollisionSolver::Intersection CollisionSolver::calculateRayColliderIntersection(const PlaneColliderSegment& collider, const vec3_t& rayOrigin, const vec3_t& rayEnd) {
	vec3_t rayDirection = glm::normalize(rayEnd - rayOrigin);

	float_t det = glm::dot(collider.normal, rayDirection);
	if(std::abs(det) < 0.00001) {
		return Intersection();
	}

	float_t t = -glm::dot(collider.normal, rayOrigin - collider.center) / det;
	vec3_t point = rayOrigin + rayDirection * t;

	if(!isPointOnCollider(point, collider) || !isPointOnLineSegment(point, rayOrigin, rayEnd)) {
		return Intersection();
	}

	return Intersection(point);
}

void CollisionSolver::solve(const ParticleType& particleType, ParticleWritePtr particles, uint32_t p, float_t t, float_t dt, const LineColliderSegment& collider) const {
	vec2_t closestPoint = calculateClosestPointOnLine(particles.globalPosition[p], collider);
	if(!isPointOnLineSegment(closestPoint, collider.startPoint, collider.endPoint)) {
		return;
	}

	float_t colliderLife = std::fmod(t - collider.lifetimeStart, collider.lifetimeDuration) / collider.lifetimeDuration;

	vec2_t globalParticlePosition = vec2_t(particles.globalPosition[p]);
	vec2_t globalParticleVelocity = vec2_t(particles.velocity[p]);
	vec2_t globalParticleForce = vec2_t(particles.force[p]);
	vec2_t colliderToParticle = globalParticlePosition - closestPoint;
	float_t distanceSqr = glm::length2(colliderToParticle);
	float_t particleRadius = std::min(particles.size[p].x, particles.size[p].y) * 0.5;
	particleRadius = std::max(particleRadius * particleType.physicalSize.get(particles.life[p]), 0.01);

	if(distanceSqr <= particleRadius * particleRadius) {
		if(collider.killOnContact) {
			particles.life[p] = 1.0;
		}
		else {
			float_t distance = std::sqrt(distanceSqr);
			vec2_t normal = glm::normalize(colliderToParticle);
			vec2_t reflectedVelocity = glm::reflect(globalParticleVelocity, normal);
			vec2_t segmentVector = glm::normalize(collider.endPoint - collider.startPoint);
			float_t slideFactor = glm::dot((globalParticleForce != vec2_t(0.0)) ? glm::normalize(globalParticleForce) : vec2_t(0.0, 1.0), segmentVector) * glm::length(globalParticleForce) * distance / particleRadius;
			float_t bounce = collider.bounce.get(colliderLife) * particleType.bounce.get(particles.life[p]);
			float_t friction = std::min(collider.friction.get(colliderLife) * particleType.friction.get(particles.life[p]), 1.0);

			particles.velocity[p] = vec3_t(reflectedVelocity * bounce, particles.velocity[p].z);
			particles.velocity[p] += vec3_t(segmentVector * slideFactor * (1.0 - friction), 0.0);
			particles.position[p] += vec3_t(normal * (particleRadius - distance), 0.0);
		}
	}
	else {
		Intersection intersection = calculateRayColliderIntersection(collider, globalParticlePosition, globalParticlePosition + globalParticleVelocity * dt + globalParticleForce * dt * dt);

		if(intersection.hit) {
			if(collider.killOnContact) {
				particles.life[p] = 1.0;
			}
			else {
				vec2_t normal = glm::normalize(colliderToParticle);
				vec2_t reflectedVelocity = glm::reflect(globalParticleVelocity, normal);
				float_t bounce = collider.bounce.get(colliderLife) * particleType.bounce.get(particles.life[p]);

				particles.velocity[p] = vec3_t(reflectedVelocity * bounce, particles.velocity[p].z);
			}
		}
	}
}
void CollisionSolver::solve(const ParticleType& particleType, ParticleWritePtr particles, uint32_t p, float_t t, float_t dt, const PlaneColliderSegment& collider) const {
	vec3_t closestPoint = calculateClosestPointOnPlane(particles.globalPosition[p], collider);
	if(!isPointOnCollider(closestPoint, collider)) {
		return;
	}

	float_t colliderLife = std::fmod(t - collider.lifetimeStart, collider.lifetimeDuration) / collider.lifetimeDuration;
	vec3_t globalParticlePosition = particles.globalPosition[p];
	vec3_t globalParticleVelocity = particles.velocity[p];
	vec3_t globalParticleForce = particles.force[p];
	vec3_t normalizedParticleForce = (globalParticleForce != vec3_t(0.0))
		? glm::normalize(globalParticleForce)
		: vec3_t(0.0, 1.0, 0.0);
	vec3_t colliderToParticle = globalParticlePosition - closestPoint;
	vec3_t colliderNormal = glm::normalize(colliderToParticle);
	float_t distanceSqr = glm::length2(colliderToParticle);
	float_t particleRadius = std::min(particles.size[p].x, std::min(particles.size[p].y, particles.size[p].z)) * 0.5;
	particleRadius = std::max(particleRadius * particleType.physicalSize.get(particles.life[p]), 0.01);

	if(distanceSqr <= particleRadius * particleRadius) {
		if(collider.killOnContact) {
			particles.life[p] = 1.0;
		}
		else {
			float_t distance = std::sqrt(distanceSqr);
			vec3_t reflectedVelocity = glm::reflect(globalParticleVelocity, colliderNormal);

			vec3_t slideVector = calculateClosestPointOnPlane(closestPoint + normalizedParticleForce, collider) - closestPoint;
			if(slideVector != vec3_t(0.0)) {
				slideVector = glm::normalize(slideVector);
			}

			float_t slideFactor = glm::dot(normalizedParticleForce, slideVector) * glm::length(globalParticleForce) * distance / particleRadius;
			float_t bounce = collider.bounce.get(colliderLife) * particleType.bounce.get(particles.life[p]);
			float_t friction = std::min(collider.friction.get(colliderLife) * particleType.friction.get(particles.life[p]), 1.0);

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
				vec3_t reflectedVelocity = glm::reflect(globalParticleVelocity, colliderNormal);
				float_t bounce = collider.bounce.get(colliderLife) * particleType.bounce.get(particles.life[p]);

				particles.velocity[p] = reflectedVelocity * bounce;
			}
		}
	}
}

GridIndex<int32_t> CollisionSolver::toGridIndex(const vec3_t& position) const {
	int32_t x = std::min(std::max(static_cast<int32_t>(std::floor((position.x - gridBottom.x) / gridCellDimension.x)), 0), static_cast<int32_t>(grid.getWidth()) - 1);
	int32_t y = std::min(std::max(static_cast<int32_t>(std::floor((position.y - gridBottom.y) / gridCellDimension.y)), 0), static_cast<int32_t>(grid.getHeight()) - 1);

	return GridIndex<int32_t>{ x, y };
}

void CollisionSolver::findPotentialColliders(std::unordered_set<uint32_t>& potentialColliders, int32_t cx, int32_t cy, const vec2_t& size) const {
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