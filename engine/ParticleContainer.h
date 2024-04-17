#pragma once

#include "ParticleData.h"
#include <vector>

namespace pixelpart {
class ParticleContainer {
public:
	ParticleContainer() = default;
	ParticleContainer(uint32_t capacity);

	uint32_t spawn(uint32_t count);

	void kill(uint32_t index);
	void killAll();

	ParticleWritePtr getParticleWritePtr(uint32_t start = 0u);
	ParticleReadPtr getParticleReadPtr(uint32_t start = 0u) const;
	uint32_t getParticleCapacity() const;
	uint32_t getNumParticles() const;

private:
	void swap(std::size_t i, std::size_t j);

	uint32_t particleCapacity = 0u;
	uint32_t numParticles = 0u;

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
};
}