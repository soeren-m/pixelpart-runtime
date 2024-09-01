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

		vec2_t startPoint = vec2_t(-1.0, 0.0);
		vec2_t endPoint = vec2_t(+1.0, 0.0);
	};

	struct PlaneColliderSegment : ColliderSegment {
		PlaneColliderSegment(const Collider& collider, std::size_t segmentIndex);

		vec3_t center = vec3_t(0.0, 0.0, 0.0);
		vec3_t normal = vec3_t(0.0, 1.0, 0.0);
		vec3_t rightVector = vec3_t(1.0, 0.0, 0.0);
		vec3_t upVector = vec3_t(0.0, 0.0, 1.0);
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
		Intersection(const vec3_t& p);

		bool hit = false;
		vec3_t point = vec3_t(0.0);
	};

	static bool isPointOnLineSegment(const vec2_t& p, const vec2_t& l1, const vec2_t& l2);
	static bool isPointOnLineSegment(const vec3_t& p, const vec3_t& l1, const vec3_t& l2);
	static bool isPointOnCollider(const vec3_t& p, const PlaneColliderSegment& collider);
	static vec2_t calculateClosestPointOnLine(const vec2_t& p, const LineColliderSegment& collider);
	static vec3_t calculateClosestPointOnPlane(const vec3_t& p, const PlaneColliderSegment& collider);
	static Intersection calculateRayColliderIntersection(const LineColliderSegment& collider, const vec2_t& rayOrigin, const vec2_t& rayEnd);
	static Intersection calculateRayColliderIntersection(const PlaneColliderSegment& collider, const vec3_t& rayOrigin, const vec3_t& rayEnd);

	void solve(const ParticleType& particleType, ParticleCollection::WritePtr particles, uint32_t p, float_t t, float_t dt, const LineColliderSegment& collider) const;
	void solve(const ParticleType& particleType, ParticleCollection::WritePtr particles, uint32_t p, float_t t, float_t dt, const PlaneColliderSegment& collider) const;

	GridIndex<int32_t> toGridIndex(const vec3_t& position) const;

	void findPotentialColliders(std::unordered_set<uint32_t>& potentialColliders, int32_t cx, int32_t cy, const vec2_t& size) const;

	std::vector<LineColliderSegment> lineColliders;
	std::vector<PlaneColliderSegment> planeColliders;

	Grid<std::vector<uint32_t>> solverGrid;
	vec2_t solverGridBottom = vec2_t(0.0);
	vec2_t solverGridTop = vec2_t(0.0);
	vec2_t solverGridCellDimension = vec2_t(1.0);
	uint32_t solverGridCellCountFactor = 3u;
	float_t solverGridPadding = 0.25;
};
}