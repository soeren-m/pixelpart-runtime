#pragma once

#include "../common/Types.h"

namespace pixelpart {
struct ParticleWritePtr {
	id_t* id = nullptr;
	id_t* parentId = nullptr;
	float_t* life = nullptr;
	float_t* lifespan = nullptr;
	vec3_t* position = nullptr;
	vec3_t* globalPosition = nullptr;
	vec3_t* velocity = nullptr;
	vec3_t* force = nullptr;
	vec3_t* rotation = nullptr;
	vec3_t* initialRotation = nullptr;
	vec3_t* initialAngularVelocity = nullptr;
	vec3_t* size = nullptr;
	float_t* initialSize = nullptr;
	vec4_t* color = nullptr;
	vec4_t* initialColor = nullptr;
};

struct ParticleReadPtr {
	const id_t* id = nullptr;
	const id_t* parentId = nullptr;
	const float_t* life = nullptr;
	const float_t* lifespan = nullptr;
	const vec3_t* position = nullptr;
	const vec3_t* globalPosition = nullptr;
	const vec3_t* velocity = nullptr;
	const vec3_t* force = nullptr;
	const vec3_t* rotation = nullptr;
	const vec3_t* initialRotation = nullptr;
	const vec3_t* initialAngularVelocity = nullptr;
	const vec3_t* size = nullptr;
	const float_t* initialSize = nullptr;
	const vec4_t* color = nullptr;
	const vec4_t* initialColor = nullptr;
};
}