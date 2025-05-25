#pragma once

#include "ParticleModifier.h"
#include "LineQueryGrid.h"
#include "../effect/LineCollider.h"
#include "../effect/PlaneCollider.h"
#include "../common/Math.h"
#include "../common/Curve.h"
#include "../common/Transform.h"
#include <bitset>
#include <array>
#include <vector>

namespace pixelpart {
class CollisionModifier : public ParticleModifier {
public:
	CollisionModifier() = default;

	virtual void run(const Effect* effect, RuntimeContext runtimeContext, ParticleRuntimeId runtimeId,
		ParticleCollection::WritePtr particles, std::uint32_t particleCount) const override;

	virtual void prepare(const Effect& effect, const RuntimeContext& runtimeContext) override;

private:
	struct ColliderObject {
		ColliderObject(const Collider& collider);

		std::bitset<256> exclusionSet;
		float_t startTime = 0.0;
		float_t duration = 0.0;
		bool repeat = false;
		bool killOnContact = false;

		Curve<float_t> bounce;
		Curve<float_t> friction;
	};

	struct Line2dColliderObject : ColliderObject {
		Line2dColliderObject(const LineCollider& collider, const Transform& transform, std::size_t segmentIndex);
		Line2dColliderObject(const PlaneCollider& collider, const Transform& transform);

		float2_t start = float2_t(-1.0, 0.0);
		float2_t end = float2_t(+1.0, 0.0);
	};

	struct Plane3dColliderObject : ColliderObject {
		Plane3dColliderObject(const PlaneCollider& collider, const Transform& transform);

		float3_t center = float3_t(0.0, 0.0, 0.0);
		float3_t normal = float3_t(0.0, 1.0, 0.0);
		std::array<float3_t, 4> vertices;
	};

	struct Intersection {
		Intersection();
		Intersection(const float3_t& p);

		bool hit = false;
		float3_t point = float3_t(0.0);
	};

	static bool isPointOnLineSegment(const float2_t& p, const float2_t& l1, const float2_t& l2);
	static bool isPointOnLineSegment(const float3_t& p, const float3_t& l1, const float3_t& l2);
	static bool isPointOnCollider(const float3_t& p, const Plane3dColliderObject& collider);
	static float2_t calculateClosestPointOnLine(const float2_t& p, const Line2dColliderObject& collider);
	static float3_t calculateClosestPointOnPlane(const float3_t& p, const Plane3dColliderObject& collider);
	static Intersection calculateRayColliderIntersection(const Line2dColliderObject& collider, const float2_t& rayOrigin, const float2_t& rayEnd);
	static Intersection calculateRayColliderIntersection(const Plane3dColliderObject& collider, const float3_t& rayOrigin, const float3_t& rayEnd);

	void collide(const ParticleType& particleType, ParticleCollection::WritePtr particles, std::uint32_t p, float_t t, float_t dt, const Line2dColliderObject& collider) const;
	void collide(const ParticleType& particleType, ParticleCollection::WritePtr particles, std::uint32_t p, float_t t, float_t dt, const Plane3dColliderObject& collider) const;

	std::vector<Line2dColliderObject> line2dColliders;
	std::vector<Plane3dColliderObject> plane3dColliders;

	LineQueryGrid line2dColliderGrid;
};
}