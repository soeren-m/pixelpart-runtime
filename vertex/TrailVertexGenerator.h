#pragma once

#include "VertexGenerator.h"
#include "VertexAttribute.h"
#include "VertexWindingOrder.h"
#include "../effect/Curve.h"
#include "../platform/ThreadPool.h"
#include "../types/Types.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_map>

namespace pixelpart {
class TrailVertexGenerator : public VertexGenerator {
public:
	TrailVertexGenerator(const Effect& effect, id_t particleEmitterId, id_t particleTypeId,
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

	void buildStandardTrailSegments(pixelpart::ParticleCollection::ReadPtr particles, std::uint32_t particleCount);
	void buildSmoothTrailSegments(pixelpart::ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		std::uint32_t segmentCountPerTrail);

	template <typename T>
	void generateIndex(std::uint8_t* buffer, const VertexAttribute& attribute) const {
		std::size_t stride = attribute.byteStride;
		buffer += attribute.byteOffset;

		switch(generatorVertexFormat.windingOrder()) {
			case VertexWindingOrder::cw: {
				for(std::uint32_t s = 0, v = 0; s < generatorTotalSegmentVertexCount - 1; s++) {
					if(generatorTrailIds[s] != generatorTrailIds[s + 1]) {
						continue;
					}

					*reinterpret_cast<T*>(buffer + stride * 0) = static_cast<T>(v * 5 + 0);
					*reinterpret_cast<T*>(buffer + stride * 1) = static_cast<T>(v * 5 + 4);
					*reinterpret_cast<T*>(buffer + stride * 2) = static_cast<T>(v * 5 + 1);
					*reinterpret_cast<T*>(buffer + stride * 3) = static_cast<T>(v * 5 + 2);
					*reinterpret_cast<T*>(buffer + stride * 4) = static_cast<T>(v * 5 + 4);
					*reinterpret_cast<T*>(buffer + stride * 5) = static_cast<T>(v * 5 + 0);
					*reinterpret_cast<T*>(buffer + stride * 6) = static_cast<T>(v * 5 + 1);
					*reinterpret_cast<T*>(buffer + stride * 7) = static_cast<T>(v * 5 + 4);
					*reinterpret_cast<T*>(buffer + stride * 8) = static_cast<T>(v * 5 + 3);
					*reinterpret_cast<T*>(buffer + stride * 9) = static_cast<T>(v * 5 + 3);
					*reinterpret_cast<T*>(buffer + stride * 10) = static_cast<T>(v * 5 + 4);
					*reinterpret_cast<T*>(buffer + stride * 11) = static_cast<T>(v * 5 + 2);
					buffer += stride * 12;

					v++;
				}

				break;
			}

			default: {
				for(std::uint32_t s = 0, v = 0; s < generatorTotalSegmentVertexCount - 1; s++) {
					if(generatorTrailIds[s] != generatorTrailIds[s + 1]) {
						continue;
					}

					*reinterpret_cast<T*>(buffer + stride * 0) = static_cast<T>(v * 5 + 0);
					*reinterpret_cast<T*>(buffer + stride * 1) = static_cast<T>(v * 5 + 1);
					*reinterpret_cast<T*>(buffer + stride * 2) = static_cast<T>(v * 5 + 4);
					*reinterpret_cast<T*>(buffer + stride * 3) = static_cast<T>(v * 5 + 2);
					*reinterpret_cast<T*>(buffer + stride * 4) = static_cast<T>(v * 5 + 0);
					*reinterpret_cast<T*>(buffer + stride * 5) = static_cast<T>(v * 5 + 4);
					*reinterpret_cast<T*>(buffer + stride * 6) = static_cast<T>(v * 5 + 1);
					*reinterpret_cast<T*>(buffer + stride * 7) = static_cast<T>(v * 5 + 3);
					*reinterpret_cast<T*>(buffer + stride * 8) = static_cast<T>(v * 5 + 4);
					*reinterpret_cast<T*>(buffer + stride * 9) = static_cast<T>(v * 5 + 3);
					*reinterpret_cast<T*>(buffer + stride * 10) = static_cast<T>(v * 5 + 2);
					*reinterpret_cast<T*>(buffer + stride * 11) = static_cast<T>(v * 5 + 4);
					buffer += stride * 12;

					v++;
				}

				break;
			}
		}
	}

	template <typename T>
	void generateId(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
		std::size_t stride = attribute.byteStride;
		buffer += attribute.byteOffset;

		switch(attribute.dataGenerationMode) {
			case VertexDataGenerationMode::vertex: {
				for(std::uint32_t s = 0; s < generatorTotalSegmentVertexCount - 1; s++) {
					if(generatorTrailIds[s] != generatorTrailIds[s + 1]) {
						continue;
					}

					T trailId = static_cast<T>(generatorTrailIds[s]);

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
				for(std::uint32_t s = 0; s < generatorTotalSegmentVertexCount - 1; s++) {
					if(generatorTrailIds[s] != generatorTrailIds[s + 1]) {
						continue;
					}

					T trailId = static_cast<T>(generatorTrailIds[s]);

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
	void generateColorFloat(std::uint8_t* buffer, const VertexAttribute& attribute,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const;
	void generateColorByte(std::uint8_t* buffer, const VertexAttribute& attribute,
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

	std::shared_ptr<ThreadPool> generatorThreadPool;

	std::uint32_t generatorTotalSegmentVertexCount = 0;
	std::vector<std::uint32_t> generatorSortKeys;
	std::vector<std::uint32_t> generatorTrailIds;
	std::vector<float> generatorTrailIndices;
	std::vector<float> generatorTrailLengths;
	std::vector<float> generatorLives;
	std::vector<math::vector3<float>> generatorPositions;
	std::vector<float> generatorWidths;
	std::vector<math::vector4<float>> generatorColors;
	std::vector<math::vector3<float>> generatorVelocities;
	std::vector<math::vector3<float>> generatorTangents;
	std::vector<math::vector3<float>> generatorSideDirections;
	// TODO: vector<bool> generatorDegenerate
};
}
