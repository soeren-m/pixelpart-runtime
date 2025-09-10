#include "MeshVertexGenerator.h"
#include "ParticleSorting.h"
#include "VertexDataGenerationMode.h"
#include "VertexFormatException.h"
#include "../common/Transform.h"
#include "../effect/ParticleSortCriterion.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "../glm/gtx/transform.hpp"
#include "../glm/gtx/matrix_major_storage.hpp"
#include <cmath>
#include <algorithm>

namespace pixelpart {
MeshVertexGenerator::MeshVertexGenerator(const Effect& effect, id_t particleEmitterId, id_t particleTypeId,
	const VertexFormat& vertexFormat) : VertexGenerator(),
	generatorEffect(effect),
	generatorParticleEmitterId(particleEmitterId), generatorParticleTypeId(particleTypeId),
	generatorVertexFormat(vertexFormat) {
	validateVertexFormat();
}

VertexDataBufferDimensions MeshVertexGenerator::buildGeometry(
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

void MeshVertexGenerator::generateVertexData(const VertexDataBufferCollection& dataBuffers,
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
			case VertexAttributeUsage::position2d:
				generatePosition2d(buffer, attribute, particles, particleCount);
				break;
			case VertexAttributeUsage::position3d:
				generatePosition3d(buffer, attribute, particles, particleCount);
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
			case VertexAttributeUsage::matrix4x4_column_major:
				generateMatrix4x4ColumnMajor(buffer, attribute, particles, particleCount, runtimeContext, sceneContext);
				break;
			case VertexAttributeUsage::matrix4x3_column_major:
				generateMatrix4x3ColumnMajor(buffer, attribute, particles, particleCount, runtimeContext, sceneContext);
				break;
			case VertexAttributeUsage::matrix4x4_row_major:
				generateMatrix4x4RowMajor(buffer, attribute, particles, particleCount, runtimeContext, sceneContext);
				break;
			case VertexAttributeUsage::matrix4x3_row_major:
				generateMatrix4x3RowMajor(buffer, attribute, particles, particleCount, runtimeContext, sceneContext);
				break;
			default:
				break;
		}
	}
}

void MeshVertexGenerator::validateVertexFormat() const {
	std::int32_t attributeIndex = 0;

	for(const VertexAttribute& attribute : generatorVertexFormat.attributes()) {
		if(attribute.dataGenerationMode != VertexDataGenerationMode::instance) {
			throw VertexFormatException("Unsupported data generation mode", attributeIndex);
		}

		switch(attribute.usage) {
			case VertexAttributeUsage::position2d:
			case VertexAttributeUsage::position3d:
			case VertexAttributeUsage::color:
			case VertexAttributeUsage::life:
			case VertexAttributeUsage::velocity2d:
			case VertexAttributeUsage::velocity3d:
			case VertexAttributeUsage::matrix4x4_column_major:
			case VertexAttributeUsage::matrix4x3_column_major:
			case VertexAttributeUsage::matrix4x4_row_major:
			case VertexAttributeUsage::matrix4x3_row_major:
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

void MeshVertexGenerator::generatePosition2d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		*reinterpret_cast<glm::vec2*>(buffer) = glm::vec2(particles.globalPosition[p]);
		buffer += stride;
	}
}
void MeshVertexGenerator::generatePosition3d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		*reinterpret_cast<glm::vec3*>(buffer) = glm::vec3(particles.globalPosition[p]);
		buffer += stride;
	}
}
void MeshVertexGenerator::generateColor(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		*reinterpret_cast<glm::vec4*>(buffer) = glm::vec4(particles.color[p]);
		buffer += stride;
	}
}
void MeshVertexGenerator::generateLife(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		*reinterpret_cast<float*>(buffer) = static_cast<float>(particles.life[p]);
		buffer += stride;
	}
}
void MeshVertexGenerator::generateVelocity2d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		*reinterpret_cast<glm::vec2*>(buffer) = glm::vec2(particles.velocity[p]);
		buffer += stride;
	}
}
void MeshVertexGenerator::generateVelocity3d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		*reinterpret_cast<glm::vec3*>(buffer) = glm::vec3(particles.velocity[p]);
		buffer += stride;
	}
}
void MeshVertexGenerator::generateMatrix4x4ColumnMajor(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	AlignmentMode alignmentMode = particleType.alignmentMode();
	float3_t pivot = particleType.pivot().value();

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	float3_t emitterPosition = emitterTransform.position();
	mat3_t emitterRotation = rotationMatrix3d(emitterTransform.rotation());

	mat4_t globalScaleMatrix = glm::scale(sceneContext.effectScale);

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		glm::mat4 transformMatix = globalScaleMatrix * generateTransformationMatrix(
			alignmentMode, pivot, emitterPosition, emitterRotation,
			particles, p);

		*reinterpret_cast<glm::mat4*>(buffer) = transformMatix;
		buffer += stride;
	}
}
void MeshVertexGenerator::generateMatrix4x3ColumnMajor(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	AlignmentMode alignmentMode = particleType.alignmentMode();
	float3_t pivot = particleType.pivot().value();

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	float3_t emitterPosition = emitterTransform.position();
	mat3_t emitterRotation = rotationMatrix3d(emitterTransform.rotation());

	mat4_t globalScaleMatrix = glm::scale(sceneContext.effectScale);

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		glm::mat4x3 transformMatix = glm::mat4(globalScaleMatrix * generateTransformationMatrix(
			alignmentMode, pivot, emitterPosition, emitterRotation,
			particles, p));

		*reinterpret_cast<glm::mat4x3*>(buffer) = transformMatix;
		buffer += stride;
	}
}
void MeshVertexGenerator::generateMatrix4x4RowMajor(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	AlignmentMode alignmentMode = particleType.alignmentMode();
	float3_t pivot = particleType.pivot().value();

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	float3_t emitterPosition = emitterTransform.position();
	mat3_t emitterRotation = rotationMatrix3d(emitterTransform.rotation());

	mat4_t globalScaleMatrix = glm::scale(sceneContext.effectScale);

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		glm::mat4 transformMatix = glm::rowMajor4(globalScaleMatrix * generateTransformationMatrix(
			alignmentMode, pivot, emitterPosition, emitterRotation,
			particles, p));

		*reinterpret_cast<glm::mat4*>(buffer) = transformMatix;
		buffer += stride;
	}
}
void MeshVertexGenerator::generateMatrix4x3RowMajor(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	AlignmentMode alignmentMode = particleType.alignmentMode();
	float3_t pivot = particleType.pivot().value();

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	float3_t emitterPosition = emitterTransform.position();
	mat3_t emitterRotation = rotationMatrix3d(emitterTransform.rotation());

	mat4_t globalScaleMatrix = glm::scale(sceneContext.effectScale);

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		glm::mat3x4 transformMatix = glm::mat4(glm::rowMajor4(globalScaleMatrix * generateTransformationMatrix(
			alignmentMode, pivot, emitterPosition, emitterRotation,
			particles, p)));

		*reinterpret_cast<glm::mat3x4*>(buffer) = transformMatix;
		buffer += stride;
	}
}

mat4_t MeshVertexGenerator::generateTransformationMatrix(AlignmentMode alignmentMode, const float3_t& pivot,
	const float3_t& emitterPosition, const mat3_t& emitterRotation,
	ParticleCollection::ReadPtr particles, std::uint32_t particleIndex) {
	mat4_t alignmentMatrix = mat4_t(1.0);
	switch(alignmentMode) {
		case AlignmentMode::motion:
			alignmentMatrix = mat4_t(lookAtMatrix3d(particles.velocity[particleIndex]));
			break;
		case AlignmentMode::emission:
			alignmentMatrix = mat4_t(lookAtMatrix3d(emitterPosition - particles.globalPosition[particleIndex]));
			break;
		case AlignmentMode::emitter:
			alignmentMatrix = mat4_t(emitterRotation);
			break;
		default:
			break;
	}

	float3_t particlePivot = pivot * particles.size[particleIndex];
	mat4_t rotationMatrix =
		glm::translate(particlePivot) *
		mat4_t(rotationMatrix3d(particles.rotation[particleIndex])) *
		glm::translate(-particlePivot);

	mat4_t translationMatrix = glm::translate(particles.globalPosition[particleIndex]);
	mat4_t scaleMatrix = glm::scale(particles.size[particleIndex]);

	return translationMatrix * alignmentMatrix * rotationMatrix * scaleMatrix;
}

mat3_t MeshVertexGenerator::rotationMatrix3d(const float3_t& rollYawPitch) {
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
mat3_t MeshVertexGenerator::lookAtMatrix3d(const float3_t& direction) {
	float3_t up = worldUpVector3;
	float3_t forward = glm::normalize(direction);
	float3_t right = glm::normalize(glm::cross(forward, up));
	up = glm::normalize(glm::cross(right, forward));

	return mat3_t(right, up, -forward);
}
}
