#pragma once

#include "VertexGenerator.h"
#include "VertexAttribute.h"
#include "../effect/AlignmentMode.h"
#include "../platform/ThreadPool.h"
#include "../types/Types.h"
#include <cstddef>
#include <cstdint>
#include <memory>

namespace pixelpart {
class MeshVertexGenerator : public VertexGenerator {
public:
	MeshVertexGenerator(const Effect& effect, id_t particleEmitterId, id_t particleTypeId,
		const VertexFormat& vertexFormat,
		std::shared_ptr<ThreadPool> threadPool);

	virtual VertexDataBufferDimensions buildGeometry(
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) override;

	virtual void generateVertexData(const VertexDataBufferCollection& dataBuffers,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const override;

private:
	void validateVertexFormat() const;

	template <typename T>
	void generateId(unsigned char* buffer, const VertexAttribute& attribute,
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

	void generatePosition2d(unsigned char* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generatePosition3d(unsigned char* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateColorFloat(unsigned char* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateColorByte(unsigned char* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateLife(unsigned char* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateId(unsigned char* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateVelocity2d(unsigned char* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateVelocity3d(unsigned char* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateMatrix4x4ColumnMajor(unsigned char* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const;
	void generateMatrix4x3ColumnMajor(unsigned char* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const;
	void generateMatrix4x4RowMajor(unsigned char* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const;
	void generateMatrix4x3RowMajor(unsigned char* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const;

	static matrix4_t particleTransformationMatrix(AlignmentMode alignmentMode, const float3_t& pivot,
		const matrix3_t& simulationSpaceToGlobalMatrix,
		const float3_t& emitterPosition, const matrix3_t& emitterRotation,
		ParticleCollection::ReadPtr particles, std::uint32_t particleIndex);

	static matrix3_t rotationMatrix3d(const float3_t& rollYawPitch);

	const Effect& generatorEffect;
	id_t generatorParticleEmitterId;
	id_t generatorParticleTypeId;

	VertexFormat generatorVertexFormat;

	std::shared_ptr<ThreadPool> generatorThreadPool;

	ParticleCollection generatorSortedParticleCollection;
};
}
