#pragma once

#include "ParticleSolver.h"
#include "../common/Math.h"
#include "../common/Curve.h"
#include "../common/Grid.h"
#include "../effect/Collider.h"
#include <bitset>
#include <vector>
#include <unordered_set>

namespace pixelpart {
class CollisionSolver : public ParticleSolver {
public:
	struct ColliderSegment {
		ColliderSegment(const Collider& collider);

		std::bitset<256> exclusionSet;
		float_t start = 0.0;
		float_t duration = 0.0;
		bool repeat = false;
		bool killOnContact = false;

		Curve<float_t> bounce;
		Curve<float_t> friction;
	};

	struct LineColliderSegment : ColliderSegment {
		LineColliderSegment(const Collider& collider, std::size_t segmentIndex);

		float2_t startPoint = float2_t(-1.0, 0.0);
		float2_t endPoint = float2_t(+1.0, 0.0);
	};

	struct PlaneColliderSegment : ColliderSegment {
		PlaneColliderSegment(const Collider& collider, std::size_t segmentIndex);

		float3_t center = float3_t(0.0, 0.0, 0.0);
		float3_t normal = float3_t(0.0, 1.0, 0.0);
		float3_t rightVector = float3_t(1.0, 0.0, 0.0);
		float3_t upVector = float3_t(0.0, 0.0, 1.0);
	};

	CollisionSolver();

	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const override;

	virtual void prepare(const Effect& effect) override;

	void gridCellCountFactor(uint32_t factor);
	void gridPadding(float_t padding);

private:
	struct Intersection {
		Intersection();
		Intersection(const float3_t& p);

		bool hit = false;
		float3_t point = float3_t(0.0);
	};

	static bool isPointOnLineSegment(const float2_t& p, const float2_t& l1, const float2_t& l2);
	static bool isPointOnLineSegment(const float3_t& p, const float3_t& l1, const float3_t& l2);
	static bool isPointOnCollider(const float3_t& p, const PlaneColliderSegment& collider);
	static float2_t calculateClosestPointOnLine(const float2_t& p, const LineColliderSegment& collider);
	static float3_t calculateClosestPointOnPlane(const float3_t& p, const PlaneColliderSegment& collider);
	static Intersection calculateRayColliderIntersection(const LineColliderSegment& collider, const float2_t& rayOrigin, const float2_t& rayEnd);
	static Intersection calculateRayColliderIntersection(const PlaneColliderSegment& collider, const float3_t& rayOrigin, const float3_t& rayEnd);

	void solve(const ParticleType& particleType, ParticleCollection::WritePtr particles, uint32_t p, float_t t, float_t dt, const LineColliderSegment& collider) const;
	void solve(const ParticleType& particleType, ParticleCollection::WritePtr particles, uint32_t p, float_t t, float_t dt, const PlaneColliderSegment& collider) const;

	GridIndex<int32_t> toGridIndex(const float3_t& position) const;

	void findPotentialColliders(std::unordered_set<uint32_t>& potentialColliders, int32_t cx, int32_t cy, const float2_t& size) const;

	std::vector<LineColliderSegment> lineColliders;
	std::vector<PlaneColliderSegment> planeColliders;

	Grid<std::vector<uint32_t>> solverGrid;
	float2_t solverGridBottom = float2_t(0.0);
	float2_t solverGridTop = float2_t(0.0);
	float2_t solverGridCellDimension = float2_t(1.0);
	uint32_t solverGridCellCountFactor = 3u;
	float_t solverGridPadding = 0.25;
};
}