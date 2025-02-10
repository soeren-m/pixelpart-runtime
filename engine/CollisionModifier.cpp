#include "CollisionModifier.h"
#include "../common/Id.h"
#include "../glm/gtx/norm.hpp"
#include "../glm/gtx/rotate_vector.hpp"
#include <cmath>
#include <algorithm>

namespace pixelpart {
void CollisionModifier::run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const {
	if( !line2dColliders.empty()) {
		for(uint32_t p = 0u; p < particleCount; p++) {
			LineQueryGrid::QueryResult potentialColliderIndices = line2dColliderGrid.queryLine(
				particles.globalPosition[p],
				particles.globalPosition[p] + particles.velocity[p] * dt + particles.force[p] * dt * dt,
				particles.size[p] * particleType.physicalSize().at(particles.life[p]));

			for(uint32_t colliderIndex : potentialColliderIndices) {
				const Line2dColliderObject& collider = line2dColliders[colliderIndex];
				if(collider.exclusionSet[particleType.id().value()] ||
					(t < collider.startTime) ||
					(t > collider.startTime + collider.duration && !collider.repeat)) {
					continue;
				}

				collide(particleType, particles, p, t, dt, collider);
			}
		}
	}

	for(const Plane3dColliderObject& planeCollider : plane3dColliders) {
		if(planeCollider.exclusionSet[particleType.id().value()] ||
			(t < planeCollider.startTime) ||
			(t > planeCollider.startTime + planeCollider.duration && !planeCollider.repeat)) {
			continue;
		}

		for(uint32_t p = 0u; p < particleCount; p++) {
			collide(particleType, particles, p, t, dt, planeCollider);
		}
	}
}

void CollisionModifier::prepare(const Effect& effect, float_t t) {
	line2dColliders.clear();
	plane3dColliders.clear();
	line2dColliderGrid.clear();

	if(effect.is3d()) {

		for(const Collider* collider : effect.sceneGraph().nodesWithType<Collider>()) {
			const PlaneCollider* planeCollider = dynamic_cast<const PlaneCollider*>(collider);

			if(planeCollider) {
				NodeTransform transform = effect.sceneGraph().globalTransform(collider->id(), t);
				plane3dColliders.emplace_back(*planeCollider, transform);
			}
		}
	}
	else {
		for(const Collider* collider : effect.sceneGraph().nodesWithType<Collider>()) {
			const LineCollider* lineCollider = dynamic_cast<const LineCollider*>(collider);
			const PlaneCollider* planeCollider = dynamic_cast<const PlaneCollider*>(collider);

			NodeTransform transform = effect.sceneGraph().globalTransform(collider->id(), t);

			if(lineCollider) {
				for(std::size_t segmentIndex = 0u; segmentIndex + 1u < lineCollider->points().size(); segmentIndex++) {
					line2dColliders.emplace_back(*lineCollider, transform, segmentIndex);
				}
			}
			else if(planeCollider) {
				line2dColliders.emplace_back(*planeCollider, transform);
			}
		}

		std::vector<LineQueryGrid::Line> lines;
		lines.reserve(line2dColliders.size());

		for(const Line2dColliderObject& collider : line2dColliders) {
			lines.emplace_back(collider.start, collider.end);
		}

		line2dColliderGrid.build(lines);
	}
}

CollisionModifier::ColliderObject::ColliderObject(const Collider& collider) :
	startTime(collider.start()), duration(collider.duration()), repeat(collider.repeat()),
	killOnContact(collider.killOnContact().value()),
	bounce(collider.bounce().resultCurve()),
	friction(collider.friction().resultCurve()) {
	for(id_t particleTypeId : collider.exclusionSet()) {
		exclusionSet.set(particleTypeId.value());
	}
}

CollisionModifier::Line2dColliderObject::Line2dColliderObject(const LineCollider& collider, const NodeTransform& transform, std::size_t segmentIndex) : ColliderObject(collider) {
	start = float2_t(transform * float4_t(collider.point(segmentIndex), 1.0));
	end = float2_t(transform * float4_t(collider.point(segmentIndex + 1u), 1.0));
}
CollisionModifier::Line2dColliderObject::Line2dColliderObject(const PlaneCollider& collider, const NodeTransform& transform) : ColliderObject(collider) {
	start = float2_t(transform * float4_t(-0.5, 0.0, 0.0, 1.0));
	end = float2_t(transform * float4_t(+0.5, 0.0, 0.0, 1.0));
}

CollisionModifier::Plane3dColliderObject::Plane3dColliderObject(const PlaneCollider& collider, const NodeTransform& transform) : ColliderObject(collider) {
	float3_t start = float3_t(transform * float4_t(-0.5, 0.0, 0.0, 1.0));
	float3_t end = float3_t(transform * float4_t(+0.5, 0.0, 0.0, 1.0));

	center = (start + end) * 0.5;
	rightVector = end - center;

	normal = float3_t(1.0, 0.0, 0.0);
	if(std::abs(rightVector.x) > 0.001) {
		normal = glm::normalize(float3_t(-rightVector.y / rightVector.x, 1.0, 0.0));
	}
	else if(std::abs(rightVector.z) > 0.001) {
		normal = glm::normalize(float3_t(0.0, 1.0, -rightVector.y / rightVector.z));
	}

	upVector = glm::cross(glm::normalize(rightVector), normal);
}

CollisionModifier::Intersection::Intersection() {

}
CollisionModifier::Intersection::Intersection(const float3_t& p) : hit(true), point(p) {

}

bool CollisionModifier::isPointOnLineSegment(const float2_t& p, const float2_t& l1, const float2_t& l2) {
	float_t A = glm::dot(l2 - l1, p - l1);
	float_t B = glm::dot(l2 - l1, l2 - l1);

	return A >= 0.0 && A < B;
}
bool CollisionModifier::isPointOnLineSegment(const float3_t& p, const float3_t& l1, const float3_t& l2) {
	float_t A = glm::dot(l2 - l1, p - l1);
	float_t B = glm::dot(l2 - l1, l2 - l1);

	return A >= 0.0 && A < B;
}
bool CollisionModifier::isPointOnCollider(const float3_t& p, const Plane3dColliderObject& collider) {
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

float2_t CollisionModifier::calculateClosestPointOnLine(const float2_t& p, const Line2dColliderObject& collider) {
	float_t A1 = collider.end.y - collider.start.y;
	float_t B1 = collider.start.x - collider.end.x;
	float_t C1 = (collider.end.y - collider.start.y) * collider.start.x + (collider.start.x - collider.end.x) * collider.start.y;
	float_t C2 = -B1 * p.x + A1 * p.y;
	float_t det = A1 * A1 - -B1 * B1;

	return (det != 0.0)
		? float2_t(
			(A1 * C1 - B1 * C2) / det,
			(A1 * C2 - -B1 * C1) / det)
		: p;
}
float3_t CollisionModifier::calculateClosestPointOnPlane(const float3_t& p, const Plane3dColliderObject& collider) {
	float_t signedDistance = glm::dot(collider.normal, p - collider.center);

	return p - collider.normal * signedDistance;
}
CollisionModifier::Intersection CollisionModifier::calculateRayColliderIntersection(const Line2dColliderObject& collider, const float2_t& rayOrigin, const float2_t& rayEnd) {
	float_t A1 = collider.end.y - collider.start.y;
	float_t B1 = collider.start.x - collider.end.x;
	float_t C1 = A1 * collider.start.x + B1 * collider.start.y;
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

	if(!isPointOnLineSegment(point, collider.start, collider.end) || !isPointOnLineSegment(point, rayOrigin, rayEnd)) {
		return Intersection();
	}

	return Intersection(float3_t(point, 0.0));
}
CollisionModifier::Intersection CollisionModifier::calculateRayColliderIntersection(const Plane3dColliderObject& collider, const float3_t& rayOrigin, const float3_t& rayEnd) {
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

void CollisionModifier::collide(const ParticleType& particleType, ParticleCollection::WritePtr particles, uint32_t p, float_t t, float_t dt, const Line2dColliderObject& collider) const {
	float2_t closestPoint = calculateClosestPointOnLine(particles.globalPosition[p], collider);
	if(!isPointOnLineSegment(closestPoint, collider.start, collider.end)) {
		return;
	}

	float_t colliderLife = std::fmod(t - collider.startTime, collider.duration) / collider.duration;

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
			float2_t segmentVector = glm::normalize(collider.end - collider.start);
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
void CollisionModifier::collide(const ParticleType& particleType, ParticleCollection::WritePtr particles, uint32_t p, float_t t, float_t dt, const Plane3dColliderObject& collider) const {
	float3_t closestPoint = calculateClosestPointOnPlane(particles.globalPosition[p], collider);
	if(!isPointOnCollider(closestPoint, collider)) {
		return;
	}

	float_t colliderLife = std::fmod(t - collider.startTime, collider.duration) / collider.duration;
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
}