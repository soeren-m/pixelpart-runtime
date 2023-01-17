#pragma once

#include "Types.h"
#include <vector>

namespace pixelpart {
struct ParticleData {
	std::vector<uint32_t> id;
	std::vector<uint32_t> parentId;
	std::vector<floatd> life;
	std::vector<floatd> lifespan;
	std::vector<vec3d> position;
	std::vector<vec3d> globalPosition;
	std::vector<vec3d> velocity;
	std::vector<vec3d> force;
	std::vector<vec3d> rotation;
	std::vector<vec3d> initialRotation;
	std::vector<vec3d> initialAngularVelocity;
	std::vector<vec3d> size;
	std::vector<floatd> initialSize;
	std::vector<vec4d> color;
	std::vector<vec4d> initialColor;

	void resize(uint32_t n);
	void swap(uint32_t i, uint32_t j);
};

struct ParticleDataPointer {
	uint32_t* id;
	uint32_t* parentId;
	floatd* life;
	floatd* lifespan;
	vec3d* position;
	vec3d* globalPosition;
	vec3d* velocity;
	vec3d* force;
	vec3d* rotation;
	vec3d* initialRotation;
	vec3d* initialAngularVelocity;
	vec3d* size;
	floatd* initialSize;
	vec4d* color;
	vec4d* initialColor;
};
}