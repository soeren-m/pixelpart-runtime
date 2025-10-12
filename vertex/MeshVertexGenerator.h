#pragma once

#include "VertexGenerator.h"
#include "VertexAttribute.h"
#include "../common/Math.h"
#include "../effect/AlignmentMode.h"
#include <cstddef>
#include <cstdint>

namespace pixelpart {
class MeshVertexGenerator : public VertexGenerator {
public:
	MeshVertexGenerator(const Effect& effect, id_t particleEmitterId, id_t particleTypeId,
		const VertexFormat& vertexFormat);

	virtual VertexDataBufferDimensions buildGeometry(
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) override;

	virtual void generateVertexData(const VertexDataBufferCollection& dataBuffers,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const override;

private:
	void validateVertexFormat() const;

	template <typename T>
	void generateId(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
		std::size_t stride = attribute.byteStride;
		buffer += attribute.byteOffset;

		switch(attribute.dataGenerationMode) {
			case VertexDataGenerationMode::instance:
				for(std::uint32_t p = 0; p < particleCount; p++) {
					*reinterpret_cast<T*>(buffer) = static_cast<T>(particles.id[p]);
					buffer += stride;
				}
				break;

			default:
				break;
		}
	}

	void generatePosition2d(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generatePosition3d(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateColor(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateLife(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateId(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateVelocity2d(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateVelocity3d(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateMatrix4x4ColumnMajor(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const;
	void generateMatrix4x3ColumnMajor(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const;
	void generateMatrix4x4RowMajor(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const;
	void generateMatrix4x3RowMajor(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const;

	static mat4_t generateTransformationMatrix(AlignmentMode alignmentMode, const float3_t& pivot,
		const float3_t& emitterPosition, const mat3_t& emitterRotation,
		ParticleCollection::ReadPtr particles, std::uint32_t particleIndex);

	static mat3_t rotationMatrix3d(const float3_t& rollYawPitch);
	static mat3_t lookAtMatrix3d(const float3_t& direction);

	const Effect& generatorEffect;
	id_t generatorParticleEmitterId;
	id_t generatorParticleTypeId;

	VertexFormat generatorVertexFormat;

	ParticleCollection generatorSortedParticleCollection;
};
}
