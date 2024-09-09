#pragma once

#include "../common/Types.h"
#include "../common/Math.h"
#include <vector>

namespace pixelpart {
class ParticleCollection {
public:
	struct WritePtr {
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

	struct ReadPtr {
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

	//TODO ParticleCollection() = default;
	ParticleCollection(uint32_t capacity);

	uint32_t add(uint32_t number);
	void remove(uint32_t index);
	void removeAll();

	WritePtr writePtr(uint32_t index = 0u);
	ReadPtr readPtr(uint32_t index = 0u) const;

	uint32_t capacity() const;
	uint32_t count() const;

private:
	uint32_t particleCapacity = 0u;
	uint32_t particleCount = 0u;

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