#pragma once

#include "../common/Types.h"

namespace pixelpart {
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