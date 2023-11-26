#pragma once

#include "../effect/Effect.h"
#include "../common/Grid.h"
#include "ParticleData.h"
#include <bitset>
#include <unordered_set>

namespace pixelpart {
class CollisionSolver {
public:
	struct ColliderSegment {
		std::bitset<256> exclusionSet;
		floatd lifetimeStart = 0.0;
		floatd lifetimeDuration = 0.0;
		bool repeat = false;
		bool killOnContact = false;

		Curve<floatd> bounce;
		Curve<floatd> friction;

		ColliderSegment(const Collider& collider);
	};

	struct LineColliderSegment : ColliderSegment {
		vec2d startPoint = vec2d(-1.0, 0.0);
		vec2d endPoint = vec2d(+1.0, 0.0);

		LineColliderSegment(const Collider& collider, std::size_t segmentIndex);
	};

	struct PlaneColliderSegment : ColliderSegment {
		vec3d center = vec3d(0.0, 0.0, 0.0);
		vec3d normal = vec3d(0.0, 1.0, 0.0);
		vec3d rightVector = vec3d(1.0, 0.0, 0.0);
		vec3d upVector = vec3d(0.0, 0.0, 1.0);

		PlaneColliderSegment(const Collider& collider, std::size_t segmentIndex);
	};

	CollisionSolver();

	void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, floatd t, floatd dt) const;

	void update(const Effect* effect);

	void setGridCellCountFactor(uint32_t factor);
	void setGridPadding(floatd padding);

private:
	struct Intersection {
		bool hit = false;
		vec3d point = vec3d(0.0);

		Intersection();
		Intersection(const vec3d& p);
	};

	static bool isPointOnLineSegment(const vec2d& p, const vec2d& l1, const vec2d& l2);
	static bool isPointOnLineSegment(const vec3d& p, const vec3d& l1, const vec3d& l2);
	static bool isPointOnCollider(const vec3d& p, const PlaneColliderSegment& collider);
	static vec2d calculateClosestPointOnLine(const vec2d& p, const LineColliderSegment& collider);
	static vec3d calculateClosestPointOnPlane(const vec3d& p, const PlaneColliderSegment& collider);
	static Intersection calculateRayColliderIntersection(const LineColliderSegment& collider, const vec2d& rayOrigin, const vec2d& rayEnd);
	static Intersection calculateRayColliderIntersection(const PlaneColliderSegment& collider, const vec3d& rayOrigin, const vec3d& rayEnd);

	void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t p, floatd t, floatd dt, const LineColliderSegment& collider) const;
	void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t p, floatd t, floatd dt, const PlaneColliderSegment& collider) const;

	GridIndex<int32_t> toGridIndex(const vec3d& position) const;

	void findPotentialColliders(std::unordered_set<uint32_t>& potentialColliders, int32_t cx, int32_t cy, const vec2d& size) const;

	std::vector<LineColliderSegment> lineColliders;
	std::vector<PlaneColliderSegment> planeColliders;

	Grid<std::vector<uint32_t>> grid;
	vec2d gridBottom = vec2d(0.0);
	vec2d gridTop = vec2d(0.0);
	vec2d gridCellDimension = vec2d(1.0);
	uint32_t gridCellCountFactor = 3;
	floatd gridPadding = 0.25;
};
}