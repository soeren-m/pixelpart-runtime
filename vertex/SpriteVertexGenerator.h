#pragma once

#include "VertexGenerator.h"
#include "VertexAttribute.h"
#include "VertexWindingOrder.h"
#include "../common/Math.h"
#include <cstddef>
#include <cstdint>

namespace pixelpart {
class SpriteVertexGenerator : public VertexGenerator {
public:
	SpriteVertexGenerator(const Effect& effect, id_t particleEmitterId, id_t particleTypeId,
		const VertexFormat& vertexFormat);

	virtual VertexDataBufferDimensions buildGeometry(
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const RuntimeContext& runtimeContext, const SceneContext& sceneContext) override;

	virtual void generateVertexData(const VertexDataBufferCollection& dataBuffers,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const override;

private:
	void validateVertexFormat() const;

	template <typename T>
	void generateIndex(std::uint8_t* buffer, const VertexAttribute& attribute,
		std::uint32_t particleCount) const {
		std::size_t stride = attribute.byteStride;
		buffer += attribute.byteOffset;

		switch(generatorVertexFormat.windingOrder()) {
			case VertexWindingOrder::cw:
				for(std::uint32_t p = 0; p < particleCount; p++) {
					*reinterpret_cast<T*>(buffer + stride * 0) = static_cast<T>(p * 4 + 0);
					*reinterpret_cast<T*>(buffer + stride * 1) = static_cast<T>(p * 4 + 3);
					*reinterpret_cast<T*>(buffer + stride * 2) = static_cast<T>(p * 4 + 1);
					*reinterpret_cast<T*>(buffer + stride * 3) = static_cast<T>(p * 4 + 1);
					*reinterpret_cast<T*>(buffer + stride * 4) = static_cast<T>(p * 4 + 3);
					*reinterpret_cast<T*>(buffer + stride * 5) = static_cast<T>(p * 4 + 2);
					buffer += stride * 6;
				}
				break;
			default:
				for(std::uint32_t p = 0; p < particleCount; p++) {
					*reinterpret_cast<T*>(buffer + stride * 0) = static_cast<T>(p * 4 + 0);
					*reinterpret_cast<T*>(buffer + stride * 1) = static_cast<T>(p * 4 + 1);
					*reinterpret_cast<T*>(buffer + stride * 2) = static_cast<T>(p * 4 + 3);
					*reinterpret_cast<T*>(buffer + stride * 3) = static_cast<T>(p * 4 + 1);
					*reinterpret_cast<T*>(buffer + stride * 4) = static_cast<T>(p * 4 + 2);
					*reinterpret_cast<T*>(buffer + stride * 5) = static_cast<T>(p * 4 + 3);
					buffer += stride * 6;
				}
				break;
		}
	}

	template <typename T>
	void generateId(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
		std::size_t stride = attribute.byteStride;
		buffer += attribute.byteOffset;

		switch(attribute.dataGenerationMode) {
			case VertexDataGenerationMode::vertex: {
				for(std::uint32_t p = 0; p < particleCount; p++) {
					T particleId = static_cast<T>(particles.id[p]);

					*reinterpret_cast<T*>(buffer + stride * 0) = particleId;
					*reinterpret_cast<T*>(buffer + stride * 1) = particleId;
					*reinterpret_cast<T*>(buffer + stride * 2) = particleId;
					*reinterpret_cast<T*>(buffer + stride * 3) = particleId;
					*reinterpret_cast<T*>(buffer + stride * 4) = particleId;
					*reinterpret_cast<T*>(buffer + stride * 5) = particleId;
					buffer += stride * 6;
				}

				break;
			}

			case VertexDataGenerationMode::element: {
				for(std::uint32_t p = 0; p < particleCount; p++) {
					T particleId = static_cast<T>(particles.id[p]);

					*reinterpret_cast<T*>(buffer + stride * 0) = particleId;
					*reinterpret_cast<T*>(buffer + stride * 1) = particleId;
					*reinterpret_cast<T*>(buffer + stride * 2) = particleId;
					*reinterpret_cast<T*>(buffer + stride * 3) = particleId;
					buffer += stride * 4;
				}

				break;
			}

			case VertexDataGenerationMode::instance: {
				for(std::uint32_t p = 0; p < particleCount; p++) {
					*reinterpret_cast<T*>(buffer) = static_cast<T>(particles.id[p]);
					buffer += stride;
				}

				break;
			}

			default: {
				break;
			}
		}
	}

	void generatePosition2d(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const;
	void generatePosition3d(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const;
	void generateNormal(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const;
	void generateTextureCoord(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateColor(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateLife(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateVelocity2d(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateVelocity3d(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;

	static float3_t rotatePoint2d(const float3_t& point, const float3_t& center, float_t angle);
	static mat3_t rotationMatrix3d(const float3_t& rollYawPitch);
	static mat3_t lookAtMatrix3d(const float3_t& direction);

	const Effect& generatorEffect;
	id_t generatorParticleEmitterId;
	id_t generatorParticleTypeId;

	VertexFormat generatorVertexFormat;

	ParticleCollection generatorSortedParticleCollection;
};
}
