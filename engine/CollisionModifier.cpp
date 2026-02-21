#include "CollisionModifier.h"
#include "../common/Id.h"
#include "../math/Common.h"
#include "../math/Geometry.h"
#include "../effect/ParticleType.h"
#include <cmath>
#include <algorithm>

namespace pixelpart {
void CollisionModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);
	float_t t = runtimeContext.time();
	float_t dt = runtimeContext.deltaTime();

	if(!modifierLine2dColliders.empty()) {
		for(std::uint32_t p = 0; p < particleCount; p++) {
			LineQueryGrid::QueryResult potentialColliderIndices = modifierLine2dColliderGrid.queryLine(
				float2_t(particles.globalPosition[p]),
				float2_t(particles.globalPosition[p] + particles.velocity[p] * dt + particles.force[p] * dt * dt),
				float2_t(particles.size[p] * particleType.physicalSize().at(particles.life[p])));

			for(std::uint32_t colliderIndex : potentialColliderIndices) {
				const Line2dColliderObject& collider = modifierLine2dColliders[colliderIndex];
				if(collider.exclusionSet[particleType.id().value()]) {
					continue;
				}

				collide(particleType, particles, p, t, dt, collider);
			}
		}
	}

	for(const Plane3dColliderObject& planeCollider : modifierPlane3dColliders) {
		if(planeCollider.exclusionSet[particleType.id().value()]) {
			continue;
		}

		for(std::uint32_t p = 0; p < particleCount; p++) {
			collide(particleType, particles, p, t, dt, planeCollider);
		}
	}
}

void CollisionModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {
	modifierLine2dColliders.clear();
	modifierPlane3dColliders.clear();
	modifierLine2dColliderGrid.clear();

	if(effect->is3d()) {
		for(const Collider* collider : effect->sceneGraph().nodesWithType<Collider>()) {
			if(!collider->active(runtimeContext)) {
				continue;
			}

			const PlaneCollider* planeCollider = dynamic_cast<const PlaneCollider*>(collider);

			if(planeCollider) {
				Transform transform = effect->sceneGraph().globalTransform(collider->id(), runtimeContext);
				modifierPlane3dColliders.emplace_back(*planeCollider, transform);
			}
		}
	}
	else {
		for(const Collider* collider : effect->sceneGraph().nodesWithType<Collider>()) {
			if(!collider->active(runtimeContext)) {
				continue;
			}

			const LineCollider* lineCollider = dynamic_cast<const LineCollider*>(collider);
			const PlaneCollider* planeCollider = dynamic_cast<const PlaneCollider*>(collider);

			Transform transform = effect->sceneGraph().globalTransform(collider->id(), runtimeContext);

			if(lineCollider) {
				for(std::size_t segmentIndex = 0; segmentIndex + 1 < lineCollider->points().size(); segmentIndex++) {
					modifierLine2dColliders.emplace_back(*lineCollider, transform, segmentIndex);
				}
			}
			else if(planeCollider) {
				modifierLine2dColliders.emplace_back(*planeCollider, transform);
			}
		}

		std::vector<LineQueryGrid::Line> lines;
		lines.reserve(modifierLine2dColliders.size());

		for(const Line2dColliderObject& collider : modifierLine2dColliders) {
			lines.emplace_back(collider.start, collider.end);
		}

		modifierLine2dColliderGrid.build(lines);
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

CollisionModifier::Line2dColliderObject::Line2dColliderObject(const LineCollider& collider, const Transform& transform, std::size_t segmentIndex) : ColliderObject(collider) {
	start = float2_t(transform * float4_t(collider.point(segmentIndex), 1.0));
	end = float2_t(transform * float4_t(collider.point(segmentIndex + 1), 1.0));
}
CollisionModifier::Line2dColliderObject::Line2dColliderObject(const PlaneCollider& collider, const Transform& transform) : ColliderObject(collider) {
	start = float2_t(transform * float4_t(-0.5, 0.0, 0.0, 1.0));
	end = float2_t(transform * float4_t(+0.5, 0.0, 0.0, 1.0));
}

CollisionModifier::Plane3dColliderObject::Plane3dColliderObject(const PlaneCollider& collider, const Transform& transform) : ColliderObject(collider) {
	center = float3_t(transform * float4_t(0.0, 0.0, 0.0, 1.0));
	normal = float3_t(transform * float4_t(0.0, 1.0, 0.0, 0.0));

	vertices = std::array<float3_t, 4>{
		float3_t(transform * float4_t(-0.5, 0.0, -0.5, 1.0)),
		float3_t(transform * float4_t(+0.5, 0.0, -0.5, 1.0)),
		float3_t(transform * float4_t(-0.5, 0.0, +0.5, 1.0)),
		float3_t(transform * float4_t(+0.5, 0.0, +0.5, 1.0))
	};
}

bool CollisionModifier::isPointOnLineSegment(const float2_t& p, const float2_t& l1, const float2_t& l2) {
	float_t a = math::dot(l2 - l1, p - l1);
	float_t b = math::dot(l2 - l1, l2 - l1);

	return a >= 0.0 && a < b;
}
bool CollisionModifier::isPointOnLineSegment(const float3_t& p, const float3_t& l1, const float3_t& l2) {
	float_t a = math::dot(l2 - l1, p - l1);
	float_t b = math::dot(l2 - l1, l2 - l1);

	return a >= 0.0 && a < b;
}
bool CollisionModifier::isPointOnCollider(const float3_t& p, const Plane3dColliderObject& collider) {
	float3_t v1ToV0 = collider.vertices[0] - collider.vertices[1];
	float3_t v1ToV3 = collider.vertices[3] - collider.vertices[1];
	float3_t v2ToV0 = collider.vertices[0] - collider.vertices[2];
	float3_t v2ToV3 = collider.vertices[3] - collider.vertices[2];
	float3_t v1ToP = p - collider.vertices[1];
	float3_t v2ToP = p - collider.vertices[2];

	return
		math::dot(v1ToP, v1ToV0) > 0.0 &&
		math::dot(v1ToP, v1ToV3) > 0.0 &&
		math::dot(v2ToP, v2ToV0) > 0.0 &&
		math::dot(v2ToP, v2ToV3) > 0.0;
}

float2_t CollisionModifier::closestPointOnLine(const float2_t& p, const Line2dColliderObject& collider) {
	float_t a1 = collider.end.y - collider.start.y;
	float_t b1 = collider.start.x - collider.end.x;
	float_t c1 = (collider.end.y - collider.start.y) * collider.start.x + (collider.start.x - collider.end.x) * collider.start.y;
	float_t c2 = -b1 * p.x + a1 * p.y;
	float_t det = a1 * a1 - -b1 * b1;

	return (det != 0.0)
		? float2_t(
			(a1 * c1 - b1 * c2) / det,
			(a1 * c2 - -b1 * c1) / det)
		: p;
}
float3_t CollisionModifier::closestPointOnPlane(const float3_t& p, const Plane3dColliderObject& collider) {
	float_t signedDistance = math::dot(collider.normal, p - collider.center);

	return p - collider.normal * signedDistance;
}
std::optional<float3_t> CollisionModifier::rayColliderIntersection(const Line2dColliderObject& collider, const float2_t& rayOrigin, const float2_t& rayEnd) {
	float_t a1 = collider.end.y - collider.start.y;
	float_t b1 = collider.start.x - collider.end.x;
	float_t c1 = a1 * collider.start.x + b1 * collider.start.y;
	float_t a2 = rayEnd.y - rayOrigin.y;
	float_t b2 = rayOrigin.x - rayEnd.x;
	float_t c2 = a2 * rayOrigin.x + b2 * rayOrigin.y;
	float_t det = a1 * b2 - a2 * b1;

	if(std::abs(det) < 0.00001) {
		return std::nullopt;
	}

	float2_t point = float2_t(
		(b2 * c1 - b1 * c2) / det,
		(a1 * c2 - a2 * c1) / det);

	if(!isPointOnLineSegment(point, collider.start, collider.end) || !isPointOnLineSegment(point, rayOrigin, rayEnd)) {
		return std::nullopt;
	}

	return float3_t(point, 0.0);
}
std::optional<float3_t> CollisionModifier::rayColliderIntersection(const Plane3dColliderObject& collider, const float3_t& rayOrigin, const float3_t& rayEnd) {
	float3_t rayDirection = math::normalize(rayEnd - rayOrigin);

	float_t det = math::dot(collider.normal, rayDirection);
	if(std::abs(det) < 0.00001) {
		return std::nullopt;
	}

	float_t t = -math::dot(collider.normal, rayOrigin - collider.center) / det;
	float3_t point = rayOrigin + rayDirection * t;

	if(!isPointOnCollider(point, collider) || !isPointOnLineSegment(point, rayOrigin, rayEnd)) {
		return std::nullopt;
	}

	return point;
}

void CollisionModifier::collide(const ParticleType& particleType, ParticleCollection::WritePtr particles, std::uint32_t p, float_t t, float_t dt, const Line2dColliderObject& collider) const {
	float2_t closestPoint = closestPointOnLine(float2_t(particles.globalPosition[p]), collider);
	if(!isPointOnLineSegment(closestPoint, collider.start, collider.end)) {
		return;
	}

	float_t colliderLife = std::fmod(t - collider.startTime, collider.duration) / collider.duration;

	float2_t globalParticlePosition = float2_t(particles.globalPosition[p]);
	float2_t globalParticleVelocity = float2_t(particles.velocity[p]);
	float2_t globalParticleForce = float2_t(particles.force[p]);
	float2_t colliderToParticle = globalParticlePosition - closestPoint;
	float_t distanceSqr = math::lengthSquared(colliderToParticle);
	float_t particleRadius = std::min(particles.size[p].x, particles.size[p].y) * 0.5;
	particleRadius = std::max(particleRadius * particleType.physicalSize().at(particles.life[p]), 0.01);

	if(distanceSqr <= particleRadius * particleRadius) {
		if(collider.killOnContact) {
			particles.life[p] = 1.0;
		}
		else {
			float_t distance = std::sqrt(distanceSqr);
			float2_t normal = math::normalize(colliderToParticle);
			float2_t reflectedVelocity = math::reflect(globalParticleVelocity, normal);
			float2_t segmentVector = math::normalize(collider.end - collider.start);
			float_t slideFactor = math::dot((globalParticleForce != float2_t(0.0)) ? math::normalize(globalParticleForce) : float2_t(0.0, 1.0), segmentVector) * math::length(globalParticleForce) * distance / particleRadius;
			float_t bounce = collider.bounce.at(colliderLife) * particleType.bounce().at(particles.life[p]);
			float_t friction = std::min(collider.friction.at(colliderLife) * particleType.friction().at(particles.life[p]), 1.0);

			particles.velocity[p] = float3_t(reflectedVelocity * bounce, particles.velocity[p].z);
			particles.velocity[p] += float3_t(segmentVector * slideFactor * (1.0 - friction), 0.0);
			particles.position[p] += float3_t(normal * (particleRadius - distance), 0.0);
		}
	}
	else {
		std::optional<float3_t> intersection = rayColliderIntersection(collider, globalParticlePosition, globalParticlePosition + globalParticleVelocity * dt + globalParticleForce * dt * dt);

		if(intersection) {
			if(collider.killOnContact) {
				particles.life[p] = 1.0;
			}
			else {
				float2_t normal = math::normalize(colliderToParticle);
				float2_t reflectedVelocity = math::reflect(globalParticleVelocity, normal);
				float_t bounce = collider.bounce.at(colliderLife) * particleType.bounce().at(particles.life[p]);

				particles.velocity[p] = float3_t(reflectedVelocity * bounce, particles.velocity[p].z);
			}
		}
	}
}
void CollisionModifier::collide(const ParticleType& particleType, ParticleCollection::WritePtr particles, std::uint32_t p, float_t t, float_t dt, const Plane3dColliderObject& collider) const {
	float3_t closestPoint = closestPointOnPlane(particles.globalPosition[p], collider);
	if(!isPointOnCollider(closestPoint, collider)) {
		return;
	}

	float_t colliderLife = std::fmod(t - collider.startTime, collider.duration) / collider.duration;
	float3_t globalParticlePosition = particles.globalPosition[p];
	float3_t globalParticleVelocity = particles.velocity[p];
	float3_t globalParticleForce = particles.force[p];
	float3_t normalizedParticleForce = (globalParticleForce != float3_t(0.0))
		? math::normalize(globalParticleForce)
		: float3_t(0.0, 1.0, 0.0);
	float3_t colliderToParticle = globalParticlePosition - closestPoint;
	float3_t colliderNormal = math::normalize(colliderToParticle);
	float_t distanceSqr = math::lengthSquared(colliderToParticle);
	float_t particleRadius = std::min(particles.size[p].x, std::min(particles.size[p].y, particles.size[p].z)) * 0.5;
	particleRadius = std::max(particleRadius * particleType.physicalSize().at(particles.life[p]), 0.01);

	if(distanceSqr <= particleRadius * particleRadius) {
		if(collider.killOnContact) {
			particles.life[p] = 1.0;
		}
		else {
			float_t distance = std::sqrt(distanceSqr);
			float3_t reflectedVelocity = math::reflect(globalParticleVelocity, colliderNormal);

			float3_t slideVector = closestPointOnPlane(closestPoint + normalizedParticleForce, collider) - closestPoint;
			if(slideVector != float3_t(0.0)) {
				slideVector = math::normalize(slideVector);
			}

			float_t slideFactor = math::dot(normalizedParticleForce, slideVector) * math::length(globalParticleForce) * distance / particleRadius;
			float_t bounce = collider.bounce.at(colliderLife) * particleType.bounce().at(particles.life[p]);
			float_t friction = std::min(collider.friction.at(colliderLife) * particleType.friction().at(particles.life[p]), 1.0);

			particles.velocity[p] = reflectedVelocity * bounce;
			particles.velocity[p] += slideVector * slideFactor * (1.0 - friction);
			particles.position[p] += colliderNormal * (particleRadius - distance);
		}
	}
	else {
		std::optional<float3_t> intersection = rayColliderIntersection(collider, globalParticlePosition, globalParticlePosition + globalParticleVelocity * dt + globalParticleForce * dt * dt);

		if(intersection) {
			if(collider.killOnContact) {
				particles.life[p] = 1.0;
			}
			else {
				float3_t reflectedVelocity = math::reflect(globalParticleVelocity, colliderNormal);
				float_t bounce = collider.bounce.at(colliderLife) * particleType.bounce().at(particles.life[p]);

				particles.velocity[p] = reflectedVelocity * bounce;
			}
		}
	}
}
}
