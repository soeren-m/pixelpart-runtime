#pragma once

#include "ParticleSolver.h"
#include "../common/Grid.h"
#include <bitset>
#include <unordered_set>

namespace pixelpart {
class CollisionSolver : public ParticleSolver {
public:
	struct ColliderSegment {
		std::bitset<256> exclusionSet;
		float_t lifetimeStart = 0.0;
		float_t lifetimeDuration = 0.0;
		bool repeat = false;
		bool killOnContact = false;

		Curve<float_t> bounce;
		Curve<float_t> friction;

		ColliderSegment(const Collider& collider);
	};

	struct LineColliderSegment : ColliderSegment {
		vec2_t startPoint = vec2_t(-1.0, 0.0);
		vec2_t endPoint = vec2_t(+1.0, 0.0);

		LineColliderSegment(const Collider& collider, std::size_t segmentIndex);
	};

	struct PlaneColliderSegment : ColliderSegment {
		vec3_t center = vec3_t(0.0, 0.0, 0.0);
		vec3_t normal = vec3_t(0.0, 1.0, 0.0);
		vec3_t rightVector = vec3_t(1.0, 0.0, 0.0);
		vec3_t upVector = vec3_t(0.0, 0.0, 1.0);

		PlaneColliderSegment(const Collider& collider, std::size_t segmentIndex);
	};

	CollisionSolver();

	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleWritePtr particles, uint32_t numParticles, float_t t, float_t dt) const override;

	virtual void refresh(const Effect& effect) override;

	void setGridCellCountFactor(uint32_t factor);
	void setGridPadding(float_t padding);

private:
	struct Intersection {
		bool hit = false;
		vec3_t point = vec3_t(0.0);

		Intersection();
		Intersection(const vec3_t& p);
	};

	static bool isPointOnLineSegment(const vec2_t& p, const vec2_t& l1, const vec2_t& l2);
	static bool isPointOnLineSegment(const vec3_t& p, const vec3_t& l1, const vec3_t& l2);
	static bool isPointOnCollider(const vec3_t& p, const PlaneColliderSegment& collider);
	static vec2_t calculateClosestPointOnLine(const vec2_t& p, const LineColliderSegment& collider);
	static vec3_t calculateClosestPointOnPlane(const vec3_t& p, const PlaneColliderSegment& collider);
	static Intersection calculateRayColliderIntersection(const LineColliderSegment& collider, const vec2_t& rayOrigin, const vec2_t& rayEnd);
	static Intersection calculateRayColliderIntersection(const PlaneColliderSegment& collider, const vec3_t& rayOrigin, const vec3_t& rayEnd);

	void solve(const ParticleType& particleType, ParticleWritePtr particles, uint32_t p, float_t t, float_t dt, const LineColliderSegment& collider) const;
	void solve(const ParticleType& particleType, ParticleWritePtr particles, uint32_t p, float_t t, float_t dt, const PlaneColliderSegment& collider) const;

	GridIndex<int32_t> toGridIndex(const vec3_t& position) const;

	void findPotentialColliders(std::unordered_set<uint32_t>& potentialColliders, int32_t cx, int32_t cy, const vec2_t& size) const;

	std::vector<LineColliderSegment> lineColliders;
	std::vector<PlaneColliderSegment> planeColliders;

	Grid<std::vector<uint32_t>> grid;
	vec2_t gridBottom = vec2_t(0.0);
	vec2_t gridTop = vec2_t(0.0);
	vec2_t gridCellDimension = vec2_t(1.0);
	uint32_t gridCellCountFactor = 3u;
	float_t gridPadding = 0.25;
};
}