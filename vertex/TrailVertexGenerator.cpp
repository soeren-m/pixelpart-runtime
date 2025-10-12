#include "TrailVertexGenerator.h"
#include "VertexDataGenerationMode.h"
#include "VertexFormatException.h"
#include "../common/Transform.h"
#include "../effect/AlignmentMode.h"
#include "../effect/ParticleRendererSettings.h"
#include <cmath>
#include <algorithm>

namespace pixelpart {
TrailVertexGenerator::TrailVertexGenerator(const Effect& effect, id_t particleEmitterId, id_t particleTypeId,
	const VertexFormat& vertexFormat) : VertexGenerator(),
	generatorEffect(effect),
	generatorParticleEmitterId(particleEmitterId), generatorParticleTypeId(particleTypeId),
	generatorVertexFormat(vertexFormat) {
	validateVertexFormat();
}

VertexDataBufferDimensions TrailVertexGenerator::buildGeometry(
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) {
	const float_t epsilon = 0.000001;

	const ParticleEmitter& particleEmitter = generatorEffect.sceneGraph().at<ParticleEmitter>(generatorParticleEmitterId);
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);

	VertexDataBufferDimensions result;
	for(const VertexAttribute& attribute : generatorVertexFormat.attributes()) {
		if(attribute.bufferIndex >= result.size()) {
			result.resize(attribute.bufferIndex + 1, 0);
		}
	}

	if(particleCount < 2) {
		return result;
	}

	std::vector<std::uint32_t> sortKeys(particleCount);
	std::iota(sortKeys.begin(), sortKeys.end(), 0);
	std::sort(sortKeys.begin(), sortKeys.end(), [&particles](std::uint32_t p0, std::uint32_t p1) {
		return (particles.parentId[p0] != particles.parentId[p1])
			? particles.parentId[p0] > particles.parentId[p1]
			: particles.id[p0] > particles.id[p1];
	});

	for(auto& trailEntry : generatorTrails) {
		trailEntry.second.particleCount = 0;
	}

	Trail* currentTrail = nullptr;
	for(std::uint32_t p = 0, particleParentId = 0, trailId = 0xFFFFFFFE; p < particleCount; p++) {
		particleParentId = particles.parentId[sortKeys[p]];

		if(particleParentId != trailId) {
			trailId = particleParentId;
			currentTrail = &generatorTrails[trailId];
		}

		currentTrail->particleCount++;
	}

	for(auto trailIt = generatorTrails.begin(); trailIt != generatorTrails.end(); ) {
		if(trailIt->second.particleCount == 0) {
			trailIt = generatorTrails.erase(trailIt);
		}
		else {
			trailIt++;
		}
	}

	currentTrail = nullptr;

	switch(particleType.trailRendererSettings().smoothingMethod) {
		case ParticleTrailRendererSettings::SmoothingMethod::spline: {
			for(std::uint32_t p = 0, particleParentId = 0, trailId = 0xFFFFFFFE; p < particleCount; p++) {
				std::uint32_t particleIndex = sortKeys[p];
				particleParentId = particles.parentId[particleIndex];

				if(particleParentId != trailId) {
					trailId = particleParentId;
					currentTrail = &generatorTrails.at(trailId);

					std::vector<float_t> relativeParticlePosition(currentTrail->particleCount);
					relativeParticlePosition[0] = 0.0;

					currentTrail->length = 0.0;
					for(std::uint32_t q = 1; q < currentTrail->particleCount; q++) {
						currentTrail->length += glm::length(particles.globalPosition[sortKeys[p + q]] - particles.globalPosition[sortKeys[p + q - 1]]);
						relativeParticlePosition[q] = currentTrail->length;
					}
					for(std::uint32_t q = 1; q < currentTrail->particleCount; q++) {
						relativeParticlePosition[q] /= currentTrail->length;
					}

					Curve<float3_t> positionCurve(CurveInterpolation::spline);
					positionCurve.enableFixedCache(particleType.trailRendererSettings().smoothingSegmentCount);
					positionCurve.points(orderedPoints(relativeParticlePosition.data(), particles.globalPosition, sortKeys.data() + p, currentTrail->particleCount));

					Curve<float3_t> sizeCurve(CurveInterpolation::spline);
					sizeCurve.enableFixedCache(particleType.trailRendererSettings().smoothingSegmentCount);
					sizeCurve.points(orderedPoints(relativeParticlePosition.data(), particles.size, sortKeys.data() + p, currentTrail->particleCount));

					Curve<float4_t> colorCurve(CurveInterpolation::spline);
					colorCurve.enableFixedCache(particleType.trailRendererSettings().smoothingSegmentCount);
					colorCurve.points(orderedPoints(relativeParticlePosition.data(), particles.color, sortKeys.data() + p, currentTrail->particleCount));

					Curve<float3_t> velocityCurve(CurveInterpolation::spline);
					velocityCurve.enableFixedCache(particleType.trailRendererSettings().smoothingSegmentCount);
					velocityCurve.points(orderedPoints(relativeParticlePosition.data(), particles.velocity, sortKeys.data() + p, currentTrail->particleCount));

					Curve<float_t> lifeCurve(CurveInterpolation::spline);
					lifeCurve.enableFixedCache(particleType.trailRendererSettings().smoothingSegmentCount);
					lifeCurve.points(orderedPoints(relativeParticlePosition.data(), particles.life, sortKeys.data() + p, currentTrail->particleCount));

					currentTrail->position = positionCurve.cache();
					currentTrail->size = sizeCurve.cache();
					currentTrail->color = colorCurve.cache();
					currentTrail->velocity = velocityCurve.cache();
					currentTrail->life = lifeCurve.cache();
					currentTrail->direction.resize(particleType.trailRendererSettings().smoothingSegmentCount);
					currentTrail->directionToEdge.resize(particleType.trailRendererSettings().smoothingSegmentCount);
					currentTrail->index.resize(particleType.trailRendererSettings().smoothingSegmentCount);
				}
			}

			break;
		}

		default: {
			for(std::uint32_t p = 0, q = 0, particleParentId = 0, trailId = 0xFFFFFFFE; p < particleCount; p++, q++) {
				std::uint32_t particleIndex = sortKeys[p];
				particleParentId = particles.parentId[particleIndex];

				if(particleParentId != trailId) {
					trailId = particleParentId;
					q = 0;

					currentTrail = &generatorTrails.at(trailId);
					currentTrail->position.resize(currentTrail->particleCount);
					currentTrail->size.resize(currentTrail->particleCount);
					currentTrail->color.resize(currentTrail->particleCount);
					currentTrail->velocity.resize(currentTrail->particleCount);
					currentTrail->life.resize(currentTrail->particleCount);
					currentTrail->direction.resize(currentTrail->particleCount);
					currentTrail->directionToEdge.resize(currentTrail->particleCount);
					currentTrail->index.resize(currentTrail->particleCount);
				}

				currentTrail->position[q] = particles.globalPosition[particleIndex];
				currentTrail->size[q] = particles.size[particleIndex];
				currentTrail->color[q] = particles.color[particleIndex];
				currentTrail->velocity[q] = particles.velocity[particleIndex];
				currentTrail->life[q] = particles.life[particleIndex];
			}

			break;
		}
	}

	for(auto& trailEntry : generatorTrails) {
		Trail& trail = trailEntry.second;
		trail.length = 0.0;

		if(trail.position.size() < 2) {
			continue;
		}

		std::size_t lastTrailIndex = trail.position.size() - 1;
		for(std::size_t trailIndex = 0; trailIndex < lastTrailIndex; trailIndex++) {
			float3_t toNext = trail.position[trailIndex + 1] - trail.position[trailIndex];
			float_t distance = std::max(glm::length(toNext), epsilon);

			trail.direction[trailIndex] = toNext / distance;
			trail.index[trailIndex] = trail.length;
			trail.length += distance;
		}

		trail.direction[lastTrailIndex] = trail.direction[lastTrailIndex - 1];
		trail.index[lastTrailIndex] = trail.length;

		for(std::size_t trailIndex = lastTrailIndex; trailIndex > 0; trailIndex--) {
			float3_t toEdge = trail.direction[trailIndex] - trail.direction[trailIndex - 1];
			float_t toEdgeLength = glm::length(toEdge);
			if(toEdgeLength < epsilon) {
				trail.directionToEdge[trailIndex] = float3_t(
					-trail.direction[trailIndex].y,
					trail.direction[trailIndex].x,
					trail.direction[trailIndex].z);
			}
			else {
				trail.directionToEdge[trailIndex] = (glm::dot(glm::cross(trail.direction[trailIndex], float3_t(0.0, 0.0, 1.0)), toEdge / toEdgeLength) < 0.0)
					? +toEdge / toEdgeLength
					: -toEdge / toEdgeLength;
			}
		}

		trail.directionToEdge[0] = trail.directionToEdge[1];

		for(float_t& index : trail.index) {
			index /= trail.length;
		}
	}

	for(const VertexAttribute& attribute : generatorVertexFormat.attributes()) {
		std::uint32_t count = 0;

		for(const auto& trailEntry : generatorTrails) {
			const Trail& trail = trailEntry.second;
			if(trail.position.empty()) {
				continue;
			}

			switch(attribute.dataGenerationMode) {
				case VertexDataGenerationMode::vertex:
					count += (static_cast<std::uint32_t>(trail.position.size()) - 1) * 12;
					break;
				case VertexDataGenerationMode::element:
					count += (static_cast<std::uint32_t>(trail.position.size()) - 1) * 5;
					break;
				default:
					break;
			}
		}

		result[attribute.bufferIndex] = std::max(result[attribute.bufferIndex], count);
	}

	return result;
}

void TrailVertexGenerator::generateVertexData(const VertexDataBufferCollection& dataBuffers,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	for(const VertexAttribute& attribute : generatorVertexFormat.attributes()) {
		if(attribute.bufferIndex >= dataBuffers.size()) {
			continue;
		}

		std::uint8_t* buffer = dataBuffers[attribute.bufferIndex];

		switch(attribute.usage) {
			case VertexAttributeUsage::index:
				switch(attribute.componentType) {
					case VertexDataType::type_uint8:
						generateIndex<std::uint8_t>(buffer, attribute);
						break;
					case VertexDataType::type_uint16:
						generateIndex<std::uint16_t>(buffer, attribute);
						break;
					case VertexDataType::type_uint32:
						generateIndex<std::uint32_t>(buffer, attribute);
						break;
					case VertexDataType::type_int8:
						generateIndex<std::int8_t>(buffer, attribute);
						break;
					case VertexDataType::type_int16:
						generateIndex<std::int16_t>(buffer, attribute);
						break;
					case VertexDataType::type_int32:
						generateIndex<std::int32_t>(buffer, attribute);
						break;
					default:
						break;
				}
				break;
			case VertexAttributeUsage::position2d:
				generatePosition2d(buffer, attribute, particles, particleCount, sceneContext);
				break;
			case VertexAttributeUsage::position3d:
				generatePosition3d(buffer, attribute, particles, particleCount, sceneContext);
				break;
			case VertexAttributeUsage::normal:
				generateNormal(buffer, attribute, particles, particleCount, sceneContext);
				break;
			case VertexAttributeUsage::texture_coord:
				generateTextureCoord(buffer, attribute, particles, particleCount);
				break;
			case VertexAttributeUsage::color:
				generateColor(buffer, attribute, particles, particleCount);
				break;
			case VertexAttributeUsage::life:
				generateLife(buffer, attribute, particles, particleCount);
				break;
			case VertexAttributeUsage::id:
				switch(attribute.componentType) {
					case VertexDataType::type_uint8:
						generateId<std::uint8_t>(buffer, attribute, particles, particleCount);
						break;
					case VertexDataType::type_uint16:
						generateId<std::uint16_t>(buffer, attribute, particles, particleCount);
						break;
					case VertexDataType::type_uint32:
						generateId<std::uint32_t>(buffer, attribute, particles, particleCount);
						break;
					case VertexDataType::type_int8:
						generateId<std::int8_t>(buffer, attribute, particles, particleCount);
						break;
					case VertexDataType::type_int16:
						generateId<std::int16_t>(buffer, attribute, particles, particleCount);
						break;
					case VertexDataType::type_int32:
						generateId<std::int32_t>(buffer, attribute, particles, particleCount);
						break;
					case VertexDataType::type_float:
						generateId<float>(buffer, attribute, particles, particleCount);
						break;
					default:
						break;
				}
				break;
			case VertexAttributeUsage::velocity2d:
				generateVelocity2d(buffer, attribute, particles, particleCount);
				break;
			case VertexAttributeUsage::velocity3d:
				generateVelocity3d(buffer, attribute, particles, particleCount);
				break;
			default:
				break;
		}
	}
}

void TrailVertexGenerator::validateVertexFormat() const {
	std::int32_t attributeIndex = 0;

	for(const VertexAttribute& attribute : generatorVertexFormat.attributes()) {
		if(attribute.dataGenerationMode == VertexDataGenerationMode::instance) {
			throw VertexFormatException("Unsupported data generation mode", attributeIndex);
		}

		switch(attribute.usage) {
			case VertexAttributeUsage::index:
				if(attribute.componentType == VertexDataType::type_float) {
					throw VertexFormatException("Unsupported component type", attributeIndex);
				}
				if(attribute.dataGenerationMode != VertexDataGenerationMode::vertex) {
					throw VertexFormatException("Unsupported data generation mode", attributeIndex);
				}
				break;
			case VertexAttributeUsage::position2d:
			case VertexAttributeUsage::position3d:
			case VertexAttributeUsage::normal:
			case VertexAttributeUsage::color:
			case VertexAttributeUsage::life:
			case VertexAttributeUsage::velocity2d:
			case VertexAttributeUsage::velocity3d:
				if(attribute.componentType != VertexDataType::type_float) {
					throw VertexFormatException("Unsupported component type", attributeIndex);
				}
				break;
			case VertexAttributeUsage::texture_coord:
				if(attribute.componentType != VertexDataType::type_float) {
					throw VertexFormatException("Unsupported component type", attributeIndex);
				}
				break;
			case VertexAttributeUsage::id:
				break;
			default:
				throw VertexFormatException("Unsupported vertex attribute", attributeIndex);
		}

		attributeIndex++;
	}
}

void TrailVertexGenerator::generatePosition2d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const SceneContext& sceneContext) const {
	float3_t vertices[5];

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(const auto& trailEntry : generatorTrails) {
		const Trail& trail = trailEntry.second;
		if(trail.position.size() < 2) {
			continue;
		}

		for(std::uint32_t p = 0; p < trail.position.size() - 1; p++) {
			float3_t startToEdgeDirection = trail.directionToEdge[p];
			float3_t endToEdgeDirection = trail.directionToEdge[p + 1];

			float3_t startToEdge = startToEdgeDirection * std::max(trail.size[p].x, trail.size[p].y) * 0.5;
			float3_t endToEdge = endToEdgeDirection * std::max(trail.size[p + 1].x, trail.size[p + 1].y) * 0.5;

			vertices[0] = (trail.position[p] + startToEdge) * sceneContext.effectScale;
			vertices[1] = (trail.position[p] - startToEdge) * sceneContext.effectScale;
			vertices[2] = (trail.position[p + 1] + endToEdge) * sceneContext.effectScale;
			vertices[3] = (trail.position[p + 1] - endToEdge) * sceneContext.effectScale;
			vertices[4] = (trail.position[p] + trail.position[p + 1]) * 0.5 * sceneContext.effectScale;

			switch(attribute.dataGenerationMode) {
				case VertexDataGenerationMode::vertex: {
					switch(generatorVertexFormat.windingOrder()) {
						case VertexWindingOrder::cw:
							*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = vertices[0];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = vertices[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = vertices[1];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = vertices[2];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 4) = vertices[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 5) = vertices[0];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 6) = vertices[1];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 7) = vertices[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 8) = vertices[3];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 9) = vertices[3];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 10) = vertices[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 11) = vertices[2];
							buffer += stride * 12;
							break;
						default:
							*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = vertices[0];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = vertices[1];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = vertices[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = vertices[2];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 4) = vertices[0];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 5) = vertices[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 6) = vertices[1];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 7) = vertices[3];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 8) = vertices[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 9) = vertices[3];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 10) = vertices[2];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 11) = vertices[4];
							buffer += stride * 12;
							break;
					}

					break;
				}

				case VertexDataGenerationMode::element: {
					*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = vertices[0];
					*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = vertices[1];
					*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = vertices[2];
					*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = vertices[3];
					*reinterpret_cast<glm::vec2*>(buffer + stride * 4) = vertices[4];
					buffer += stride * 5;

					break;
				}

				default: {
					break;
				}
			}
		}
	}
}
void TrailVertexGenerator::generatePosition3d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	AlignmentMode alignmentMode = particleType.alignmentMode();

	float3_t vertices[5];

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(const auto& trailEntry : generatorTrails) {
		const Trail& trail = trailEntry.second;
		if(trail.position.size() < 2) {
			continue;
		}

		for(std::uint32_t p = 0; p < trail.position.size() - 1; p++) {
			float3_t startToEdgeDirection = trail.directionToEdge[p];
			float3_t endToEdgeDirection = trail.directionToEdge[p + 1];

			if(alignmentMode == AlignmentMode::camera) {
				float3_t cameraToStart = glm::normalize(sceneContext.cameraPosition - trail.position[p]);
				float3_t cameraToEnd = glm::normalize(sceneContext.cameraPosition - trail.position[p + 1]);
				startToEdgeDirection = glm::normalize(glm::cross(cameraToStart, trail.direction[p]));
				endToEdgeDirection = glm::normalize(glm::cross(cameraToEnd, trail.direction[p + 1]));
			}

			float3_t startToEdge = startToEdgeDirection * std::max(trail.size[p].x, std::max(trail.size[p].y, trail.size[p].z)) * 0.5;
			float3_t endToEdge = endToEdgeDirection * std::max(trail.size[p + 1].x, std::max(trail.size[p + 1].y, trail.size[p + 1].z)) * 0.5;

			vertices[0] = (trail.position[p] + startToEdge) * sceneContext.effectScale;
			vertices[1] = (trail.position[p] - startToEdge) * sceneContext.effectScale;
			vertices[2] = (trail.position[p + 1] + endToEdge) * sceneContext.effectScale;
			vertices[3] = (trail.position[p + 1] - endToEdge) * sceneContext.effectScale;
			vertices[4] = (trail.position[p] + trail.position[p + 1]) * 0.5 * sceneContext.effectScale;

			switch(attribute.dataGenerationMode) {
				case VertexDataGenerationMode::vertex: {
					switch(generatorVertexFormat.windingOrder()) {
						case VertexWindingOrder::cw:
							*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = vertices[0];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = vertices[4];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = vertices[1];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = vertices[2];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = vertices[4];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 5) = vertices[0];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 6) = vertices[1];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 7) = vertices[4];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 8) = vertices[3];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 9) = vertices[3];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 10) = vertices[4];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 11) = vertices[2];
							buffer += stride * 12;
							break;
						default:
							*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = vertices[0];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = vertices[1];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = vertices[4];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = vertices[2];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = vertices[0];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 5) = vertices[4];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 6) = vertices[1];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 7) = vertices[3];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 8) = vertices[4];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 9) = vertices[3];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 10) = vertices[2];
							*reinterpret_cast<glm::vec3*>(buffer + stride * 11) = vertices[4];
							buffer += stride * 12;
							break;
					}

					break;
				}

				case VertexDataGenerationMode::element: {
					*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = vertices[0];
					*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = vertices[1];
					*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = vertices[2];
					*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = vertices[3];
					*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = vertices[4];
					buffer += stride * 5;

					break;
				}

				default: {
					break;
				}
			}
		}
	}
}
void TrailVertexGenerator::generateNormal(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	AlignmentMode alignmentMode = particleType.alignmentMode();

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(const auto& trailEntry : generatorTrails) {
		const Trail& trail = trailEntry.second;
		if(trail.direction.size() < 2) {
			continue;
		}

		for(std::uint32_t p = 0; p < trail.direction.size() - 1; p++) {
			float3_t startToEdgeDirection = trail.directionToEdge[p];
			float3_t endToEdgeDirection = trail.directionToEdge[p + 1];

			if(alignmentMode == AlignmentMode::camera) {
				float3_t cameraToStart = glm::normalize(sceneContext.cameraPosition - trail.position[p]);
				float3_t cameraToEnd = glm::normalize(sceneContext.cameraPosition - trail.position[p + 1]);
				startToEdgeDirection = glm::normalize(glm::cross(cameraToStart, trail.direction[p]));
				endToEdgeDirection = glm::normalize(glm::cross(cameraToEnd, trail.direction[p + 1]));
			}

			glm::vec3 normal = glm::cross(trail.direction[p], startToEdgeDirection);
			glm::vec3 nextNormal = glm::cross(trail.direction[p + 1], endToEdgeDirection);
			glm::vec3 centerNormal = (normal + nextNormal) * 0.5f;

			switch(attribute.dataGenerationMode) {
				case VertexDataGenerationMode::vertex:
					switch(generatorVertexFormat.windingOrder()) {
						case VertexWindingOrder::cw:
							*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = normal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = centerNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = normal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = nextNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = centerNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 5) = normal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 6) = normal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 7) = centerNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 8) = nextNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 9) = nextNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 10) = centerNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 11) = nextNormal;
							buffer += stride * 12;
							break;
						default:
							*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = normal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = normal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = centerNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = nextNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = normal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 5) = centerNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 6) = normal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 7) = nextNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 8) = centerNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 9) = nextNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 10) = nextNormal;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 11) = centerNormal;
							buffer += stride * 12;
							break;
					}
					break;
				case VertexDataGenerationMode::element:
					*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = normal;
					*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = normal;
					*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = nextNormal;
					*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = nextNormal;
					*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = centerNormal;
					buffer += stride * 5;
					break;
				default:
					break;
			}
		}
	}
}
void TrailVertexGenerator::generateTextureCoord(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	ParticleTrailRendererSettings::TextureRotation textureRotation =
		particleType.trailRendererSettings().textureRotation;
	float uvFactor =
		static_cast<float>(particleType.trailRendererSettings().textureUVFactor);

	glm::vec2 textureCoords[5];

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(const auto& trailEntry : generatorTrails) {
		const Trail& trail = trailEntry.second;
		if(trail.index.size() < 2) {
			continue;
		}

		for(std::uint32_t p = 0; p < trail.index.size() - 1; p++) {
			float index = static_cast<float>(trail.index[p]);
			float nextIndex = static_cast<float>(trail.index[p + 1]);

			switch(textureRotation) {
				case ParticleTrailRendererSettings::TextureRotation::left:
					textureCoords[0] = glm::vec2(index * uvFactor, 1.0f);
					textureCoords[1] = glm::vec2(index * uvFactor, 0.0f);
					textureCoords[2] = glm::vec2(nextIndex * uvFactor, 1.0f);
					textureCoords[3] = glm::vec2(nextIndex * uvFactor, 0.0f);
					textureCoords[4] = glm::vec2((index + nextIndex) * 0.5f * uvFactor, 0.5f);
					break;
				case ParticleTrailRendererSettings::TextureRotation::down:
					textureCoords[0] = glm::vec2(1.0f, 1.0f - index * uvFactor);
					textureCoords[1] = glm::vec2(0.0f, 1.0f - index * uvFactor);
					textureCoords[2] = glm::vec2(1.0f, 1.0f - nextIndex * uvFactor);
					textureCoords[3] = glm::vec2(0.0f, 1.0f - nextIndex * uvFactor);
					textureCoords[4] = glm::vec2(0.5f, 1.0f - (index + nextIndex) * 0.5f * uvFactor);
					break;
				case ParticleTrailRendererSettings::TextureRotation::right:
					textureCoords[0] = glm::vec2(1.0f - index * uvFactor, 0.0f);
					textureCoords[1] = glm::vec2(1.0f - index * uvFactor, 1.0f);
					textureCoords[2] = glm::vec2(1.0f - nextIndex * uvFactor, 0.0f);
					textureCoords[3] = glm::vec2(1.0f - nextIndex * uvFactor, 1.0f);
					textureCoords[4] = glm::vec2(1.0f - (index + nextIndex) * 0.5f * uvFactor, 0.5f);
					break;
				default:
					textureCoords[0] = glm::vec2(0.0f, index * uvFactor);
					textureCoords[1] = glm::vec2(1.0f, index * uvFactor);
					textureCoords[2] = glm::vec2(0.0f, nextIndex * uvFactor);
					textureCoords[3] = glm::vec2(1.0f, nextIndex * uvFactor);
					textureCoords[4] = glm::vec2(0.5f, (index + nextIndex) * 0.5f * uvFactor);
					break;
			}

			switch(attribute.dataGenerationMode) {
				case VertexDataGenerationMode::vertex: {
					switch(generatorVertexFormat.windingOrder()) {
						case VertexWindingOrder::cw:
							*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = textureCoords[0];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = textureCoords[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = textureCoords[1];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = textureCoords[2];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 4) = textureCoords[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 5) = textureCoords[0];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 6) = textureCoords[1];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 7) = textureCoords[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 8) = textureCoords[3];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 9) = textureCoords[3];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 10) = textureCoords[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 11) = textureCoords[2];
							buffer += stride * 12;
							break;
						default:
							*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = textureCoords[0];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = textureCoords[1];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = textureCoords[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = textureCoords[2];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 4) = textureCoords[0];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 5) = textureCoords[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 6) = textureCoords[1];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 7) = textureCoords[3];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 8) = textureCoords[4];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 9) = textureCoords[3];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 10) = textureCoords[2];
							*reinterpret_cast<glm::vec2*>(buffer + stride * 11) = textureCoords[4];
							buffer += stride * 12;
							break;
					}

					break;
				}

				case VertexDataGenerationMode::element: {
					*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = textureCoords[0];
					*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = textureCoords[1];
					*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = textureCoords[2];
					*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = textureCoords[3];
					*reinterpret_cast<glm::vec2*>(buffer + stride * 4) = textureCoords[4];
					buffer += stride * 5;
					break;
				}

				default: {
					break;
				}
			}
		}
	}
}
void TrailVertexGenerator::generateColor(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(const auto& trailEntry : generatorTrails) {
		const Trail& trail = trailEntry.second;
		if(trail.color.size() < 2) {
			continue;
		}

		switch(attribute.dataGenerationMode) {
			case VertexDataGenerationMode::vertex: {
				switch(generatorVertexFormat.windingOrder()) {
					case VertexWindingOrder::cw:
						for(std::uint32_t p = 0; p < trail.color.size() - 1; p++) {
							glm::vec2 velocity = trail.velocity[p];
							glm::vec2 nextVelocity = trail.velocity[p + 1];
							glm::vec2 centerVelocity = (velocity + nextVelocity) * 0.5f;

							*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = velocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = centerVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = velocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = nextVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 4) = centerVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 5) = velocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 6) = velocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 7) = centerVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 8) = nextVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 9) = nextVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 10) = centerVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 11) = nextVelocity;
							buffer += stride * 12;
						}
						break;
					default:
						for(std::uint32_t p = 0; p < trail.color.size() - 1; p++) {
							glm::vec4 color = trail.color[p];
							glm::vec4 nextColor = trail.color[p + 1];
							glm::vec4 centerColor = (color + nextColor) * 0.5f;

							*reinterpret_cast<glm::vec4*>(buffer + stride * 0) = color;
							*reinterpret_cast<glm::vec4*>(buffer + stride * 1) = color;
							*reinterpret_cast<glm::vec4*>(buffer + stride * 2) = centerColor;
							*reinterpret_cast<glm::vec4*>(buffer + stride * 3) = nextColor;
							*reinterpret_cast<glm::vec4*>(buffer + stride * 4) = color;
							*reinterpret_cast<glm::vec4*>(buffer + stride * 5) = centerColor;
							*reinterpret_cast<glm::vec4*>(buffer + stride * 6) = color;
							*reinterpret_cast<glm::vec4*>(buffer + stride * 7) = nextColor;
							*reinterpret_cast<glm::vec4*>(buffer + stride * 8) = centerColor;
							*reinterpret_cast<glm::vec4*>(buffer + stride * 9) = nextColor;
							*reinterpret_cast<glm::vec4*>(buffer + stride * 10) = nextColor;
							*reinterpret_cast<glm::vec4*>(buffer + stride * 11) = centerColor;
							buffer += stride * 12;
						}
						break;
				}

				break;
			}

			case VertexDataGenerationMode::element: {
				for(std::uint32_t p = 0; p < trail.color.size() - 1; p++) {
					glm::vec4 color = trail.color[p];
					glm::vec4 nextColor = trail.color[p + 1];

					*reinterpret_cast<glm::vec4*>(buffer + stride * 0) = color;
					*reinterpret_cast<glm::vec4*>(buffer + stride * 1) = color;
					*reinterpret_cast<glm::vec4*>(buffer + stride * 2) = nextColor;
					*reinterpret_cast<glm::vec4*>(buffer + stride * 3) = nextColor;
					*reinterpret_cast<glm::vec4*>(buffer + stride * 4) = (color + nextColor) * 0.5f;
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
void TrailVertexGenerator::generateLife(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(const auto& trailEntry : generatorTrails) {
		const Trail& trail = trailEntry.second;
		if(trail.life.size() < 2) {
			continue;
		}

		switch(attribute.dataGenerationMode) {
			case VertexDataGenerationMode::vertex: {
				switch(generatorVertexFormat.windingOrder()) {
					case VertexWindingOrder::cw:
						for(std::uint32_t p = 0; p < trail.life.size() - 1; p++) {
							float life = static_cast<float>(trail.life[p]);
							float nextLife = static_cast<float>(trail.life[p + 1]);
							float centerLife = (life + nextLife) * 0.5f;

							*reinterpret_cast<float*>(buffer + stride * 0) = life;
							*reinterpret_cast<float*>(buffer + stride * 1) = centerLife;
							*reinterpret_cast<float*>(buffer + stride * 2) = life;
							*reinterpret_cast<float*>(buffer + stride * 3) = nextLife;
							*reinterpret_cast<float*>(buffer + stride * 4) = centerLife;
							*reinterpret_cast<float*>(buffer + stride * 5) = life;
							*reinterpret_cast<float*>(buffer + stride * 6) = life;
							*reinterpret_cast<float*>(buffer + stride * 7) = centerLife;
							*reinterpret_cast<float*>(buffer + stride * 8) = nextLife;
							*reinterpret_cast<float*>(buffer + stride * 9) = nextLife;
							*reinterpret_cast<float*>(buffer + stride * 10) = centerLife;
							*reinterpret_cast<float*>(buffer + stride * 11) = nextLife;
							buffer += stride * 12;
						}
						break;
					default:
						for(std::uint32_t p = 0; p < trail.life.size() - 1; p++) {
							float life = static_cast<float>(trail.life[p]);
							float nextLife = static_cast<float>(trail.life[p + 1]);
							float centerLife = (life + nextLife) * 0.5f;

							*reinterpret_cast<float*>(buffer + stride * 0) = life;
							*reinterpret_cast<float*>(buffer + stride * 1) = life;
							*reinterpret_cast<float*>(buffer + stride * 2) = centerLife;
							*reinterpret_cast<float*>(buffer + stride * 3) = nextLife;
							*reinterpret_cast<float*>(buffer + stride * 4) = life;
							*reinterpret_cast<float*>(buffer + stride * 5) = centerLife;
							*reinterpret_cast<float*>(buffer + stride * 6) = life;
							*reinterpret_cast<float*>(buffer + stride * 7) = nextLife;
							*reinterpret_cast<float*>(buffer + stride * 8) = centerLife;
							*reinterpret_cast<float*>(buffer + stride * 9) = nextLife;
							*reinterpret_cast<float*>(buffer + stride * 10) = nextLife;
							*reinterpret_cast<float*>(buffer + stride * 11) = centerLife;
							buffer += stride * 12;
						}
						break;
				}

				break;
			}

			case VertexDataGenerationMode::element: {
				for(std::uint32_t p = 0; p < trail.life.size() - 1; p++) {
					float life = static_cast<float>(trail.life[p]);
					float nextLife = static_cast<float>(trail.life[p + 1]);

					*reinterpret_cast<float*>(buffer + stride * 0) = life;
					*reinterpret_cast<float*>(buffer + stride * 1) = life;
					*reinterpret_cast<float*>(buffer + stride * 2) = nextLife;
					*reinterpret_cast<float*>(buffer + stride * 3) = nextLife;
					*reinterpret_cast<float*>(buffer + stride * 4) = (life + nextLife) * 0.5f;
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
void TrailVertexGenerator::generateVelocity2d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(const auto& trailEntry : generatorTrails) {
		const Trail& trail = trailEntry.second;
		if(trail.velocity.size() < 2) {
			continue;
		}

		switch(attribute.dataGenerationMode) {
			case VertexDataGenerationMode::vertex: {
				switch(generatorVertexFormat.windingOrder()) {
					case VertexWindingOrder::cw:
						for(std::uint32_t p = 0; p < trail.velocity.size() - 1; p++) {
							glm::vec2 velocity = trail.velocity[p];
							glm::vec2 nextVelocity = trail.velocity[p + 1];
							glm::vec2 centerVelocity = (velocity + nextVelocity) * 0.5f;

							*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = velocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = centerVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = velocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = nextVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 4) = centerVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 5) = velocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 6) = velocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 7) = centerVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 8) = nextVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 9) = nextVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 10) = centerVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 11) = nextVelocity;
							buffer += stride * 12;
						}
						break;
					default:
						for(std::uint32_t p = 0; p < trail.velocity.size() - 1; p++) {
							glm::vec2 velocity = trail.velocity[p];
							glm::vec2 nextVelocity = trail.velocity[p + 1];
							glm::vec2 centerVelocity = (velocity + nextVelocity) * 0.5f;

							*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = velocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = velocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = centerVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = nextVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 4) = velocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 5) = centerVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 6) = velocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 7) = nextVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 8) = centerVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 9) = nextVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 10) = nextVelocity;
							*reinterpret_cast<glm::vec2*>(buffer + stride * 11) = centerVelocity;
							buffer += stride * 12;
						}
						break;
				}

				break;
			}

			case VertexDataGenerationMode::element: {
				for(std::uint32_t p = 0; p < trail.velocity.size() - 1; p++) {
					glm::vec2 velocity = trail.velocity[p];
					glm::vec2 nextVelocity = trail.velocity[p + 1];

					*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = velocity;
					*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = velocity;
					*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = nextVelocity;
					*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = nextVelocity;
					*reinterpret_cast<glm::vec2*>(buffer + stride * 4) = (velocity + nextVelocity) * 0.5f;
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
void TrailVertexGenerator::generateVelocity3d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(const auto& trailEntry : generatorTrails) {
		const Trail& trail = trailEntry.second;
		if(trail.velocity.size() < 2) {
			continue;
		}

		switch(attribute.dataGenerationMode) {
			case VertexDataGenerationMode::vertex: {
				switch(generatorVertexFormat.windingOrder()) {
					case VertexWindingOrder::cw:
						for(std::uint32_t p = 0; p < trail.velocity.size() - 1; p++) {
							glm::vec3 velocity = trail.velocity[p];
							glm::vec3 nextVelocity = trail.velocity[p + 1];
							glm::vec3 centerVelocity = (velocity + nextVelocity) * 0.5f;

							*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = velocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = centerVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = velocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = nextVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = centerVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 5) = velocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 6) = velocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 7) = centerVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 8) = nextVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 9) = nextVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 10) = centerVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 11) = nextVelocity;
							buffer += stride * 12;
						}
						break;
					default:
						for(std::uint32_t p = 0; p < trail.velocity.size() - 1; p++) {
							glm::vec3 velocity = trail.velocity[p];
							glm::vec3 nextVelocity = trail.velocity[p + 1];
							glm::vec3 centerVelocity = (velocity + nextVelocity) * 0.5f;

							*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = velocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = velocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = centerVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = nextVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = velocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 5) = centerVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 6) = velocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 7) = nextVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 8) = centerVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 9) = nextVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 10) = nextVelocity;
							*reinterpret_cast<glm::vec3*>(buffer + stride * 11) = centerVelocity;
							buffer += stride * 12;
						}
						break;
				}

				break;
			}

			case VertexDataGenerationMode::element: {
				for(std::uint32_t p = 0; p < trail.velocity.size() - 1; p++) {
					glm::vec3 velocity = trail.velocity[p];
					glm::vec3 nextVelocity = trail.velocity[p + 1];

					*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = velocity;
					*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = velocity;
					*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = nextVelocity;
					*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = nextVelocity;
					*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = (velocity + nextVelocity) * 0.5f;
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
}
