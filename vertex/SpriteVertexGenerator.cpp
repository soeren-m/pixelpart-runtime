#include "SpriteVertexGenerator.h"
#include "ParticleSorting.h"
#include "VertexDataGenerationMode.h"
#include "VertexFormatException.h"
#include "../common/Transform.h"
#include "../common/Coordinates.h"
#include "../math/Common.h"
#include "../math/MatrixCommon.h"
#include "../math/Geometry.h"
#include "../math/Trigonometry.h"
#include "../math/Transformation.h"
#include "../effect/ParticleSortCriterion.h"
#include "../effect/AlignmentMode.h"
#include <cmath>
#include <algorithm>

namespace pixelpart {
SpriteVertexGenerator::SpriteVertexGenerator(const Effect& effect, id_t particleEmitterId, id_t particleTypeId,
	const VertexFormat& vertexFormat,
	std::shared_ptr<ThreadPool> threadPool) : VertexGenerator(),
	generatorEffect(effect),
	generatorParticleEmitterId(particleEmitterId), generatorParticleTypeId(particleTypeId),
	generatorVertexFormat(vertexFormat),
	generatorThreadPool(threadPool) {
	validateVertexFormat();
}

VertexDataBufferDimensions SpriteVertexGenerator::buildGeometry(
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	ParticleSortCriterion sortCriterion = particleType.spriteRendererSettings().sortCriterion;

	if(generatorEffect.is3d() && sortCriterion != ParticleSortCriterion::none) {
		if(generatorThreadPool) {
			sortParticles(generatorSortedParticleCollection, particles, particleCount,
				sceneContext, sortCriterion, *generatorThreadPool);
		}
		else {
			sortParticles(generatorSortedParticleCollection, particles, particleCount,
				sceneContext, sortCriterion);
		}
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
	const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
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
				switch(attribute.componentType) {
					case VertexDataType::type_uint8:
						generateColorByte(buffer, attribute, particles, particleCount);
						break;
					case VertexDataType::type_float:
						generateColorFloat(buffer, attribute, particles, particleCount);
						break;
					default:
						break;
				}
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
			case VertexAttributeUsage::color:
				if(attribute.componentType != VertexDataType::type_uint8 && attribute.componentType != VertexDataType::type_float) {
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

void SpriteVertexGenerator::generatePosition2d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	math::vector2<float> emitterPosition(emitterTransform.position());
	math::matrix3x3<float> emitterRotationMatrix(rotationMatrix3d(emitterTransform.rotation()));

	AlignmentMode alignmentMode = particleType.alignmentMode();
	math::vector3<float> pivot(particleType.pivot().value());

	math::vector2<float> scale(sceneContext.effectScale);
	math::vector2<float> vertexPositions[4];

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		switch(particleType.alignmentMode()) {
			case AlignmentMode::motion: {
				math::vector2<float> particlePosition(particles.globalPosition[p]);
				math::vector2<float> particleSize(particles.size[p]);
				math::vector2<float> particlePivot = math::vector2<float>(pivot) * particleSize;
				float angle = math::radians(static_cast<float>(particles.rotation[p].x) + math::degrees(math::orientedAngle(math::vector2<float>(worldUpVector2), particles.velocity[p] != float3_t(0.0)
					? math::vector2<float>(math::normalize(particles.velocity[p]))
					: math::vector2<float>(worldUpVector2))));
				vertexPositions[0] = particlePosition + math::rotatePoint(math::vector2<float>(-0.5f, -0.5f) * particleSize, particlePivot, angle);
				vertexPositions[1] = particlePosition + math::rotatePoint(math::vector2<float>(+0.5f, -0.5f) * particleSize, particlePivot, angle);
				vertexPositions[2] = particlePosition + math::rotatePoint(math::vector2<float>(+0.5f, +0.5f) * particleSize, particlePivot, angle);
				vertexPositions[3] = particlePosition + math::rotatePoint(math::vector2<float>(-0.5f, +0.5f) * particleSize, particlePivot, angle);
				break;
			}
			case AlignmentMode::emission: {
				math::vector2<float> particlePosition(particles.globalPosition[p]);
				math::vector2<float> particleSize(particles.size[p]);
				math::vector2<float> particlePivot = math::vector2<float>(pivot) * particleSize;
				math::vector2<float> emissionDirection = emitterPosition - particlePosition;
				float angle = math::radians(static_cast<float>(particles.rotation[p].x) + math::degrees(math::orientedAngle(math::vector2<float>(worldUpVector2), emissionDirection != math::vector2<float>(0.0f)
					? math::normalize(emissionDirection)
					: math::vector2<float>(worldUpVector2))));
				vertexPositions[0] = particlePosition + math::rotatePoint(math::vector2<float>(-0.5f, -0.5f) * particleSize, particlePivot, angle);
				vertexPositions[1] = particlePosition + math::rotatePoint(math::vector2<float>(+0.5f, -0.5f) * particleSize, particlePivot, angle);
				vertexPositions[2] = particlePosition + math::rotatePoint(math::vector2<float>(+0.5f, +0.5f) * particleSize, particlePivot, angle);
				vertexPositions[3] = particlePosition + math::rotatePoint(math::vector2<float>(-0.5f, +0.5f) * particleSize, particlePivot, angle);
				break;
			}
			case AlignmentMode::emitter: {
				math::vector3<float> particlePosition(particles.globalPosition[p]);
				math::vector3<float> particleSize(particles.size[p]);
				math::vector3<float> particlePivot = pivot * particleSize;
				math::matrix3x3<float> particleRotationMatrix = math::matrix3x3<float>(rotationMatrix3d(particles.rotation[p]));
				vertexPositions[0] = math::vector2<float>(particlePosition + emitterRotationMatrix * (particleRotationMatrix * (math::vector3<float>(-0.5f, -0.5f, 0.0f) * particleSize - particlePivot) + particlePivot));
				vertexPositions[1] = math::vector2<float>(particlePosition + emitterRotationMatrix * (particleRotationMatrix * (math::vector3<float>(+0.5f, -0.5f, 0.0f) * particleSize - particlePivot) + particlePivot));
				vertexPositions[2] = math::vector2<float>(particlePosition + emitterRotationMatrix * (particleRotationMatrix * (math::vector3<float>(+0.5f, +0.5f, 0.0f) * particleSize - particlePivot) + particlePivot));
				vertexPositions[3] = math::vector2<float>(particlePosition + emitterRotationMatrix * (particleRotationMatrix * (math::vector3<float>(-0.5f, +0.5f, 0.0f) * particleSize - particlePivot) + particlePivot));
				break;
			}
			default: {
				math::vector3<float> particlePosition(particles.globalPosition[p]);
				math::vector3<float> particleSize(particles.size[p]);
				math::vector3<float> particlePivot = pivot * particleSize;
				math::matrix3x3<float> particleRotationMatrix = math::matrix3x3<float>(rotationMatrix3d(particles.rotation[p]));
				vertexPositions[0] = math::vector2<float>(particlePosition + (particleRotationMatrix * (math::vector3<float>(-0.5f, -0.5f, 0.0f) * particleSize - particlePivot) + particlePivot));
				vertexPositions[1] = math::vector2<float>(particlePosition + (particleRotationMatrix * (math::vector3<float>(+0.5f, -0.5f, 0.0f) * particleSize - particlePivot) + particlePivot));
				vertexPositions[2] = math::vector2<float>(particlePosition + (particleRotationMatrix * (math::vector3<float>(+0.5f, +0.5f, 0.0f) * particleSize - particlePivot) + particlePivot));
				vertexPositions[3] = math::vector2<float>(particlePosition + (particleRotationMatrix * (math::vector3<float>(-0.5f, +0.5f, 0.0f) * particleSize - particlePivot) + particlePivot));
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
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 0) = vertexPositions[0];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 1) = vertexPositions[3];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 2) = vertexPositions[1];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 3) = vertexPositions[1];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 4) = vertexPositions[3];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 5) = vertexPositions[2];
						buffer += stride * 6;
						break;
					default:
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 0) = vertexPositions[0];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 1) = vertexPositions[1];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 2) = vertexPositions[3];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 3) = vertexPositions[1];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 4) = vertexPositions[2];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 5) = vertexPositions[3];
						buffer += stride * 6;
						break;
				}
				break;
			case VertexDataGenerationMode::element:
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 0) = vertexPositions[0];
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 1) = vertexPositions[1];
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 2) = vertexPositions[2];
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 3) = vertexPositions[3];
				buffer += stride * 4;
				break;
			case VertexDataGenerationMode::instance:
				*reinterpret_cast<math::vector2<float>*>(buffer) = math::vector2<float>(particles.globalPosition[p]) * scale;
				buffer += stride;
				break;
			default:
				break;
		}
	}
}
void SpriteVertexGenerator::generatePosition3d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	float3_t emitterPosition = emitterTransform.position();
	math::matrix3x3<float> emitterRotationMatrix(rotationMatrix3d(emitterTransform.rotation()));

	AlignmentMode alignmentMode = particleType.alignmentMode();
	math::vector3<float> pivot(particleType.pivot().value());

	math::vector3<float> cameraRight(sceneContext.cameraRight);
	math::vector3<float> cameraUp(sceneContext.cameraUp);
	math::vector3<float> scale(sceneContext.effectScale);

	math::vector3<float> vertexPositions[4];

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		math::matrix3x3<float> particleRotationMatrix(rotationMatrix3d(particles.rotation[p]));
		math::vector3<float> particlePosition(particles.globalPosition[p]);
		math::vector3<float> particleSize(particles.size[p]);
		math::vector3<float> particlePivot = pivot * particleSize;

		vertexPositions[0] = (particleRotationMatrix * (math::vector3<float>(-0.5f, -0.5f, 0.0f) * particleSize - particlePivot) + particlePivot) * scale;
		vertexPositions[1] = (particleRotationMatrix * (math::vector3<float>(+0.5f, -0.5f, 0.0f) * particleSize - particlePivot) + particlePivot) * scale;
		vertexPositions[2] = (particleRotationMatrix * (math::vector3<float>(+0.5f, +0.5f, 0.0f) * particleSize - particlePivot) + particlePivot) * scale;
		vertexPositions[3] = (particleRotationMatrix * (math::vector3<float>(-0.5f, +0.5f, 0.0f) * particleSize - particlePivot) + particlePivot) * scale;

		switch(alignmentMode) {
			case AlignmentMode::camera: {
				vertexPositions[0] = particlePosition * scale + cameraRight * vertexPositions[0].x + cameraUp * vertexPositions[0].y;
				vertexPositions[1] = particlePosition * scale + cameraRight * vertexPositions[1].x + cameraUp * vertexPositions[1].y;
				vertexPositions[2] = particlePosition * scale + cameraRight * vertexPositions[2].x + cameraUp * vertexPositions[2].y;
				vertexPositions[3] = particlePosition * scale + cameraRight * vertexPositions[3].x + cameraUp * vertexPositions[3].y;
				break;
			}
			case AlignmentMode::motion: {
				math::matrix3x3<float> lookAtMatrix(math::transpose(math::lookAtMatrix(particles.velocity[p] * sceneContext.effectScale, worldUpVector3)));
				vertexPositions[0] = particlePosition * scale + lookAtMatrix * vertexPositions[0];
				vertexPositions[1] = particlePosition * scale + lookAtMatrix * vertexPositions[1];
				vertexPositions[2] = particlePosition * scale + lookAtMatrix * vertexPositions[2];
				vertexPositions[3] = particlePosition * scale + lookAtMatrix * vertexPositions[3];
				break;
			}
			case AlignmentMode::emission: {
				math::matrix3x3<float> lookAtMatrix(math::transpose(math::lookAtMatrix(emitterPosition - particles.globalPosition[p], worldUpVector3)));
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
						*reinterpret_cast<math::vector3<float>*>(buffer + stride * 0) = vertexPositions[0];
						*reinterpret_cast<math::vector3<float>*>(buffer + stride * 1) = vertexPositions[3];
						*reinterpret_cast<math::vector3<float>*>(buffer + stride * 2) = vertexPositions[1];
						*reinterpret_cast<math::vector3<float>*>(buffer + stride * 3) = vertexPositions[1];
						*reinterpret_cast<math::vector3<float>*>(buffer + stride * 4) = vertexPositions[3];
						*reinterpret_cast<math::vector3<float>*>(buffer + stride * 5) = vertexPositions[2];
						buffer += stride * 6;
						break;
					default:
						*reinterpret_cast<math::vector3<float>*>(buffer + stride * 0) = vertexPositions[0];
						*reinterpret_cast<math::vector3<float>*>(buffer + stride * 1) = vertexPositions[1];
						*reinterpret_cast<math::vector3<float>*>(buffer + stride * 2) = vertexPositions[3];
						*reinterpret_cast<math::vector3<float>*>(buffer + stride * 3) = vertexPositions[1];
						*reinterpret_cast<math::vector3<float>*>(buffer + stride * 4) = vertexPositions[2];
						*reinterpret_cast<math::vector3<float>*>(buffer + stride * 5) = vertexPositions[3];
						buffer += stride * 6;
						break;
				}
				break;
			case VertexDataGenerationMode::element:
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 0) = vertexPositions[0];
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 1) = vertexPositions[1];
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 2) = vertexPositions[2];
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 3) = vertexPositions[3];
				buffer += stride * 4;
				break;
			case VertexDataGenerationMode::instance:
				*reinterpret_cast<math::vector3<float>*>(buffer) = particlePosition * scale;
				buffer += stride;
				break;
			default:
				break;
		}
	}
}
void SpriteVertexGenerator::generateNormal(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);

	math::vector3<float> upVector3(worldUpVector3);
	math::vector3<float> cameraPosition(sceneContext.cameraPosition);

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	math::vector3<float> emitterPosition(emitterTransform.position());
	math::matrix3x3<float> emitterRotationMatrix(rotationMatrix3d(emitterTransform.rotation()));

	AlignmentMode alignmentMode = particleType.alignmentMode();

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		math::matrix3x3<float> particleRotationMatrix(rotationMatrix3d(particles.rotation[p]));
		math::vector3<float> particleNormal = particleRotationMatrix * math::vector3<float>(0.0f, 0.0f, 1.0f);

		switch(alignmentMode) {
			case AlignmentMode::camera:
				particleNormal = cameraPosition - math::vector3<float>(particles.globalPosition[p]);
				break;
			case AlignmentMode::motion:
				particleNormal = math::transpose(math::lookAtMatrix(math::vector3<float>(particles.velocity[p]), upVector3)) * particleNormal;
				break;
			case AlignmentMode::emission:
				particleNormal = math::transpose(math::lookAtMatrix(emitterPosition - math::vector3<float>(particles.globalPosition[p]), upVector3)) * particleNormal;
				break;
			case AlignmentMode::emitter:
				particleNormal = emitterRotationMatrix * particleNormal;
				break;
			default:
				break;
		}

		switch(attribute.dataGenerationMode) {
			case VertexDataGenerationMode::vertex:
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 0) = particleNormal;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 1) = particleNormal;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 2) = particleNormal;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 3) = particleNormal;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 4) = particleNormal;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 5) = particleNormal;
				buffer += stride * 6;
				break;
			case VertexDataGenerationMode::element:
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 0) = particleNormal;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 1) = particleNormal;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 2) = particleNormal;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 3) = particleNormal;
				buffer += stride * 4;
				break;
			case VertexDataGenerationMode::instance:
				*reinterpret_cast<math::vector3<float>*>(buffer) = particleNormal;
				buffer += stride;
				break;
			default:
				break;
		}
	}
}
void SpriteVertexGenerator::generateTextureCoord(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	const math::vector2<float> uv[] {
		math::vector2<float>(0.0f, 0.0f),
		math::vector2<float>(1.0f, 0.0f),
		math::vector2<float>(1.0f, 1.0f),
		math::vector2<float>(0.0f, 1.0f)
	};

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	switch(attribute.dataGenerationMode) {
		case VertexDataGenerationMode::vertex: {
			switch(generatorVertexFormat.windingOrder()) {
				case VertexWindingOrder::cw:
					for(std::uint32_t p = 0; p < particleCount; p++) {
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 0) = uv[0];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 1) = uv[3];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 2) = uv[1];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 3) = uv[1];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 4) = uv[3];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 5) = uv[2];
						buffer += stride * 6;
					}
					break;
				default:
					for(std::uint32_t p = 0; p < particleCount; p++) {
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 0) = uv[0];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 1) = uv[1];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 2) = uv[3];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 3) = uv[1];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 4) = uv[2];
						*reinterpret_cast<math::vector2<float>*>(buffer + stride * 5) = uv[3];
						buffer += stride * 6;
					}
					break;
			}

			break;
		}

		case VertexDataGenerationMode::element: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 0) = uv[0];
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 1) = uv[1];
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 2) = uv[2];
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 3) = uv[3];
				buffer += stride * 4;
			}

			break;
		}

		default: {
			break;
		}
	}
}
void SpriteVertexGenerator::generateColorFloat(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	switch(attribute.dataGenerationMode) {
		case VertexDataGenerationMode::vertex: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				math::vector4<float> particleColor(particles.color[p]);

				*reinterpret_cast<math::vector4<float>*>(buffer + stride * 0) = particleColor;
				*reinterpret_cast<math::vector4<float>*>(buffer + stride * 1) = particleColor;
				*reinterpret_cast<math::vector4<float>*>(buffer + stride * 2) = particleColor;
				*reinterpret_cast<math::vector4<float>*>(buffer + stride * 3) = particleColor;
				*reinterpret_cast<math::vector4<float>*>(buffer + stride * 4) = particleColor;
				*reinterpret_cast<math::vector4<float>*>(buffer + stride * 5) = particleColor;
				buffer += stride * 6;
			}

			break;
		}

		case VertexDataGenerationMode::element: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				math::vector4<float> particleColor(particles.color[p]);

				*reinterpret_cast<math::vector4<float>*>(buffer + stride * 0) = particleColor;
				*reinterpret_cast<math::vector4<float>*>(buffer + stride * 1) = particleColor;
				*reinterpret_cast<math::vector4<float>*>(buffer + stride * 2) = particleColor;
				*reinterpret_cast<math::vector4<float>*>(buffer + stride * 3) = particleColor;
				buffer += stride * 4;
			}

			break;
		}

		case VertexDataGenerationMode::instance: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				*reinterpret_cast<math::vector4<float>*>(buffer) = math::vector4<float>(particles.color[p]);
				buffer += stride;
			}

			break;
		}

		default: {
			break;
		}
	}
}
void SpriteVertexGenerator::generateColorByte(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	switch(attribute.dataGenerationMode) {
		case VertexDataGenerationMode::vertex: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				ColorUByte color(particles.color[p]);

				for(std::uint32_t v = 0; v < 6; v++) {
					*(buffer + 0) = color.r;
					*(buffer + 1) = color.g;
					*(buffer + 2) = color.b;
					*(buffer + 3) = color.a;
					buffer += stride;
				}
			}

			break;
		}

		case VertexDataGenerationMode::element: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				ColorUByte color(particles.color[p]);

				for(std::uint32_t v = 0; v < 4; v++) {
					*(buffer + 0) = color.r;
					*(buffer + 1) = color.g;
					*(buffer + 2) = color.b;
					*(buffer + 3) = color.a;
					buffer += stride;
				}
			}

			break;
		}

		case VertexDataGenerationMode::instance: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				ColorUByte color(particles.color[p]);

				*(buffer + 0) = color.r;
				*(buffer + 1) = color.g;
				*(buffer + 2) = color.b;
				*(buffer + 3) = color.a;
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
				math::vector2<float> particleVelocity(particles.velocity[p]);

				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 0) = particleVelocity;
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 1) = particleVelocity;
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 2) = particleVelocity;
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 3) = particleVelocity;
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 4) = particleVelocity;
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 5) = particleVelocity;
				buffer += stride * 6;
			}

			break;
		}

		case VertexDataGenerationMode::element: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				math::vector2<float> particleVelocity(particles.velocity[p]);

				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 0) = particleVelocity;
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 1) = particleVelocity;
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 2) = particleVelocity;
				*reinterpret_cast<math::vector2<float>*>(buffer + stride * 3) = particleVelocity;
				buffer += stride * 4;
			}

			break;
		}

		case VertexDataGenerationMode::instance: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				*reinterpret_cast<math::vector2<float>*>(buffer) = math::vector2<float>(particles.velocity[p]);
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
				math::vector3<float> particleVelocity(particles.velocity[p]);

				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 0) = particleVelocity;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 1) = particleVelocity;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 2) = particleVelocity;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 3) = particleVelocity;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 4) = particleVelocity;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 5) = particleVelocity;
				buffer += stride * 6;
			}

			break;
		}

		case VertexDataGenerationMode::element: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				math::vector3<float> particleVelocity(particles.velocity[p]);

				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 0) = particleVelocity;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 1) = particleVelocity;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 2) = particleVelocity;
				*reinterpret_cast<math::vector3<float>*>(buffer + stride * 3) = particleVelocity;
				buffer += stride * 4;
			}

			break;
		}

		case VertexDataGenerationMode::instance: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				*reinterpret_cast<math::vector3<float>*>(buffer) = math::vector3<float>(particles.velocity[p]);
				buffer += stride;
			}

			break;
		}

		default: {
			break;
		}
	}
}

matrix3_t SpriteVertexGenerator::rotationMatrix3d(const float3_t& rollYawPitch) {
	float3_t rollYawPitchRad = math::radians(rollYawPitch);

	return matrix3_t(math::yawPitchRollRotationMatrix(rollYawPitchRad.y, rollYawPitchRad.z, rollYawPitchRad.x));
}
}
