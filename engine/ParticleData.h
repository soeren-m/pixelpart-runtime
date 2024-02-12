#pragma once

#include "../common/Types.h"
#include <vector>

namespace pixelpart {
struct ParticleData {
	std::vector<id_t> id;
	std::vector<id_t> parentId;
	std::vector<float_t> life;
	std::vector<float_t> lifespan;
	std::vector<vec3_t> position;
	std::vector<vec3_t> globalPosition;
	std::vector<vec3_t> velocity;
	std::vector<vec3_t> force;
	std::vector<vec3_t> rotation;
	std::vector<vec3_t> initialRotation;
	std::vector<vec3_t> initialAngularVelocity;
	std::vector<vec3_t> size;
	std::vector<float_t> initialSize;
	std::vector<vec4_t> color;
	std::vector<vec4_t> initialColor;

	void resize(uint32_t n);
	void swap(uint32_t i, uint32_t j);
};

struct ParticleDataPointer {
	id_t* id;
	id_t* parentId;
	float_t* life;
	float_t* lifespan;
	vec3_t* position;
	vec3_t* globalPosition;
	vec3_t* velocity;
	vec3_t* force;
	vec3_t* rotation;
	vec3_t* initialRotation;
	vec3_t* initialAngularVelocity;
	vec3_t* size;
	float_t* initialSize;
	vec4_t* color;
	vec4_t* initialColor;
};
}