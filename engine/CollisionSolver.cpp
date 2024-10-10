#include "CollisionSolver.h"
#include "../common/Id.h"
#include "../glm/gtx/norm.hpp"
#include "../glm/gtx/rotate_vector.hpp"
#include <cmath>
#include <algorithm>

namespace pixelpart {
CollisionSolver::ColliderSegment::ColliderSegment(const Collider& collider) :
	start(collider.start()), duration(collider.duration()), repeat(collider.repeat()),
	killOnContact(collider.killOnContact().value()),
	bounce(collider.bounce().resultCurve()),
	friction(collider.friction().resultCurve()) {

}

CollisionSolver::LineColliderSegment::LineColliderSegment(const Collider& collider, std::size_t segmentIndex) : ColliderSegment(collider) {
	for(id_t particleTypeId : collider.exclusionSet()) {
		exclusionSet.set(particleTypeId.value());
	}

	startPoint = float2_t(collider.points().at(segmentIndex));
	endPoint = float2_t(collider.points().at(segmentIndex + 1u));
}

CollisionSolver::PlaneColliderSegment::PlaneColliderSegment(const Collider& collider, std::size_t segmentIndex) : ColliderSegment(collider) {
	for(id_t particleTypeId : collider.exclusionSet()) {
		exclusionSet.set(particleTypeId.value());
	}

	center = (collider.point(segmentIndex + 1u) + collider.point(segmentIndex)) * 0.5;
	rightVector = collider.point(segmentIndex + 1u) - center;

	normal = float3_t(1.0, 0.0, 0.0);
	if(std::abs(rightVector.x) > 0.001) {
		normal = glm::normalize(float3_t(-rightVector.y / rightVector.x, 1.0, 0.0));
	}
	else if(std::abs(rightVector.z) > 0.001) {
		normal = glm::normalize(float3_t(0.0, 1.0, -rightVector.y / rightVector.z));
	}

	upVector = glm::cross(glm::normalize(rightVector), normal) * collider.width().value() * 0.5;

	normal = glm::rotate(normal, glm::radians(collider.orientation().value()), rightVector);
	upVector = glm::rotate(upVector, glm::radians(collider.orientation().value()), rightVector);
}

CollisionSolver::CollisionSolver() : solverGrid(1u, 1u) {

}

void CollisionSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const {
	if(!planeColliders.empty()) {
		for(uint32_t p = 0u; p < particleCount; p++) {
			for(const PlaneColliderSegment& collider : planeColliders) {
				if(collider.exclusionSet[particleType.id().value()] ||
					(t < collider.start) ||
					(t > collider.start + collider.duration && !collider.repeat)) {
					continue;
				}

				solve(particleType, particles, p, t, dt, collider);
			}
		}
	}
	else if(!lineColliders.empty()) {
		for(uint32_t p = 0u; p < particleCount; p++) {
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
					findPotentialColliders(potentialColliders, cx, cy, particles.size[p] * particleType.physicalSize().at(particles.life[p]));
				}
			}

			for(uint32_t i : potentialColliders) {
				const LineColliderSegment& collider = lineColliders[i];
				if(collider.exclusionSet[particleType.id().value()] ||
					t < collider.start ||
					t > collider.start + collider.duration && !collider.repeat) {
					continue;
				}

				solve(particleType, particles, p, t, dt, collider);
			}
		}
	}
}

void CollisionSolver::prepare(const Effect& effect) {
	lineColliders.clear();
	planeColliders.clear();
	solverGrid.clear();

	if(effect.is3d()) {
		planeColliders.reserve(effect.colliders().count());
		for(const Collider& collider : effect.colliders()) {
			for(std::size_t i = 0u; i + 1u < collider.points().size(); i++) {
				planeColliders.emplace_back(collider, i);
			}
		}
	}
	else if(effect.colliders().count() > 0u) {
		lineColliders.reserve(effect.colliders().count());
		for(const Collider& collider : effect.colliders()) {
			for(std::size_t i = 0u; i + 1u < collider.points().size(); i++) {
				lineColliders.emplace_back(collider, i);
			}
		}

		solverGridBottom = float2_t(+FLT_MAX, +FLT_MAX);
		solverGridTop = float2_t(-FLT_MAX, -FLT_MAX);
		for(const LineColliderSegment& collider : lineColliders) {
			solverGridBottom = glm::min(solverGridBottom, glm::min(collider.startPoint, collider.endPoint));
			solverGridTop = glm::max(solverGridTop, glm::max(collider.startPoint, collider.endPoint));
		}

		solverGridBottom -= float2_t(solverGridPadding);
		solverGridTop += float2_t(solverGridPadding);

		uint32_t solverGridNumColumns = std::max(solverGridCellCountFactor * static_cast<uint32_t>(
			(solverGridTop.x - solverGridBottom.x) * std::sqrt(static_cast<float_t>(lineColliders.size()) / ((solverGridTop.x - solverGridBottom.x) * (solverGridTop.y - solverGridBottom.y)))),
				1u);
		uint32_t solverGridNumRows = std::max(solverGridCellCountFactor * static_cast<uint32_t>(
			(solverGridTop.y - solverGridBottom.y) * std::sqrt(static_cast<float_t>(lineColliders.size()) / ((solverGridTop.x - solverGridBottom.x) * (solverGridTop.y - solverGridBottom.y)))),
				1u);
		solverGrid.resize(solverGridNumColumns, solverGridNumRows);
		solverGrid.clear();

		solverGridCellDimension = (solverGridTop - solverGridBottom) / float2_t(solverGridNumColumns, solverGridNumRows);

		for(uint32_t i = 0u; i < lineColliders.size(); i++) {
			const LineColliderSegment& collider = lineColliders[i];
			float2_t direction = glm::normalize(collider.endPoint - collider.startPoint);
			float2_t p1Grid = collider.startPoint - solverGridBottom;
			float2_t p2Grid = collider.endPoint - solverGridBottom;
			float2_t delta = float2_t(0.0);
			float_t tX = 0.0;
			float_t tY = 0.0;

			if(direction.x < 0.0) {
				delta.x = -solverGridCellDimension.x / direction.x;
				tX = (std::floor(p1Grid.x / solverGridCellDimension.x) * solverGridCellDimension.x - p1Grid.x) / direction.x;
			}
			else {
				delta.x = +solverGridCellDimension.x / direction.x;
				tX = ((std::floor(p1Grid.x / solverGridCellDimension.x) + 1) * solverGridCellDimension.x - p1Grid.x) / direction.x;
			}
			if(direction.y < 0.0) {
				delta.y = -solverGridCellDimension.y / direction.y;
				tY = (std::floor(p1Grid.y / solverGridCellDimension.y) * solverGridCellDimension.y - p1Grid.y) / direction.y;
			}
			else {
				delta.y = +solverGridCellDimension.y / direction.y;
				tY = ((std::floor(p1Grid.y / solverGridCellDimension.y) + 1) * solverGridCellDimension.y - p1Grid.y) / direction.y;
			}

			int32_t cX = std::min(std::max(static_cast<int32_t>(std::floor(p1Grid.x / solverGridCellDimension.x)), 0), static_cast<int32_t>(solverGridNumColumns) - 1);
			int32_t cY = std::min(std::max(static_cast<int32_t>(std::floor(p1Grid.y / solverGridCellDimension.y)), 0), static_cast<int32_t>(solverGridNumRows) - 1);
			int32_t exitCX = std::min(std::max(static_cast<int32_t>(std::floor(p2Grid.x / solverGridCellDimension.x)), 0), static_cast<int32_t>(solverGridNumColumns) - 1);
			int32_t exitCY = std::min(std::max(static_cast<int32_t>(std::floor(p2Grid.y / solverGridCellDimension.y)), 0), static_cast<int32_t>(solverGridNumRows) - 1);

			while(solverGrid.contains(cX, cY)) {
				solverGrid(cX, cY).push_back(i);

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

void CollisionSolver::gridCellCountFactor(uint32_t factor) {
	solverGridCellCountFactor = factor;
}
void CollisionSolver::gridPadding(float_t padding) {
	solverGridPadding = padding;
}

CollisionSolver::Intersection::Intersection() {

}
CollisionSolver::Intersection::Intersection(const float3_t& p) : hit(true), point(p) {

}

bool CollisionSolver::isPointOnLineSegment(const float2_t& p, const float2_t& l1, const float2_t& l2) {
	float_t A = glm::dot(l2 - l1, p - l1);
	float_t B = glm::dot(l2 - l1, l2 - l1);

	return A >= 0.0 && A < B;
}
bool CollisionSolver::isPointOnLineSegment(const float3_t& p, const float3_t& l1, const float3_t& l2) {
	float_t A = glm::dot(l2 - l1, p - l1);
	float_t B = glm::dot(l2 - l1, l2 - l1);

	return A >= 0.0 && A < B;
}
bool CollisionSolver::isPointOnCollider(const float3_t& p, const PlaneColliderSegment& collider) {
	float3_t vertices[4] = {
		collider.center - collider.rightVector - collider.upVector,
		collider.center + collider.rightVector - collider.upVector,
		collider.center - collider.rightVector + collider.upVector,
		collider.center + collider.rightVector + collider.upVector,
	};

	float3_t V1ToV0 = vertices[0] - vertices[1];
	float3_t V1ToV3 = vertices[3] - vertices[1];
	float3_t V2ToV0 = vertices[0] - vertices[2];
	float3_t V2ToV3 = vertices[3] - vertices[2];
	float3_t V1ToP = p - vertices[1];
	float3_t V2ToP = p - vertices[2];

	return
		glm::dot(V1ToP, V1ToV0) > 0.0 &&
		glm::dot(V1ToP, V1ToV3) > 0.0 &&
		glm::dot(V2ToP, V2ToV0) > 0.0 &&
		glm::dot(V2ToP, V2ToV3) > 0.0;
}

float2_t CollisionSolver::calculateClosestPointOnLine(const float2_t& p, const LineColliderSegment& collider) {
	float_t A1 = collider.endPoint.y - collider.startPoint.y;
	float_t B1 = collider.startPoint.x - collider.endPoint.x;
	float_t C1 = (collider.endPoint.y - collider.startPoint.y) * collider.startPoint.x + (collider.startPoint.x - collider.endPoint.x) * collider.startPoint.y;
	float_t C2 = -B1 * p.x + A1 * p.y;
	float_t det = A1 * A1 - -B1 * B1;

	return (det != 0.0)
		? float2_t(
			(A1 * C1 - B1 * C2) / det,
			(A1 * C2 - -B1 * C1) / det)
		: p;
}
float3_t CollisionSolver::calculateClosestPointOnPlane(const float3_t& p, const PlaneColliderSegment& collider) {
	float_t signedDistance = glm::dot(collider.normal, p - collider.center);

	return p - collider.normal * signedDistance;
}
CollisionSolver::Intersection CollisionSolver::calculateRayColliderIntersection(const LineColliderSegment& collider, const float2_t& rayOrigin, const float2_t& rayEnd) {
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

	float2_t point = float2_t(
		(B2 * C1 - B1 * C2) / det,
		(A1 * C2 - A2 * C1) / det);

	if(!isPointOnLineSegment(point, collider.startPoint, collider.endPoint) || !isPointOnLineSegment(point, rayOrigin, rayEnd)) {
		return Intersection();
	}

	return Intersection(float3_t(point, 0.0));
}
CollisionSolver::Intersection CollisionSolver::calculateRayColliderIntersection(const PlaneColliderSegment& collider, const float3_t& rayOrigin, const float3_t& rayEnd) {
	float3_t rayDirection = glm::normalize(rayEnd - rayOrigin);

	float_t det = glm::dot(collider.normal, rayDirection);
	if(std::abs(det) < 0.00001) {
		return Intersection();
	}

	float_t t = -glm::dot(collider.normal, rayOrigin - collider.center) / det;
	float3_t point = rayOrigin + rayDirection * t;

	if(!isPointOnCollider(point, collider) || !isPointOnLineSegment(point, rayOrigin, rayEnd)) {
		return Intersection();
	}

	return Intersection(point);
}

void CollisionSolver::solve(const ParticleType& particleType, ParticleCollection::WritePtr particles, uint32_t p, float_t t, float_t dt, const LineColliderSegment& collider) const {
	float2_t closestPoint = calculateClosestPointOnLine(particles.globalPosition[p], collider);
	if(!isPointOnLineSegment(closestPoint, collider.startPoint, collider.endPoint)) {
		return;
	}

	float_t colliderLife = std::fmod(t - collider.start, collider.duration) / collider.duration;

	float2_t globalParticlePosition = float2_t(particles.globalPosition[p]);
	float2_t globalParticleVelocity = float2_t(particles.velocity[p]);
	float2_t globalParticleForce = float2_t(particles.force[p]);
	float2_t colliderToParticle = globalParticlePosition - closestPoint;
	float_t distanceSqr = glm::length2(colliderToParticle);
	float_t particleRadius = std::min(particles.size[p].x, particles.size[p].y) * 0.5;
	particleRadius = std::max(particleRadius * particleType.physicalSize().at(particles.life[p]), 0.01);

	if(distanceSqr <= particleRadius * particleRadius) {
		if(collider.killOnContact) {
			particles.life[p] = 1.0;
		}
		else {
			float_t distance = std::sqrt(distanceSqr);
			float2_t normal = glm::normalize(colliderToParticle);
			float2_t reflectedVelocity = glm::reflect(globalParticleVelocity, normal);
			float2_t segmentVector = glm::normalize(collider.endPoint - collider.startPoint);
			float_t slideFactor = glm::dot((globalParticleForce != float2_t(0.0)) ? glm::normalize(globalParticleForce) : float2_t(0.0, 1.0), segmentVector) * glm::length(globalParticleForce) * distance / particleRadius;
			float_t bounce = collider.bounce.at(colliderLife) * particleType.bounce().at(particles.life[p]);
			float_t friction = std::min(collider.friction.at(colliderLife) * particleType.friction().at(particles.life[p]), 1.0);

			particles.velocity[p] = float3_t(reflectedVelocity * bounce, particles.velocity[p].z);
			particles.velocity[p] += float3_t(segmentVector * slideFactor * (1.0 - friction), 0.0);
			particles.position[p] += float3_t(normal * (particleRadius - distance), 0.0);
		}
	}
	else {
		Intersection intersection = calculateRayColliderIntersection(collider, globalParticlePosition, globalParticlePosition + globalParticleVelocity * dt + globalParticleForce * dt * dt);

		if(intersection.hit) {
			if(collider.killOnContact) {
				particles.life[p] = 1.0;
			}
			else {
				float2_t normal = glm::normalize(colliderToParticle);
				float2_t reflectedVelocity = glm::reflect(globalParticleVelocity, normal);
				float_t bounce = collider.bounce.at(colliderLife) * particleType.bounce().at(particles.life[p]);

				particles.velocity[p] = float3_t(reflectedVelocity * bounce, particles.velocity[p].z);
			}
		}
	}
}
void CollisionSolver::solve(const ParticleType& particleType, ParticleCollection::WritePtr particles, uint32_t p, float_t t, float_t dt, const PlaneColliderSegment& collider) const {
	float3_t closestPoint = calculateClosestPointOnPlane(particles.globalPosition[p], collider);
	if(!isPointOnCollider(closestPoint, collider)) {
		return;
	}

	float_t colliderLife = std::fmod(t - collider.start, collider.duration) / collider.duration;
	float3_t globalParticlePosition = particles.globalPosition[p];
	float3_t globalParticleVelocity = particles.velocity[p];
	float3_t globalParticleForce = particles.force[p];
	float3_t normalizedParticleForce = (globalParticleForce != float3_t(0.0))
		? glm::normalize(globalParticleForce)
		: float3_t(0.0, 1.0, 0.0);
	float3_t colliderToParticle = globalParticlePosition - closestPoint;
	float3_t colliderNormal = glm::normalize(colliderToParticle);
	float_t distanceSqr = glm::length2(colliderToParticle);
	float_t particleRadius = std::min(particles.size[p].x, std::min(particles.size[p].y, particles.size[p].z)) * 0.5;
	particleRadius = std::max(particleRadius * particleType.physicalSize().at(particles.life[p]), 0.01);

	if(distanceSqr <= particleRadius * particleRadius) {
		if(collider.killOnContact) {
			particles.life[p] = 1.0;
		}
		else {
			float_t distance = std::sqrt(distanceSqr);
			float3_t reflectedVelocity = glm::reflect(globalParticleVelocity, colliderNormal);

			float3_t slideVector = calculateClosestPointOnPlane(closestPoint + normalizedParticleForce, collider) - closestPoint;
			if(slideVector != float3_t(0.0)) {
				slideVector = glm::normalize(slideVector);
			}

			float_t slideFactor = glm::dot(normalizedParticleForce, slideVector) * glm::length(globalParticleForce) * distance / particleRadius;
			float_t bounce = collider.bounce.at(colliderLife) * particleType.bounce().at(particles.life[p]);
			float_t friction = std::min(collider.friction.at(colliderLife) * particleType.friction().at(particles.life[p]), 1.0);

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
				float3_t reflectedVelocity = glm::reflect(globalParticleVelocity, colliderNormal);
				float_t bounce = collider.bounce.at(colliderLife) * particleType.bounce().at(particles.life[p]);

				particles.velocity[p] = reflectedVelocity * bounce;
			}
		}
	}
}

GridIndex<int32_t> CollisionSolver::toGridIndex(const float3_t& position) const {
	int32_t x = std::min(std::max(static_cast<int32_t>(std::floor((position.x - solverGridBottom.x) / solverGridCellDimension.x)), 0), static_cast<int32_t>(solverGrid.width()) - 1);
	int32_t y = std::min(std::max(static_cast<int32_t>(std::floor((position.y - solverGridBottom.y) / solverGridCellDimension.y)), 0), static_cast<int32_t>(solverGrid.height()) - 1);

	return GridIndex<int32_t>{ x, y };
}

void CollisionSolver::findPotentialColliders(std::unordered_set<uint32_t>& potentialColliders, int32_t cx, int32_t cy, const float2_t& size) const {
	int32_t radiusX = 1 + static_cast<int32_t>(size.x / solverGridCellDimension.x);
	int32_t radiusY = 1 + static_cast<int32_t>(size.y / solverGridCellDimension.y);

	for(int32_t y = cy - radiusY; y <= cy + radiusY && y < static_cast<int32_t>(solverGrid.height()); y++) {
		for(int32_t x = cx - radiusX; x <= cx + radiusX && x < static_cast<int32_t>(solverGrid.width()); x++) {
			if(x < 0 || y < 0) {
				continue;
			}

			for(uint32_t i : solverGrid(x, y)) {
				potentialColliders.insert(i);
			}
		}
	}
}
}