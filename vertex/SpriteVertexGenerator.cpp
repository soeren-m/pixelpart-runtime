#include "SpriteVertexGenerator.h"
#include "ParticleSorting.h"
#include "VertexDataGenerationMode.h"
#include "VertexFormatException.h"
#include "../common/Transform.h"
#include "../effect/ParticleSortCriterion.h"
#include "../effect/AlignmentMode.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "../glm/gtx/vector_angle.hpp"
#include <cmath>
#include <algorithm>

namespace pixelpart {
SpriteVertexGenerator::SpriteVertexGenerator(const Effect& effect, id_t particleEmitterId, id_t particleTypeId,
	const VertexFormat& vertexFormat) : VertexGenerator(),
	generatorEffect(effect),
	generatorParticleEmitterId(particleEmitterId), generatorParticleTypeId(particleTypeId),
	generatorVertexFormat(vertexFormat) {
	validateVertexFormat();
}

VertexDataBufferDimensions SpriteVertexGenerator::buildGeometry(
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const RuntimeContext& runtimeContext, const SceneContext& sceneContext) {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	ParticleSortCriterion sortCriterion = particleType.spriteRendererSettings().sortCriterion;

	if(generatorEffect.is3d() && sortCriterion != ParticleSortCriterion::none) {
		sortParticles(generatorSortedParticleCollection, particles, particleCount, sceneContext, sortCriterion);
	}

	VertexDataBufferDimensions result;

	for(const VertexAttribute& attribute : generatorVertexFormat.attributes()) {
		if(attribute.bufferIndex >= result.size()) {
			result.resize(attribute.bufferIndex + 1, 0);
		}

		std::uint32_t count = 0;
		switch(attribute.dataGenerationMode) {
			case VertexDataGenerationMode::vertex:
				count = particleCount * 6;
				break;
			case VertexDataGenerationMode::element:
				count = particleCount * 4;
				break;
			case VertexDataGenerationMode::instance:
				count = particleCount;
				break;
			default:
				break;
		}

		result[attribute.bufferIndex] = std::max(result[attribute.bufferIndex], count);
	}

	return result;
}

void SpriteVertexGenerator::generateVertexData(const VertexDataBufferCollection& dataBuffers,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	ParticleSortCriterion sortCriterion = particleType.spriteRendererSettings().sortCriterion;

	if(generatorEffect.is3d() && sortCriterion != ParticleSortCriterion::none) {
		particles = generatorSortedParticleCollection.readPtr();
	}

	for(const VertexAttribute& attribute : generatorVertexFormat.attributes()) {
		if(attribute.bufferIndex >= dataBuffers.size()) {
			continue;
		}

		std::uint8_t* buffer = dataBuffers[attribute.bufferIndex];

		switch(attribute.usage) {
			case VertexAttributeUsage::index:
				switch(attribute.componentType) {
					case VertexDataType::type_uint8:
						generateIndex<std::uint8_t>(buffer, attribute, particleCount);
						break;
					case VertexDataType::type_uint16:
						generateIndex<std::uint16_t>(buffer, attribute, particleCount);
						break;
					case VertexDataType::type_uint32:
						generateIndex<std::uint32_t>(buffer, attribute, particleCount);
						break;
					case VertexDataType::type_int8:
						generateIndex<std::int8_t>(buffer, attribute, particleCount);
						break;
					case VertexDataType::type_int16:
						generateIndex<std::int16_t>(buffer, attribute, particleCount);
						break;
					case VertexDataType::type_int32:
						generateIndex<std::int32_t>(buffer, attribute, particleCount);
						break;
					default:
						break;
				}
				break;
			case VertexAttributeUsage::position2d:
				generatePosition2d(buffer, attribute, particles, particleCount, runtimeContext, sceneContext);
				break;
			case VertexAttributeUsage::position3d:
				generatePosition3d(buffer, attribute, particles, particleCount, runtimeContext, sceneContext);
				break;
			case VertexAttributeUsage::normal:
				generateNormal(buffer, attribute, particles, particleCount, runtimeContext, sceneContext);
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

void SpriteVertexGenerator::validateVertexFormat() const {
	std::int32_t attributeIndex = 0;

	for(const VertexAttribute& attribute : generatorVertexFormat.attributes()) {
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
				if(attribute.dataGenerationMode == VertexDataGenerationMode::instance) {
					throw VertexFormatException("Unsupported data generation mode", attributeIndex);
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

void SpriteVertexGenerator::generatePosition2d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	float3_t emitterPosition = emitterTransform.position();
	mat3_t emitterRotationMatrix = rotationMatrix3d(emitterTransform.rotation());

	AlignmentMode alignmentMode = particleType.alignmentMode();
	float3_t pivot = particleType.pivot().value();

	glm::vec3 scale = glm::vec3(sceneContext.effectScale);
	glm::vec3 vertexPositions[4];

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t particlePosition = particles.globalPosition[p];
		float3_t particleSize = particles.size[p];
		float3_t particlePivot = pivot * particleSize;

		switch(particleType.alignmentMode()) {
			case AlignmentMode::motion: {
				float_t angle = glm::degrees(glm::orientedAngle(float2_t(0.0, 1.0), (particles.velocity[p] != float3_t(0.0))
					? float2_t(glm::normalize(particles.velocity[p])) : float2_t(0.0, 1.0)));
				vertexPositions[0] = particlePosition + rotatePoint2d(float3_t(-0.5, -0.5, 0.0) * particleSize, particlePivot, particles.rotation[p].x + angle);
				vertexPositions[1] = particlePosition + rotatePoint2d(float3_t(+0.5, -0.5, 0.0) * particleSize, particlePivot, particles.rotation[p].x + angle);
				vertexPositions[2] = particlePosition + rotatePoint2d(float3_t(+0.5, +0.5, 0.0) * particleSize, particlePivot, particles.rotation[p].x + angle);
				vertexPositions[3] = particlePosition + rotatePoint2d(float3_t(-0.5, +0.5, 0.0) * particleSize, particlePivot, particles.rotation[p].x + angle);
				break;
			}
			case AlignmentMode::emission: {
				float3_t emissionDirection = emitterPosition - particlePosition;
				float_t angle = glm::degrees(glm::orientedAngle(float2_t(0.0, 1.0), (emissionDirection != float3_t(0.0))
					? float2_t(glm::normalize(emissionDirection)) : float2_t(0.0, 1.0)));
				vertexPositions[0] = particlePosition + rotatePoint2d(float3_t(-0.5, -0.5, 0.0) * particleSize, particlePivot, particles.rotation[p].x + angle);
				vertexPositions[1] = particlePosition + rotatePoint2d(float3_t(+0.5, -0.5, 0.0) * particleSize, particlePivot, particles.rotation[p].x + angle);
				vertexPositions[2] = particlePosition + rotatePoint2d(float3_t(+0.5, +0.5, 0.0) * particleSize, particlePivot, particles.rotation[p].x + angle);
				vertexPositions[3] = particlePosition + rotatePoint2d(float3_t(-0.5, +0.5, 0.0) * particleSize, particlePivot, particles.rotation[p].x + angle);
				break;
			}
			case AlignmentMode::emitter: {
				mat3_t particleRotationMatrix = rotationMatrix3d(particles.rotation[p]);
				vertexPositions[0] = particlePosition + emitterRotationMatrix * (particleRotationMatrix * (float3_t(-0.5, -0.5, 0.0) * particleSize - particlePivot) + particlePivot);
				vertexPositions[1] = particlePosition + emitterRotationMatrix * (particleRotationMatrix * (float3_t(+0.5, -0.5, 0.0) * particleSize - particlePivot) + particlePivot);
				vertexPositions[2] = particlePosition + emitterRotationMatrix * (particleRotationMatrix * (float3_t(+0.5, +0.5, 0.0) * particleSize - particlePivot) + particlePivot);
				vertexPositions[3] = particlePosition + emitterRotationMatrix * (particleRotationMatrix * (float3_t(-0.5, +0.5, 0.0) * particleSize - particlePivot) + particlePivot);
				break;
			}
			default: {
				mat3_t particleRotationMatrix = rotationMatrix3d(particles.rotation[p]);
				vertexPositions[0] = particlePosition + (particleRotationMatrix * (float3_t(-0.5, -0.5, 0.0) * particleSize - particlePivot) + particlePivot);
				vertexPositions[1] = particlePosition + (particleRotationMatrix * (float3_t(+0.5, -0.5, 0.0) * particleSize - particlePivot) + particlePivot);
				vertexPositions[2] = particlePosition + (particleRotationMatrix * (float3_t(+0.5, +0.5, 0.0) * particleSize - particlePivot) + particlePivot);
				vertexPositions[3] = particlePosition + (particleRotationMatrix * (float3_t(-0.5, +0.5, 0.0) * particleSize - particlePivot) + particlePivot);
				break;
			}
		}

		vertexPositions[0] *= scale;
		vertexPositions[1] *= scale;
		vertexPositions[2] *= scale;
		vertexPositions[3] *= scale;

		switch(attribute.dataGenerationMode) {
			case VertexDataGenerationMode::vertex:
				switch(generatorVertexFormat.windingOrder()) {
					case VertexWindingOrder::cw:
						*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = vertexPositions[0];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = vertexPositions[3];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = vertexPositions[1];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = vertexPositions[1];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = vertexPositions[3];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 5) = vertexPositions[2];
						buffer += stride * 6;
						break;
					default:
						*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = vertexPositions[0];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = vertexPositions[1];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = vertexPositions[3];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = vertexPositions[1];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = vertexPositions[2];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 5) = vertexPositions[3];
						buffer += stride * 6;
						break;
				}
				break;
			case VertexDataGenerationMode::element:
				*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = vertexPositions[0];
				*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = vertexPositions[1];
				*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = vertexPositions[2];
				*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = vertexPositions[3];
				buffer += stride * 4;
				break;
			case VertexDataGenerationMode::instance:
				*reinterpret_cast<glm::vec2*>(buffer) = glm::vec3(particlePosition) * scale;
				buffer += stride;
				break;
			default:
				break;
		}
	}
}
void SpriteVertexGenerator::generatePosition3d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	float3_t emitterPosition = emitterTransform.position();
	glm::mat3 emitterRotationMatrix = rotationMatrix3d(emitterTransform.rotation());

	AlignmentMode alignmentMode = particleType.alignmentMode();
	glm::vec3 pivot = particleType.pivot().value();

	glm::vec3 cameraRight = glm::vec3(sceneContext.cameraRight);
	glm::vec3 cameraUp = glm::vec3(sceneContext.cameraUp);
	glm::vec3 scale = glm::vec3(sceneContext.effectScale);

	glm::vec3 vertexPositions[4];

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		glm::mat3 particleRotationMatrix = glm::mat3(rotationMatrix3d(particles.rotation[p]));
		glm::vec3 particlePosition = particles.globalPosition[p];
		glm::vec3 particleSize = particles.size[p];
		glm::vec3 particlePivot = pivot * particleSize;

		vertexPositions[0] = (particleRotationMatrix * (glm::vec3(-0.5f, -0.5f, 0.0f) * particleSize - particlePivot) + particlePivot) * scale;
		vertexPositions[1] = (particleRotationMatrix * (glm::vec3(+0.5f, -0.5f, 0.0f) * particleSize - particlePivot) + particlePivot) * scale;
		vertexPositions[2] = (particleRotationMatrix * (glm::vec3(+0.5f, +0.5f, 0.0f) * particleSize - particlePivot) + particlePivot) * scale;
		vertexPositions[3] = (particleRotationMatrix * (glm::vec3(-0.5f, +0.5f, 0.0f) * particleSize - particlePivot) + particlePivot) * scale;

		switch(alignmentMode) {
			case AlignmentMode::camera: {
				vertexPositions[0] = particlePosition * scale + cameraRight * vertexPositions[0].x + cameraUp * vertexPositions[0].y;
				vertexPositions[1] = particlePosition * scale + cameraRight * vertexPositions[1].x + cameraUp * vertexPositions[1].y;
				vertexPositions[2] = particlePosition * scale + cameraRight * vertexPositions[2].x + cameraUp * vertexPositions[2].y;
				vertexPositions[3] = particlePosition * scale + cameraRight * vertexPositions[3].x + cameraUp * vertexPositions[3].y;
				break;
			}
			case AlignmentMode::motion: {
				glm::mat3 lookAtMatrix = lookAtMatrix3d(particles.velocity[p] * sceneContext.effectScale);
				vertexPositions[0] = particlePosition * scale + lookAtMatrix * vertexPositions[0];
				vertexPositions[1] = particlePosition * scale + lookAtMatrix * vertexPositions[1];
				vertexPositions[2] = particlePosition * scale + lookAtMatrix * vertexPositions[2];
				vertexPositions[3] = particlePosition * scale + lookAtMatrix * vertexPositions[3];
				break;
			}
			case AlignmentMode::emission: {
				glm::mat3 lookAtMatrix = lookAtMatrix3d(emitterPosition - particles.globalPosition[p]);
				vertexPositions[0] = particlePosition * scale + lookAtMatrix * vertexPositions[0];
				vertexPositions[1] = particlePosition * scale + lookAtMatrix * vertexPositions[1];
				vertexPositions[2] = particlePosition * scale + lookAtMatrix * vertexPositions[2];
				vertexPositions[3] = particlePosition * scale + lookAtMatrix * vertexPositions[3];
				break;
			}
			case AlignmentMode::emitter: {
				vertexPositions[0] = particlePosition * scale + emitterRotationMatrix * vertexPositions[0];
				vertexPositions[1] = particlePosition * scale + emitterRotationMatrix * vertexPositions[1];
				vertexPositions[2] = particlePosition * scale + emitterRotationMatrix * vertexPositions[2];
				vertexPositions[3] = particlePosition * scale + emitterRotationMatrix * vertexPositions[3];
				break;
			}
			default: {
				vertexPositions[0] = particlePosition * scale + vertexPositions[0];
				vertexPositions[1] = particlePosition * scale + vertexPositions[1];
				vertexPositions[2] = particlePosition * scale + vertexPositions[2];
				vertexPositions[3] = particlePosition * scale + vertexPositions[3];
				break;
			}
		}

		switch(attribute.dataGenerationMode) {
			case VertexDataGenerationMode::vertex:
				switch(generatorVertexFormat.windingOrder()) {
					case VertexWindingOrder::cw:
						*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = vertexPositions[0];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = vertexPositions[3];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = vertexPositions[1];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = vertexPositions[1];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = vertexPositions[3];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 5) = vertexPositions[2];
						buffer += stride * 6;
						break;
					default:
						*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = vertexPositions[0];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = vertexPositions[1];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = vertexPositions[3];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = vertexPositions[1];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = vertexPositions[2];
						*reinterpret_cast<glm::vec3*>(buffer + stride * 5) = vertexPositions[3];
						buffer += stride * 6;
						break;
				}
				break;
			case VertexDataGenerationMode::element:
				*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = vertexPositions[0];
				*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = vertexPositions[1];
				*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = vertexPositions[2];
				*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = vertexPositions[3];
				buffer += stride * 4;
				break;
			case VertexDataGenerationMode::instance:
				*reinterpret_cast<glm::vec3*>(buffer) = particlePosition * scale;
				buffer += stride;
				break;
			default:
				break;
		}
	}
}
void SpriteVertexGenerator::generateNormal(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	float3_t emitterPosition = emitterTransform.position();
	glm::mat3 emitterRotationMatrix = rotationMatrix3d(emitterTransform.rotation());

	AlignmentMode alignmentMode = particleType.alignmentMode();

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		glm::mat3 particleRotationMatrix = glm::mat3(rotationMatrix3d(particles.rotation[p]));
		glm::vec3 particleNormal = glm::vec3(particleRotationMatrix * float3_t(0.0, 0.0, 1.0));

		switch(alignmentMode) {
			case AlignmentMode::camera:
				particleNormal = sceneContext.cameraPosition - particles.globalPosition[p];
				break;
			case AlignmentMode::motion:
				particleNormal = glm::mat3(lookAtMatrix3d(particles.velocity[p])) * particleNormal;
				break;
			case AlignmentMode::emission:
				particleNormal = glm::mat3(lookAtMatrix3d(emitterPosition - particles.globalPosition[p])) * particleNormal;
				break;
			case AlignmentMode::emitter:
				particleNormal = emitterRotationMatrix * particleNormal;
				break;
			default:
				break;
		}

		switch(attribute.dataGenerationMode) {
			case VertexDataGenerationMode::vertex:
				*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = particleNormal;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = particleNormal;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = particleNormal;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = particleNormal;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = particleNormal;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 5) = particleNormal;
				buffer += stride * 6;
				break;
			case VertexDataGenerationMode::element:
				*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = particleNormal;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = particleNormal;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = particleNormal;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = particleNormal;
				buffer += stride * 4;
				break;
			case VertexDataGenerationMode::instance:
				*reinterpret_cast<glm::vec3*>(buffer) = particleNormal;
				buffer += stride;
				break;
			default:
				break;
		}
	}
}
void SpriteVertexGenerator::generateTextureCoord(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	const glm::vec2 uv[] {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f)
	};

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	switch(attribute.dataGenerationMode) {
		case VertexDataGenerationMode::vertex: {
			switch(generatorVertexFormat.windingOrder()) {
				case VertexWindingOrder::cw:
					for(std::uint32_t p = 0; p < particleCount; p++) {
						*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = uv[0];
						*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = uv[3];
						*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = uv[1];
						*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = uv[1];
						*reinterpret_cast<glm::vec2*>(buffer + stride * 4) = uv[3];
						*reinterpret_cast<glm::vec2*>(buffer + stride * 5) = uv[2];
						buffer += stride * 6;
					}
					break;
				default:
					for(std::uint32_t p = 0; p < particleCount; p++) {
						*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = uv[0];
						*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = uv[1];
						*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = uv[3];
						*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = uv[1];
						*reinterpret_cast<glm::vec2*>(buffer + stride * 4) = uv[2];
						*reinterpret_cast<glm::vec2*>(buffer + stride * 5) = uv[3];
						buffer += stride * 6;
					}
					break;
			}

			break;
		}

		case VertexDataGenerationMode::element: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = uv[0];
				*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = uv[1];
				*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = uv[2];
				*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = uv[3];
				buffer += stride * 4;
			}

			break;
		}

		default: {
			break;
		}
	}
}
void SpriteVertexGenerator::generateColor(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	switch(attribute.dataGenerationMode) {
		case VertexDataGenerationMode::vertex: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				glm::vec4 particleColor = particles.color[p];

				*reinterpret_cast<glm::vec4*>(buffer + stride * 0) = particleColor;
				*reinterpret_cast<glm::vec4*>(buffer + stride * 1) = particleColor;
				*reinterpret_cast<glm::vec4*>(buffer + stride * 2) = particleColor;
				*reinterpret_cast<glm::vec4*>(buffer + stride * 3) = particleColor;
				*reinterpret_cast<glm::vec4*>(buffer + stride * 4) = particleColor;
				*reinterpret_cast<glm::vec4*>(buffer + stride * 5) = particleColor;
				buffer += stride * 6;
			}

			break;
		}

		case VertexDataGenerationMode::element: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				glm::vec4 particleColor = particles.color[p];

				*reinterpret_cast<glm::vec4*>(buffer + stride * 0) = particleColor;
				*reinterpret_cast<glm::vec4*>(buffer + stride * 1) = particleColor;
				*reinterpret_cast<glm::vec4*>(buffer + stride * 2) = particleColor;
				*reinterpret_cast<glm::vec4*>(buffer + stride * 3) = particleColor;
				buffer += stride * 4;
			}

			break;
		}

		case VertexDataGenerationMode::instance: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				*reinterpret_cast<glm::vec4*>(buffer) = glm::vec4(particles.color[p]);
				buffer += stride;
			}

			break;
		}

		default: {
			break;
		}
	}
}
void SpriteVertexGenerator::generateLife(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	switch(attribute.dataGenerationMode) {
		case VertexDataGenerationMode::vertex: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				float particleLife = static_cast<float>(particles.life[p]);

				*reinterpret_cast<float*>(buffer + stride * 0) = particleLife;
				*reinterpret_cast<float*>(buffer + stride * 1) = particleLife;
				*reinterpret_cast<float*>(buffer + stride * 2) = particleLife;
				*reinterpret_cast<float*>(buffer + stride * 3) = particleLife;
				*reinterpret_cast<float*>(buffer + stride * 4) = particleLife;
				*reinterpret_cast<float*>(buffer + stride * 5) = particleLife;
				buffer += stride * 6;
			}

			break;
		}

		case VertexDataGenerationMode::element: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				float particleLife = static_cast<float>(particles.life[p]);

				*reinterpret_cast<float*>(buffer + stride * 0) = particleLife;
				*reinterpret_cast<float*>(buffer + stride * 1) = particleLife;
				*reinterpret_cast<float*>(buffer + stride * 2) = particleLife;
				*reinterpret_cast<float*>(buffer + stride * 3) = particleLife;
				buffer += stride * 4;
			}

			break;
		}

		case VertexDataGenerationMode::instance: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				*reinterpret_cast<float*>(buffer) = static_cast<float>(particles.life[p]);
				buffer += stride;
			}

			break;
		}

		default: {
			break;
		}
	}
}
void SpriteVertexGenerator::generateVelocity2d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	switch(attribute.dataGenerationMode) {
		case VertexDataGenerationMode::vertex: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				glm::vec2 particleVelocity = particles.velocity[p];

				*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = particleVelocity;
				*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = particleVelocity;
				*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = particleVelocity;
				*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = particleVelocity;
				*reinterpret_cast<glm::vec2*>(buffer + stride * 4) = particleVelocity;
				*reinterpret_cast<glm::vec2*>(buffer + stride * 5) = particleVelocity;
				buffer += stride * 6;
			}

			break;
		}

		case VertexDataGenerationMode::element: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				glm::vec2 particleVelocity = particles.velocity[p];

				*reinterpret_cast<glm::vec2*>(buffer + stride * 0) = particleVelocity;
				*reinterpret_cast<glm::vec2*>(buffer + stride * 1) = particleVelocity;
				*reinterpret_cast<glm::vec2*>(buffer + stride * 2) = particleVelocity;
				*reinterpret_cast<glm::vec2*>(buffer + stride * 3) = particleVelocity;
				buffer += stride * 4;
			}

			break;
		}

		case VertexDataGenerationMode::instance: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				*reinterpret_cast<glm::vec2*>(buffer) = glm::vec2(particles.velocity[p]);
				buffer += stride;
			}

			break;
		}

		default: {
			break;
		}
	}
}
void SpriteVertexGenerator::generateVelocity3d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	switch(attribute.dataGenerationMode) {
		case VertexDataGenerationMode::vertex: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				glm::vec3 particleVelocity = particles.velocity[p];

				*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = particleVelocity;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = particleVelocity;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = particleVelocity;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = particleVelocity;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 4) = particleVelocity;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 5) = particleVelocity;
				buffer += stride * 6;
			}

			break;
		}

		case VertexDataGenerationMode::element: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				glm::vec3 particleVelocity = particles.velocity[p];

				*reinterpret_cast<glm::vec3*>(buffer + stride * 0) = particleVelocity;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 1) = particleVelocity;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 2) = particleVelocity;
				*reinterpret_cast<glm::vec3*>(buffer + stride * 3) = particleVelocity;
				buffer += stride * 4;
			}

			break;
		}

		case VertexDataGenerationMode::instance: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				*reinterpret_cast<glm::vec3*>(buffer) = glm::vec3(particles.velocity[p]);
				buffer += stride;
			}

			break;
		}

		default: {
			break;
		}
	}
}

float3_t SpriteVertexGenerator::rotatePoint2d(const float3_t& point, const float3_t& center, float_t angle) {
	float_t sa = std::sin(glm::radians(angle));
	float_t ca = std::cos(glm::radians(angle));

	return float3_t(
		center.x + (point.x - center.x) * ca - (point.y - center.y) * sa,
		center.y + (point.x - center.x) * sa + (point.y - center.y) * ca,
		0.0);
}
mat3_t SpriteVertexGenerator::rotationMatrix3d(const float3_t& rollYawPitch) {
	float3_t rollYawPitchRad = glm::radians(rollYawPitch);
	float_t cy = std::cos(rollYawPitchRad.y);
	float_t sy = std::sin(rollYawPitchRad.y);
	float_t cp = std::cos(rollYawPitchRad.z);
	float_t sp = std::sin(rollYawPitchRad.z);
	float_t cr = std::cos(rollYawPitchRad.x);
	float_t sr = std::sin(rollYawPitchRad.x);

	return mat3_t(
		float3_t(cy * cr + sy * sp * sr, sr * cp, -sy * cr + cy * sp * sr),
		float3_t(-cy * sr + sy * sp * cr, cr * cp, sr * sy + cy * sp * cr),
		float3_t(sy * cp, -sp, cy * cp));
}
mat3_t SpriteVertexGenerator::lookAtMatrix3d(const float3_t& direction) {
	float3_t up = worldUpVector3;
	float3_t forward = glm::normalize(direction);
	float3_t right = glm::normalize(glm::cross(forward, up));
	up = glm::normalize(glm::cross(right, forward));

	return mat3_t(right, up, -forward);
}
}
