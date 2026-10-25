#pragma once

#include "ParticleModifier.h"
#include "LineQueryGrid.h"
#include "../effect/LineCollider.h"
#include "../effect/PlaneCollider.h"
#include "../effect/ParticleEmitter.h"
#include "../effect/ParticleType.h"
#include "../effect/Curve.h"
#include "../effect/Transform.h"
#include "../types/Types.h"
#include <bitset>
#include <optional>
#include <array>
#include <vector>

namespace pixelpart {
class CollisionModifier : public ParticleModifier {
public:
	CollisionModifier() = default;

	virtual void apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
		const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const override;

	virtual void reset(const Effect* effect, EffectRuntimeContext runtimeContext) override;

private:
	struct CollisionObject {
		std::bitset<256> exclusionSet;
		float_t startTime = 0.0;
		float_t duration = 0.0;
		bool repeat = false;
		bool killOnContact = false;

		Curve<float_t> bounce;
		Curve<float_t> friction;

		CollisionObject(const Collider& collider);
	};

	struct Line2dCollisionObject : CollisionObject {
		float2_t start = float2_t(-1.0, 0.0);
		float2_t end = float2_t(+1.0, 0.0);

		Line2dCollisionObject(const LineCollider& collider, const Transform& transform, std::size_t segmentIndex);
		Line2dCollisionObject(const PlaneCollider& collider, const Transform& transform);
	};

	struct Plane3dCollisionObject : CollisionObject {
		float3_t center = float3_t(0.0, 0.0, 0.0);
		float3_t normal = float3_t(0.0, 1.0, 0.0);
		std::array<float3_t, 4> vertices;

		Plane3dCollisionObject(const PlaneCollider& collider, const Transform& transform);
	};

	static bool isPointOnLineSegment(const float2_t& p, const float2_t& lineStart, const float2_t& lineEnd);
	static bool isPointOnLineSegment(const float3_t& p, const float3_t& lineStart, const float3_t& lineEnd);
	static bool isPointOnPlaneSegment(const float3_t& p, const std::array<float3_t, 4>& planeVertices);
	static float2_t closestPointOnLine(const float2_t& p, const Line2dCollisionObject& collider);
	static float3_t closestPointOnPlane(const float3_t& p, const Plane3dCollisionObject& collider);
	static std::optional<float3_t> rayColliderIntersection(const Line2dCollisionObject& collider, const float2_t& rayOrigin, const float2_t& rayEnd);
	static std::optional<float3_t> rayColliderIntersection(const Plane3dCollisionObject& collider, const float3_t& rayOrigin, const float3_t& rayEnd);

	void calculateCollisions2d(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
		const Effect* effect, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		const EffectRuntimeContext& runtimeContext) const;
	void calculateCollisions3d(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
		const Effect* effect, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		const EffectRuntimeContext& runtimeContext) const;

	std::vector<Line2dCollisionObject> modifierLine2dColliders;
	std::vector<Plane3dCollisionObject> modifierPlane3dColliders;

	LineQueryGrid modifierLine2dColliderGrid;
};
}
