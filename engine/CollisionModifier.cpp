#include "CollisionModifier.h"
#include "../effect/Coordinates.h"
#include "../types/Id.h"
#include "../math/Common.h"
#include "../math/Geometry.h"
#include "../math/MatrixCommon.h"
#include "../math/Transformation.h"
#include <cmath>
#include <algorithm>

namespace pixelpart {
void CollisionModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const ParticleEmitter& particleEmitter = effect->sceneGraph().at<ParticleEmitter>(particleEmitterId);
	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);

	calculateCollisions2d(particles, particleCount, effect, particleEmitter, particleType, runtimeContext);
	calculateCollisions3d(particles, particleCount, effect, particleEmitter, particleType, runtimeContext);
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

		for(const Line2dCollisionObject& collider : modifierLine2dColliders) {
			lines.emplace_back(collider.start, collider.end);
		}

		modifierLine2dColliderGrid.build(lines);
	}
}

CollisionModifier::CollisionObject::CollisionObject(const Collider& collider) :
	startTime(collider.lifetimeStart()), duration(collider.lifetimeDuration()), repeat(collider.repeat()),
	killOnContact(collider.killOnContact().value()),
	bounce(collider.bounce().resultCurve()),
	friction(collider.friction().resultCurve()) {
	for(id_t particleTypeId : collider.exclusionSet()) {
		exclusionSet.set(particleTypeId.value());
	}
}

CollisionModifier::Line2dCollisionObject::Line2dCollisionObject(const LineCollider& collider, const Transform& transform, std::size_t segmentIndex) : CollisionObject(collider) {
	start = float2_t(transform * float4_t(collider.point(segmentIndex), 1.0));
	end = float2_t(transform * float4_t(collider.point(segmentIndex + 1), 1.0));
}
CollisionModifier::Line2dCollisionObject::Line2dCollisionObject(const PlaneCollider& collider, const Transform& transform) : CollisionObject(collider) {
	start = float2_t(transform * float4_t(-0.5, 0.0, 0.0, 1.0));
	end = float2_t(transform * float4_t(+0.5, 0.0, 0.0, 1.0));
}

CollisionModifier::Plane3dCollisionObject::Plane3dCollisionObject(const PlaneCollider& collider, const Transform& transform) : CollisionObject(collider) {
	center = float3_t(transform * float4_t(0.0, 0.0, 0.0, 1.0));
	normal = float3_t(transform * float4_t(0.0, 1.0, 0.0, 0.0));

	vertices = std::array<float3_t, 4>{
		float3_t(transform * float4_t(-0.5, 0.0, -0.5, 1.0)),
		float3_t(transform * float4_t(+0.5, 0.0, -0.5, 1.0)),
		float3_t(transform * float4_t(-0.5, 0.0, +0.5, 1.0)),
		float3_t(transform * float4_t(+0.5, 0.0, +0.5, 1.0))
	};
}

bool CollisionModifier::isPointOnLineSegment(const float2_t& p, const float2_t& lineStart, const float2_t& lineEnd) {
	float_t a = math::dot(lineEnd - lineStart, p - lineStart);
	float_t b = math::dot(lineEnd - lineStart, lineEnd - lineStart);

	return a >= 0.0 && a < b;
}
bool CollisionModifier::isPointOnLineSegment(const float3_t& p, const float3_t& lineStart, const float3_t& lineEnd) {
	float_t a = math::dot(lineEnd - lineStart, p - lineStart);
	float_t b = math::dot(lineEnd - lineStart, lineEnd - lineStart);

	return a >= 0.0 && a < b;
}
bool CollisionModifier::isPointOnPlaneSegment(const float3_t& p, const std::array<float3_t, 4>& planeVertices) {
	float3_t v1ToV0 = planeVertices[0] - planeVertices[1];
	float3_t v1ToV3 = planeVertices[3] - planeVertices[1];
	float3_t v2ToV0 = planeVertices[0] - planeVertices[2];
	float3_t v2ToV3 = planeVertices[3] - planeVertices[2];
	float3_t v1ToP = p - planeVertices[1];
	float3_t v2ToP = p - planeVertices[2];

	return
		math::dot(v1ToP, v1ToV0) > 0.0 &&
		math::dot(v1ToP, v1ToV3) > 0.0 &&
		math::dot(v2ToP, v2ToV0) > 0.0 &&
		math::dot(v2ToP, v2ToV3) > 0.0;
}

float2_t CollisionModifier::closestPointOnLine(const float2_t& p, const Line2dCollisionObject& collider) {
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
float3_t CollisionModifier::closestPointOnPlane(const float3_t& p, const Plane3dCollisionObject& collider) {
	float_t signedDistance = math::dot(collider.normal, p - collider.center);

	return p - collider.normal * signedDistance;
}
std::optional<float3_t> CollisionModifier::rayColliderIntersection(const Line2dCollisionObject& collider, const float2_t& rayOrigin, const float2_t& rayEnd) {
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
std::optional<float3_t> CollisionModifier::rayColliderIntersection(const Plane3dCollisionObject& collider, const float3_t& rayOrigin, const float3_t& rayEnd) {
	float3_t rayDirection = math::normalize(rayEnd - rayOrigin);

	float_t det = math::dot(collider.normal, rayDirection);
	if(std::abs(det) < 0.00001) {
		return std::nullopt;
	}

	float_t t = -math::dot(collider.normal, rayOrigin - collider.center) / det;
	float3_t point = rayOrigin + rayDirection * t;

	if(!isPointOnPlaneSegment(point, collider.vertices) || !isPointOnLineSegment(point, rayOrigin, rayEnd)) {
		return std::nullopt;
	}

	return point;
}

void CollisionModifier::calculateCollisions2d(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	const EffectRuntimeContext& runtimeContext) const {
	if(modifierLine2dColliders.empty()) {
		return;
	}

	float_t t = runtimeContext.time();
	float_t dt = runtimeContext.deltaTime();

	const Curve<float_t>& particlePhysicalSizeCurve = particleType.physicalSize().resultCurve();
	const Curve<float_t>& particleBounceCurve = particleType.bounce().resultCurve();
	const Curve<float_t>& particleFrictionCurve = particleType.friction().resultCurve();

	matrix4_t emitterGlobalTransform = math::normalizeTransformationMatrix(
		effect->sceneGraph().globalTransform(particleEmitter.id(), runtimeContext).matrix());
	matrix3_t simulationSpaceToGlobalTransform = particleType.simulationSpace() == ParticleSimulationSpace::local
		? matrix3_t(emitterGlobalTransform)
		: matrix3_t(1.0);
	matrix3_t globalToSimulationSpaceTransform = particleType.simulationSpace() == ParticleSimulationSpace::local
		? matrix3_t(math::inverse(emitterGlobalTransform))
		: matrix3_t(1.0);

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float2_t particlePosition2 = float2_t(particles.globalPosition[p]);
		float3_t particleVelocity = simulationSpaceToGlobalTransform * particles.velocity[p];
		float2_t particleVelocity2 = float2_t(particleVelocity);
		float2_t particleForce2 = float2_t(simulationSpaceToGlobalTransform * particles.force[p]);
		float2_t particleSize2 = float2_t(particles.size[p]);
		float_t particlePhysicalSize = particlePhysicalSizeCurve.at(particles.life[p]);

		LineQueryGrid::QueryResult potentialColliderIndices = modifierLine2dColliderGrid.queryLine(
			particlePosition2,
			particlePosition2 + particleVelocity2 * dt + particleForce2 * dt * dt,
			particleSize2 * particlePhysicalSize);

		for(std::uint32_t colliderIndex : potentialColliderIndices) {
			const Line2dCollisionObject& collider = modifierLine2dColliders[colliderIndex];
			if(collider.exclusionSet[particleType.id().value()]) {
				continue;
			}

			float2_t closestPoint = closestPointOnLine(particlePosition2, collider);
			if(!isPointOnLineSegment(closestPoint, collider.start, collider.end)) {
				continue;
			}

			float_t colliderLife = std::fmod(t - collider.startTime, collider.duration) / collider.duration;
			float2_t colliderToParticle = particlePosition2 - closestPoint;
			float2_t colliderNormal = math::normalize(colliderToParticle);
			float_t distanceToColliderSq = math::lengthSquared(colliderToParticle);
			float_t particleRadius = std::min(particleSize2.x, particleSize2.y) * 0.5;
			particleRadius = std::max(particleRadius * particlePhysicalSize, 0.01);

			if(distanceToColliderSq <= particleRadius * particleRadius) {
				float_t particleForceLength = math::length(particleForce2);
				float2_t particleForceDirection = particleForceLength > 0.0
					? particleForce2 / particleForceLength
					: worldUpVector2;

				float_t distanceToCollider = std::sqrt(distanceToColliderSq);
				float2_t reflectedVelocity = math::reflect(particleVelocity2, colliderNormal);

				float2_t slideVector = math::normalize(collider.end - collider.start);
				float_t slideFactor = math::dot(particleForceDirection, slideVector) * particleForceLength * distanceToCollider / particleRadius;

				float_t bounce = collider.bounce.at(colliderLife) * particleBounceCurve.at(particles.life[p]);
				float_t friction = std::min(collider.friction.at(colliderLife) * particleFrictionCurve.at(particles.life[p]), 1.0);

				particles.velocity[p] = globalToSimulationSpaceTransform * (float3_t(reflectedVelocity * bounce, particleVelocity.z) + float3_t(slideVector * slideFactor * (1.0 - friction), 0.0));
				particles.position[p] += globalToSimulationSpaceTransform * float3_t(colliderNormal * (particleRadius - distanceToCollider), 0.0);

				if(collider.killOnContact) {
					particles.life[p] = 1.0;
				}
			}
			else {
				std::optional<float3_t> intersection = rayColliderIntersection(collider,
					particlePosition2, particlePosition2 + particleVelocity2 * dt + particleForce2 * dt * dt);

				if(intersection) {
					float2_t reflectedVelocity = math::reflect(particleVelocity2, colliderNormal);
					float_t bounce = collider.bounce.at(colliderLife) * particleBounceCurve.at(particles.life[p]);

					particles.velocity[p] = globalToSimulationSpaceTransform * float3_t(reflectedVelocity * bounce, particleVelocity.z);

					if(collider.killOnContact) {
						particles.life[p] = 1.0;
					}
				}
			}
		}
	}
}
void CollisionModifier::calculateCollisions3d(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	const EffectRuntimeContext& runtimeContext) const {
	float_t t = runtimeContext.time();
	float_t dt = runtimeContext.deltaTime();

	matrix4_t emitterGlobalTransform = math::normalizeTransformationMatrix(
		effect->sceneGraph().globalTransform(particleEmitter.id(), runtimeContext).matrix());
	matrix3_t simulationSpaceToGlobalTransform = particleType.simulationSpace() == ParticleSimulationSpace::local
		? matrix3_t(emitterGlobalTransform)
		: matrix3_t(1.0);
	matrix3_t globalToSimulationSpaceTransform = particleType.simulationSpace() == ParticleSimulationSpace::local
		? matrix3_t(math::inverse(emitterGlobalTransform))
		: matrix3_t(1.0);

	for(const Plane3dCollisionObject& collider : modifierPlane3dColliders) {
		if(collider.exclusionSet[particleType.id().value()]) {
			continue;
		}

		float_t colliderLife = std::fmod(t - collider.startTime, collider.duration) / collider.duration;
		float_t colliderBounce = collider.bounce.at(colliderLife);
		float_t colliderFriction = collider.friction.at(colliderLife);

		const Curve<float_t>& particlePhysicalSizeCurve = particleType.physicalSize().resultCurve();
		const Curve<float_t>& particleBounceCurve = particleType.bounce().resultCurve();
		const Curve<float_t>& particleFrictionCurve = particleType.friction().resultCurve();

		for(std::uint32_t p = 0; p < particleCount; p++) {
			float3_t particlePosition = particles.globalPosition[p];
			float3_t closestPoint = closestPointOnPlane(particlePosition, collider);
			if(!isPointOnPlaneSegment(closestPoint, collider.vertices)) {
				continue;
			}

			float3_t particleVelocity = simulationSpaceToGlobalTransform * particles.velocity[p];
			float3_t particleForce = simulationSpaceToGlobalTransform * particles.force[p];

			float3_t colliderToParticle = particlePosition - closestPoint;
			float3_t colliderNormal = math::normalize(colliderToParticle);
			float_t distanceToColliderSq = math::lengthSquared(colliderToParticle);
			float_t particleRadius = std::min(particles.size[p].x, std::min(particles.size[p].y, particles.size[p].z)) * 0.5;
			particleRadius = std::max(particleRadius * particlePhysicalSizeCurve.at(particles.life[p]), 0.01);

			if(distanceToColliderSq <= particleRadius * particleRadius) {
				float_t particleForceLength = math::length(particleForce);
				float3_t particleForceDirection = particleForceLength > 0.0
					? particleForce / particleForceLength
					: worldUpVector3;

				float_t distanceToCollider = std::sqrt(distanceToColliderSq);
				float3_t reflectedVelocity = math::reflect(particleVelocity, colliderNormal);

				float3_t slideVector = math::safeNormalize(closestPointOnPlane(closestPoint + particleForceDirection, collider) - closestPoint);
				float_t slideFactor = math::dot(particleForceDirection, slideVector) * particleForceLength * distanceToCollider / particleRadius;

				float_t bounce = colliderBounce * particleBounceCurve.at(particles.life[p]);
				float_t friction = std::min(colliderFriction * particleFrictionCurve.at(particles.life[p]), 1.0);

				particles.velocity[p] = globalToSimulationSpaceTransform * (reflectedVelocity * bounce + slideVector * slideFactor * (1.0 - friction));
				particles.position[p] += globalToSimulationSpaceTransform * (colliderNormal * (particleRadius - distanceToCollider));

				if(collider.killOnContact) {
					particles.life[p] = 1.0;
				}
			}
			else {
				std::optional<float3_t> intersection = rayColliderIntersection(collider,
					particlePosition, particlePosition + particleVelocity * dt + particleForce * dt * dt);

				if(intersection) {
					float3_t reflectedVelocity = math::reflect(particleVelocity, colliderNormal);
					float_t bounce = colliderBounce * particleBounceCurve.at(particles.life[p]);

					particles.velocity[p] = globalToSimulationSpaceTransform * reflectedVelocity * bounce;

					if(collider.killOnContact) {
						particles.life[p] = 1.0;
					}
				}
			}
		}
	}
}
}
