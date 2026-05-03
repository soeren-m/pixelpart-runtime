#pragma once

#include "../common/Types.h"
#include <vector>

namespace pixelpart {
class ParticleCollection {
public:
	struct WritePtr {
		std::uint32_t* id = nullptr;
		std::uint32_t* parentId = nullptr;
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
			std::uint32_t* idPtr,
			std::uint32_t* parentIdPtr,
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
		const std::uint32_t* id = nullptr;
		const std::uint32_t* parentId = nullptr;
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
			const std::uint32_t* idPtr,
			const std::uint32_t* parentIdPtr,
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

	ParticleCollection(std::uint32_t maxCount = 0);

	std::uint32_t add(std::uint32_t number);
	void remove(std::uint32_t index);
	void removeDead();
	void clear();

	WritePtr writePtr(std::uint32_t index = 0);
	ReadPtr readPtr(std::uint32_t index = 0) const;

	std::uint32_t count() const;
	std::uint32_t capacity() const;

private:
	void resizeBuffers(std::size_t size);

	std::uint32_t particleCount = 0;
	std::uint32_t particleMaxCount = 0;

	std::vector<std::uint32_t> particleId;
	std::vector<std::uint32_t> particleParentId;
	std::vector<float_t> particleLife;
	std::vector<float_t> particleLifespan;
	std::vector<float3_t> particlePosition;
	std::vector<float3_t> particleGlobalPosition;
	std::vector<float3_t> particleVelocity;
	std::vector<float3_t> particleForce;
	std::vector<float3_t> particleRotation;
	std::vector<float3_t> particleInitialRotation;
	std::vector<float3_t> particleInitialAngularVelocity;
	std::vector<float3_t> particleSize;
	std::vector<float_t> particleInitialSize;
	std::vector<float4_t> particleColor;
	std::vector<float4_t> particleInitialColor;
};
}
