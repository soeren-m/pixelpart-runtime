#pragma once

#include "Types.h"
#include <vector>

namespace pixelpart {
struct ParticleData {
	std::vector<uint32_t> spawnId;
	std::vector<uint32_t> parentId;
	std::vector<floatd> life;
	std::vector<floatd> lifespan;
	std::vector<vec2d> position;
	std::vector<vec2d> globalPosition;
	std::vector<vec2d> velocity;
	std::vector<vec2d> force;
	std::vector<floatd> rotation;
	std::vector<floatd> initialRotation;
	std::vector<floatd> initialAngularVelocity;
	std::vector<vec2d> size;
	std::vector<floatd> initialSize;
	std::vector<vec4d> color;
	std::vector<vec4d> initialColor;
};

struct ParticleDataPointer {
	uint32_t* spawnId;
	uint32_t* parentId;
	floatd* life;
	floatd* lifespan;
	vec2d* position;
	vec2d* globalPosition;
	vec2d* velocity;
	vec2d* force;
	floatd* rotation;
	floatd* initialRotation;
	floatd* initialAngularVelocity;
	vec2d* size;
	floatd* initialSize;
	vec4d* color;
	vec4d* initialColor;
};
}