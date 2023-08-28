#pragma once

#include "../effect/Effect.h"
#include "../common/Grid.h"
#include "ParticleData.h"
#include <bitset>
#include <unordered_set>

namespace pixelpart {
class CollisionSolver {
public:
	struct LineCollider {
		std::bitset<256> exclusionSet;
		floatd lifetimeStart;
		floatd lifetimeDuration;
		bool repeat;
		vec2d p1;
		vec2d p2;
		Curve<floatd> bounce;
		Curve<floatd> friction;
	};

	static bool isOnSegment(const vec2d& l1, const vec2d& l2, const vec2d& p);
	static bool getClosestPointOnSegment(const vec2d& l1, const vec2d& l2, const vec2d& p, vec2d& result);
	static bool getIntersectionSegmentSegment(const vec2d& p1, const vec2d& p2, const vec2d& q1, const vec2d& q2, vec2d& result);

	CollisionSolver();

	void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, floatd t, floatd dt) const;
	void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t p, floatd t, floatd dt, const LineCollider& collider) const;

	void update(const Effect* effect);

	void setGridCellCountFactor(uint32_t factor);
	void setGridPadding(floatd padding);

private:
	GridIndex<int32_t> toGridIndex(const vec3d& position) const;

	void findPotentialColliders(std::unordered_set<uint32_t>& potentialColliders, int32_t cx, int32_t cy, const vec2d& size) const;

	std::vector<LineCollider> colliders;
	Grid<std::vector<uint32_t>> grid;
	vec2d gridBottom = vec2d(0.0);
	vec2d gridTop = vec2d(0.0);
	vec2d gridCellDimension = vec2d(1.0);

	uint32_t gridCellCountFactor = 3;
	floatd gridPadding = 0.25;
};
}