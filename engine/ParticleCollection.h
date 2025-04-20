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

		WritePtr() = default;
		WritePtr(
			uint32_t* idPtr,
			uint32_t* parentIdPtr,
			float_t* lifePtr,
			float_t* lifespanPtr,
			float3_t* positionPtr,
			float3_t* globalPositionPtr,
			float3_t* velocityPtr,
			float3_t* forcePtr,
			float3_t* rotationPtr,
			float3_t* initialRotationPtr,
			float3_t* initialAngularVelocityPtr,
			float3_t* sizePtr,
			float_t* initialSizePtr,
			float4_t* colorPtr,
			float4_t* initialColorPtr);
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

		ReadPtr() = default;
		ReadPtr(
			const uint32_t* idPtr,
			const uint32_t* parentIdPtr,
			const float_t* lifePtr,
			const float_t* lifespanPtr,
			const float3_t* positionPtr,
			const float3_t* globalPositionPtr,
			const float3_t* velocityPtr,
			const float3_t* forcePtr,
			const float3_t* rotationPtr,
			const float3_t* initialRotationPtr,
			const float3_t* initialAngularVelocityPtr,
			const float3_t* sizePtr,
			const float_t* initialSizePtr,
			const float4_t* colorPtr,
			const float4_t* initialColorPtr);
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