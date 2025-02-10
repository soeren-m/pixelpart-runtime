#pragma once

#include "../common/Types.h"
#include "../common/Math.h"
#include <vector>

namespace pixelpart {
class ParticleCollection {
public:
	struct WritePtr {
		uint32_t* id = nullptr;
		uint32_t* parentId = nullptr;
		float_t* life = nullptr;
		float_t* lifespan = nullptr;
		float3_t* position = nullptr;
		float3_t* globalPosition = nullptr;
		float3_t* velocity = nullptr;
		float3_t* force = nullptr;
		float3_t* rotation = nullptr;
		float3_t* initialRotation = nullptr;
		float3_t* initialAngularVelocity = nullptr;
		float3_t* size = nullptr;
		float_t* initialSize = nullptr;
		float4_t* color = nullptr;
		float4_t* initialColor = nullptr;
	};

	struct ReadPtr {
		const uint32_t* id = nullptr;
		const uint32_t* parentId = nullptr;
		const float_t* life = nullptr;
		const float_t* lifespan = nullptr;
		const float3_t* position = nullptr;
		const float3_t* globalPosition = nullptr;
		const float3_t* velocity = nullptr;
		const float3_t* force = nullptr;
		const float3_t* rotation = nullptr;
		const float3_t* initialRotation = nullptr;
		const float3_t* initialAngularVelocity = nullptr;
		const float3_t* size = nullptr;
		const float_t* initialSize = nullptr;
		const float4_t* color = nullptr;
		const float4_t* initialColor = nullptr;
	};

	ParticleCollection(uint32_t capacity = 1024u);

	uint32_t add(uint32_t number);
	void remove(uint32_t index);
	void removeDead();
	void clear();

	WritePtr writePtr(uint32_t index = 0u);
	ReadPtr readPtr(uint32_t index = 0u) const;

	uint32_t count() const;
	uint32_t capacity() const;

private:
	uint32_t particleCount = 0u;
	uint32_t particleCapacity = 1024u;

	std::vector<uint32_t> id;
	std::vector<uint32_t> parentId;
	std::vector<float_t> life;
	std::vector<float_t> lifespan;
	std::vector<float3_t> position;
	std::vector<float3_t> globalPosition;
	std::vector<float3_t> velocity;
	std::vector<float3_t> force;
	std::vector<float3_t> rotation;
	std::vector<float3_t> initialRotation;
	std::vector<float3_t> initialAngularVelocity;
	std::vector<float3_t> size;
	std::vector<float_t> initialSize;
	std::vector<float4_t> color;
	std::vector<float4_t> initialColor;
};
}