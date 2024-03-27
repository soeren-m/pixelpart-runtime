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

	void resize(std::size_t n);
	void swap(std::size_t i, std::size_t j);
};
}