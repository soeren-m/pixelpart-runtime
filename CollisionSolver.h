#pragma once

#include "Collider.h"
#include "Grid.h"
#include "ParticleData.h"
#include "ParticleEmitter.h"
#include <unordered_set>

namespace pixelpart {
class CollisionSolver {
public:
	struct LineCollider {
		std::bitset<256> emitterMask;
		floatd lifetimeStart;
		floatd lifetimeDuration;
		bool repeat;
		vec2d p1;
		vec2d p2;
		Curve<floatd> bounce;
		Curve<floatd> friction;
	};

	static bool onSegment(const vec2d& l1, const vec2d& l2, const vec2d& p);
	static bool closestPointOnSegment(const vec2d& l1, const vec2d& l2, const vec2d& p, vec2d& result);
	static bool intersectionSegmentSegment(const vec2d& p1, const vec2d& p2, const vec2d& q1, const vec2d& q2, vec2d& result);

	CollisionSolver();

	void solve(const ParticleEmitter& emitter, ParticleDataPointer& particles, uint32_t p, floatd particleBounce, floatd particleFriction, floatd t, floatd dt) const;
	void solve(ParticleDataPointer& particles, uint32_t p, floatd particleBounce, floatd particleFriction, floatd t, floatd dt, const LineCollider& collider) const;

	void update(const std::vector<Collider>& allColliders);

	void setGridCellCountFactor(uint32_t factor);
	void setGridPadding(floatd padding);

private:
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