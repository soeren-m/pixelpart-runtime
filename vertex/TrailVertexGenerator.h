#pragma once

#include "VertexGenerator.h"
#include "VertexAttribute.h"
#include "VertexWindingOrder.h"
#include "../common/Math.h"
#include "../common/Curve.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <unordered_map>

namespace pixelpart {
class TrailVertexGenerator : public VertexGenerator {
public:
	TrailVertexGenerator(const Effect& effect, id_t particleEmitterId, id_t particleTypeId,
		const VertexFormat& vertexFormat);

	virtual VertexDataBufferDimensions buildGeometry(
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) override;

	virtual void generateVertexData(const VertexDataBufferCollection& dataBuffers,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const override;

private:
	struct Trail {
		std::uint32_t particleCount = 0;
		float_t length = 0.0;

		std::vector<float3_t> position;
		std::vector<float3_t> size;
		std::vector<float4_t> color;
		std::vector<float3_t> velocity;
		std::vector<float_t> life;
		std::vector<float3_t> direction;
		std::vector<float3_t> directionToEdge;
		std::vector<float_t> index;
	};

	void validateVertexFormat() const;

	template <typename T>
	void generateIndex(std::uint8_t* buffer, const VertexAttribute& attribute) const {
		std::size_t stride = attribute.byteStride;
		buffer += attribute.byteOffset;

		std::uint32_t startIndex = 0;

		for(const auto& trailEntry : generatorTrails) {
			const Trail& trail = trailEntry.second;
			if(trail.position.size() < 2) {
				continue;
			}

			std::uint32_t segmentCount = static_cast<std::uint32_t>(trail.position.size() - 1);

			switch(generatorVertexFormat.windingOrder()) {
				case VertexWindingOrder::cw:
					for(std::uint32_t p = 0; p < segmentCount; p++) {
						*reinterpret_cast<T*>(buffer + stride * 0) = static_cast<T>(startIndex + p * 5 + 0);
						*reinterpret_cast<T*>(buffer + stride * 1) = static_cast<T>(startIndex + p * 5 + 4);
						*reinterpret_cast<T*>(buffer + stride * 2) = static_cast<T>(startIndex + p * 5 + 1);
						*reinterpret_cast<T*>(buffer + stride * 3) = static_cast<T>(startIndex + p * 5 + 2);
						*reinterpret_cast<T*>(buffer + stride * 4) = static_cast<T>(startIndex + p * 5 + 4);
						*reinterpret_cast<T*>(buffer + stride * 5) = static_cast<T>(startIndex + p * 5 + 0);
						*reinterpret_cast<T*>(buffer + stride * 6) = static_cast<T>(startIndex + p * 5 + 1);
						*reinterpret_cast<T*>(buffer + stride * 7) = static_cast<T>(startIndex + p * 5 + 4);
						*reinterpret_cast<T*>(buffer + stride * 8) = static_cast<T>(startIndex + p * 5 + 3);
						*reinterpret_cast<T*>(buffer + stride * 9) = static_cast<T>(startIndex + p * 5 + 3);
						*reinterpret_cast<T*>(buffer + stride * 10) = static_cast<T>(startIndex + p * 5 + 4);
						*reinterpret_cast<T*>(buffer + stride * 11) = static_cast<T>(startIndex + p * 5 + 2);
						buffer += stride * 12;
					}
					break;
				default:
					for(std::uint32_t p = 0; p < segmentCount; p++) {
						*reinterpret_cast<T*>(buffer + stride * 0) = static_cast<T>(startIndex + p * 5 + 0);
						*reinterpret_cast<T*>(buffer + stride * 1) = static_cast<T>(startIndex + p * 5 + 1);
						*reinterpret_cast<T*>(buffer + stride * 2) = static_cast<T>(startIndex + p * 5 + 4);
						*reinterpret_cast<T*>(buffer + stride * 3) = static_cast<T>(startIndex + p * 5 + 2);
						*reinterpret_cast<T*>(buffer + stride * 4) = static_cast<T>(startIndex + p * 5 + 0);
						*reinterpret_cast<T*>(buffer + stride * 5) = static_cast<T>(startIndex + p * 5 + 4);
						*reinterpret_cast<T*>(buffer + stride * 6) = static_cast<T>(startIndex + p * 5 + 1);
						*reinterpret_cast<T*>(buffer + stride * 7) = static_cast<T>(startIndex + p * 5 + 3);
						*reinterpret_cast<T*>(buffer + stride * 8) = static_cast<T>(startIndex + p * 5 + 4);
						*reinterpret_cast<T*>(buffer + stride * 9) = static_cast<T>(startIndex + p * 5 + 3);
						*reinterpret_cast<T*>(buffer + stride * 10) = static_cast<T>(startIndex + p * 5 + 2);
						*reinterpret_cast<T*>(buffer + stride * 11) = static_cast<T>(startIndex + p * 5 + 4);
						buffer += stride * 12;
					}
					break;
			}

			startIndex += segmentCount * 5;
		}
	}

	template <typename T>
	void generateId(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
		std::size_t stride = attribute.byteStride;
		buffer += attribute.byteOffset;

		for(const auto& trailEntry : generatorTrails) {
			const Trail& trail = trailEntry.second;
			if(trail.position.size() < 2) {
				continue;
			}

			T trailId = static_cast<T>(trailEntry.first);

			switch(attribute.dataGenerationMode) {
				case VertexDataGenerationMode::vertex: {
					for(std::uint32_t p = 0; p < trail.position.size() - 1; p++) {
						*reinterpret_cast<T*>(buffer + stride * 0) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 1) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 2) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 3) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 4) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 5) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 6) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 7) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 8) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 9) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 10) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 11) = trailId;
						buffer += stride * 12;
					}

					break;
				}

				case VertexDataGenerationMode::element: {
					for(std::uint32_t p = 0; p < trail.position.size() - 1; p++) {
						*reinterpret_cast<T*>(buffer + stride * 0) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 1) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 2) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 3) = trailId;
						*reinterpret_cast<T*>(buffer + stride * 4) = trailId;
						buffer += stride * 5;
					}

					break;
				}

				default: {
					break;
				}
			}
		}
	}

	void generatePosition2d(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const SceneContext& sceneContext) const;
	void generatePosition3d(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const SceneContext& sceneContext) const;
	void generateNormal(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const SceneContext& sceneContext) const;
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

	template <typename T>
	static std::vector<typename Curve<T>::Point> orderedPoints(const float_t* positionList, const T* valueList, const std::uint32_t* order, std::uint32_t count) {
		std::vector<typename Curve<T>::Point> result;
		result.resize(count);
		for(std::uint32_t index = 0; index < count; index++) {
			result[index] = typename Curve<T>::Point{ positionList[index], valueList[order[index]] };
		}

		return result;
	}

	const Effect& generatorEffect;
	id_t generatorParticleEmitterId;
	id_t generatorParticleTypeId;

	VertexFormat generatorVertexFormat;

	std::unordered_map<std::uint32_t, Trail> generatorTrails;
};
}
