#include "ParticleCollection.h"
#include <algorithm>

namespace pixelpart {
ParticleCollection::WritePtr::WritePtr(
	std::uint32_t* idPtr,
	std::uint32_t* parentIdPtr,
	float_t* lifePtr,
	float_t* lifespanPtr,
	float3_t* positionPtr,
	float3_t* globalPositionPtr,
	float3_t* velocityPtr,
	float3_t* forcePtr,
	float3_t* rotationPtr,
	float3_t* initialRotationPtr,
	float3_t* initialAngularVelocityPtr,
	float3_t* sizePtr,
	float_t* initialSizePtr,
	float4_t* colorPtr,
	float4_t* initialColorPtr) :
	id(idPtr), parentId(parentIdPtr), life(lifePtr), lifespan(lifespanPtr), position(positionPtr), globalPosition(globalPositionPtr),
	velocity(velocityPtr), force(forcePtr), rotation(rotationPtr), initialRotation(initialRotationPtr), initialAngularVelocity(initialAngularVelocityPtr),
	size(sizePtr), initialSize(initialSizePtr), color(colorPtr), initialColor(initialColorPtr) {

}

ParticleCollection::ReadPtr::ReadPtr(
	const std::uint32_t* idPtr,
	const std::uint32_t* parentIdPtr,
	const float_t* lifePtr,
	const float_t* lifespanPtr,
	const float3_t* positionPtr,
	const float3_t* globalPositionPtr,
	const float3_t* velocityPtr,
	const float3_t* forcePtr,
	const float3_t* rotationPtr,
	const float3_t* initialRotationPtr,
	const float3_t* initialAngularVelocityPtr,
	const float3_t* sizePtr,
	const float_t* initialSizePtr,
	const float4_t* colorPtr,
	const float4_t* initialColorPtr) :
	id(idPtr), parentId(parentIdPtr), life(lifePtr), lifespan(lifespanPtr), position(positionPtr), globalPosition(globalPositionPtr),
	velocity(velocityPtr), force(forcePtr), rotation(rotationPtr), initialRotation(initialRotationPtr), initialAngularVelocity(initialAngularVelocityPtr),
	size(sizePtr), initialSize(initialSizePtr), color(colorPtr), initialColor(initialColorPtr) {

}

ParticleCollection::ParticleCollection(std::uint32_t maxCount) : particleMaxCount(maxCount) {
	resizeBuffers(maxCount > 0 ? maxCount : 1024);
}

std::uint32_t ParticleCollection::add(std::uint32_t number) {
	std::size_t finalParticleCount = particleCount + number;
	if(finalParticleCount > particleId.size()) {
		if(particleMaxCount > 0) {
			number = particleMaxCount - particleCount;
		}
		else {
			resizeBuffers(std::max(finalParticleCount, particleId.size() * 2));
		}
	}

	particleCount += number;

	return number;
}
void ParticleCollection::remove(std::uint32_t index) {
	particleCount--;

	std::swap(particleId[index], particleId[particleCount]);
	std::swap(particleParentId[index], particleParentId[particleCount]);
	std::swap(particleLife[index], particleLife[particleCount]);
	std::swap(particleLifespan[index], particleLifespan[particleCount]);
	std::swap(particlePosition[index], particlePosition[particleCount]);
	std::swap(particleGlobalPosition[index], particleGlobalPosition[particleCount]);
	std::swap(particleVelocity[index], particleVelocity[particleCount]);
	std::swap(particleForce[index], particleForce[particleCount]);
	std::swap(particleRotation[index], particleRotation[particleCount]);
	std::swap(particleInitialRotation[index], particleInitialRotation[particleCount]);
	std::swap(particleInitialAngularVelocity[index], particleInitialAngularVelocity[particleCount]);
	std::swap(particleSize[index], particleSize[particleCount]);
	std::swap(particleInitialSize[index], particleInitialSize[particleCount]);
	std::swap(particleColor[index], particleColor[particleCount]);
	std::swap(particleInitialColor[index], particleInitialColor[particleCount]);
}
void ParticleCollection::removeDead() {
	for(std::uint32_t index = 0; index < particleCount; ) {
		if(particleLife[index] > 1.0) {
			remove(index);
		}
		else {
			index++;
		}
	}
}
void ParticleCollection::clear() {
	particleCount = 0;
}

ParticleCollection::WritePtr ParticleCollection::writePtr(std::uint32_t index) {
	return WritePtr(
		particleId.data() + index,
		particleParentId.data() + index,
		particleLife.data() + index,
		particleLifespan.data() + index,
		particlePosition.data() + index,
		particleGlobalPosition.data() + index,
		particleVelocity.data() + index,
		particleForce.data() + index,
		particleRotation.data() + index,
		particleInitialRotation.data() + index,
		particleInitialAngularVelocity.data() + index,
		particleSize.data() + index,
		particleInitialSize.data() + index,
		particleColor.data() + index,
		particleInitialColor.data() + index);
}
ParticleCollection::ReadPtr ParticleCollection::readPtr(std::uint32_t index) const {
	return ReadPtr(
		particleId.data() + index,
		particleParentId.data() + index,
		particleLife.data() + index,
		particleLifespan.data() + index,
		particlePosition.data() + index,
		particleGlobalPosition.data() + index,
		particleVelocity.data() + index,
		particleForce.data() + index,
		particleRotation.data() + index,
		particleInitialRotation.data() + index,
		particleInitialAngularVelocity.data() + index,
		particleSize.data() + index,
		particleInitialSize.data() + index,
		particleColor.data() + index,
		particleInitialColor.data() + index);
}

std::uint32_t ParticleCollection::count() const {
	return particleCount;
}
std::uint32_t ParticleCollection::capacity() const {
	return static_cast<std::uint32_t>(particleId.size());
}

void ParticleCollection::resizeBuffers(std::size_t size) {
	particleId.resize(size);
	particleParentId.resize(size);
	particleLife.resize(size);
	particleLifespan.resize(size);
	particlePosition.resize(size);
	particleGlobalPosition.resize(size);
	particleVelocity.resize(size);
	particleForce.resize(size);
	particleRotation.resize(size);
	particleInitialRotation.resize(size);
	particleInitialAngularVelocity.resize(size);
	particleSize.resize(size);
	particleInitialSize.resize(size);
	particleColor.resize(size);
	particleInitialColor.resize(size);
}
}
