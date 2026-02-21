#include "MeshVertexGenerator.h"
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
#include <cmath>
#include <algorithm>

namespace pixelpart {
MeshVertexGenerator::MeshVertexGenerator(const Effect& effect, id_t particleEmitterId, id_t particleTypeId,
	const VertexFormat& vertexFormat,
	std::shared_ptr<ThreadPool> threadPool) : VertexGenerator(),
	generatorEffect(effect),
	generatorParticleEmitterId(particleEmitterId), generatorParticleTypeId(particleTypeId),
	generatorVertexFormat(vertexFormat),
	generatorThreadPool(threadPool) {
	validateVertexFormat();
}

VertexDataBufferDimensions MeshVertexGenerator::buildGeometry(
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
			case VertexAttributeUsage::position2d:
				generatePosition2d(buffer, attribute, particles, particleCount);
				break;
			case VertexAttributeUsage::position3d:
				generatePosition3d(buffer, attribute, particles, particleCount);
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

void MeshVertexGenerator::generatePosition2d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		*reinterpret_cast<math::vector2<float>*>(buffer) = math::vector2<float>(particles.globalPosition[p]);
		buffer += stride;
	}
}
void MeshVertexGenerator::generatePosition3d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		*reinterpret_cast<math::vector3<float>*>(buffer) = math::vector3<float>(particles.globalPosition[p]);
		buffer += stride;
	}
}
void MeshVertexGenerator::generateColorFloat(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		*reinterpret_cast<math::vector4<float>*>(buffer) = math::vector4<float>(particles.color[p]);
		buffer += stride;
	}
}
void MeshVertexGenerator::generateColorByte(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		ColorUByte color(particles.color[p]);

		*(buffer + 0) = color.r;
		*(buffer + 1) = color.g;
		*(buffer + 2) = color.b;
		*(buffer + 3) = color.a;
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
		*reinterpret_cast<math::vector2<float>*>(buffer) = math::vector2<float>(particles.velocity[p]);
		buffer += stride;
	}
}
void MeshVertexGenerator::generateVelocity3d(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount) const {
	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		*reinterpret_cast<math::vector3<float>*>(buffer) = math::vector3<float>(particles.velocity[p]);
		buffer += stride;
	}
}
void MeshVertexGenerator::generateMatrix4x4ColumnMajor(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	AlignmentMode alignmentMode = particleType.alignmentMode();
	float3_t pivot = particleType.pivot().value();

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	float3_t emitterPosition = emitterTransform.position();
	matrix3_t emitterRotation = rotationMatrix3d(emitterTransform.rotation());

	matrix4_t globalScaleMatrix = math::scalingMatrix(sceneContext.effectScale);

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		math::matrix4x4<float> transformMatix(globalScaleMatrix * particleTransformationMatrix(
			alignmentMode, pivot, emitterPosition, emitterRotation,
			particles, p));

		*reinterpret_cast<math::matrix4x4<float>*>(buffer) = transformMatix;
		buffer += stride;
	}
}
void MeshVertexGenerator::generateMatrix4x3ColumnMajor(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	AlignmentMode alignmentMode = particleType.alignmentMode();
	float3_t pivot = particleType.pivot().value();

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	float3_t emitterPosition = emitterTransform.position();
	matrix3_t emitterRotation = rotationMatrix3d(emitterTransform.rotation());

	matrix4_t globalScaleMatrix = math::scalingMatrix(sceneContext.effectScale);

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	std::size_t vectorStride = sizeof(math::vector3<float>);

	for(std::uint32_t p = 0; p < particleCount; p++) {
		math::matrix4x4<float> transformMatix(globalScaleMatrix * particleTransformationMatrix(
			alignmentMode, pivot, emitterPosition, emitterRotation,
			particles, p));

		*reinterpret_cast<math::vector3<float>*>(buffer + vectorStride * 0) = math::vector3<float>(transformMatix[0]);
		*reinterpret_cast<math::vector3<float>*>(buffer + vectorStride * 1) = math::vector3<float>(transformMatix[1]);
		*reinterpret_cast<math::vector3<float>*>(buffer + vectorStride * 2) = math::vector3<float>(transformMatix[2]);
		*reinterpret_cast<math::vector3<float>*>(buffer + vectorStride * 3) = math::vector3<float>(transformMatix[3]);
		buffer += stride;
	}
}
void MeshVertexGenerator::generateMatrix4x4RowMajor(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	AlignmentMode alignmentMode = particleType.alignmentMode();
	float3_t pivot = particleType.pivot().value();

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	float3_t emitterPosition = emitterTransform.position();
	matrix3_t emitterRotation = rotationMatrix3d(emitterTransform.rotation());

	matrix4_t globalScaleMatrix = math::scalingMatrix(sceneContext.effectScale);

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	for(std::uint32_t p = 0; p < particleCount; p++) {
		math::matrix4x4<float> transformMatix(math::transpose(globalScaleMatrix * particleTransformationMatrix(
			alignmentMode, pivot, emitterPosition, emitterRotation,
			particles, p)));

		*reinterpret_cast<math::matrix4x4<float>*>(buffer) = transformMatix;
		buffer += stride;
	}
}
void MeshVertexGenerator::generateMatrix4x3RowMajor(std::uint8_t* buffer, const VertexAttribute& attribute,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	AlignmentMode alignmentMode = particleType.alignmentMode();
	float3_t pivot = particleType.pivot().value();

	Transform emitterTransform = generatorEffect.sceneGraph().globalTransform(generatorParticleEmitterId, runtimeContext);
	float3_t emitterPosition = emitterTransform.position();
	matrix3_t emitterRotation = rotationMatrix3d(emitterTransform.rotation());

	matrix4_t globalScaleMatrix = math::scalingMatrix(sceneContext.effectScale);

	std::size_t stride = attribute.byteStride;
	buffer += attribute.byteOffset;

	std::size_t vectorStride = sizeof(math::vector4<float>);

	for(std::uint32_t p = 0; p < particleCount; p++) {
		math::matrix4x4<float> transformMatix(math::transpose(globalScaleMatrix * particleTransformationMatrix(
			alignmentMode, pivot, emitterPosition, emitterRotation,
			particles, p)));

		*reinterpret_cast<math::vector4<float>*>(buffer + vectorStride * 0) = transformMatix[0];
		*reinterpret_cast<math::vector4<float>*>(buffer + vectorStride * 1) = transformMatix[1];
		*reinterpret_cast<math::vector4<float>*>(buffer + vectorStride * 2) = transformMatix[2];
		buffer += stride;
	}
}

matrix4_t MeshVertexGenerator::particleTransformationMatrix(AlignmentMode alignmentMode, const float3_t& pivot,
	const float3_t& emitterPosition, const matrix3_t& emitterRotation,
	ParticleCollection::ReadPtr particles, std::uint32_t particleIndex) {
	matrix4_t alignmentMatrix = matrix4_t(1.0);
	switch(alignmentMode) {
		case AlignmentMode::motion:
			alignmentMatrix = matrix4_t(math::transpose(math::lookAtMatrix(particles.velocity[particleIndex], worldUpVector3)));
			break;
		case AlignmentMode::emission:
			alignmentMatrix = matrix4_t(math::transpose(math::lookAtMatrix(emitterPosition - particles.globalPosition[particleIndex], worldUpVector3)));
			break;
		case AlignmentMode::emitter:
			alignmentMatrix = matrix4_t(emitterRotation);
			break;
		default:
			break;
	}

	float3_t particlePivot = pivot * particles.size[particleIndex];
	matrix4_t rotationMatrix =
		math::translationMatrix(particlePivot) *
		matrix4_t(rotationMatrix3d(particles.rotation[particleIndex])) *
		math::translationMatrix(-particlePivot);

	matrix4_t translationMatrix = math::translationMatrix(particles.globalPosition[particleIndex]);
	matrix4_t scalingMatrix = math::scalingMatrix(particles.size[particleIndex]);

	return translationMatrix * alignmentMatrix * rotationMatrix * scalingMatrix;
}

matrix3_t MeshVertexGenerator::rotationMatrix3d(const float3_t& rollYawPitch) {
	float3_t rollYawPitchRad = math::radians(rollYawPitch);

	return matrix3_t(math::yawPitchRollRotationMatrix(rollYawPitchRad.y, rollYawPitchRad.z, rollYawPitchRad.x));
}
}
