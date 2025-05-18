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

ParticleCollection::ParticleCollection(std::uint32_t capacity) : particleCapacity(capacity) {
	id.resize(particleCapacity);
	parentId.resize(particleCapacity);
	life.resize(particleCapacity);
	lifespan.resize(particleCapacity);
	position.resize(particleCapacity);
	globalPosition.resize(particleCapacity);
	velocity.resize(particleCapacity);
	force.resize(particleCapacity);
	rotation.resize(particleCapacity);
	initialRotation.resize(particleCapacity);
	initialAngularVelocity.resize(particleCapacity);
	size.resize(particleCapacity);
	initialSize.resize(particleCapacity);
	color.resize(particleCapacity);
	initialColor.resize(particleCapacity);
}

std::uint32_t ParticleCollection::add(std::uint32_t number) {
	number = (particleCount + number > particleCapacity)
		? particleCapacity - particleCount
		: number;

	particleCount += number;

	return number;
}
void ParticleCollection::remove(std::uint32_t index) {
	particleCount--;

	std::swap(id[index], id[particleCount]);
	std::swap(parentId[index], parentId[particleCount]);
	std::swap(life[index], life[particleCount]);
	std::swap(lifespan[index], lifespan[particleCount]);
	std::swap(position[index], position[particleCount]);
	std::swap(globalPosition[index], globalPosition[particleCount]);
	std::swap(velocity[index], velocity[particleCount]);
	std::swap(force[index], force[particleCount]);
	std::swap(rotation[index], rotation[particleCount]);
	std::swap(initialRotation[index], initialRotation[particleCount]);
	std::swap(initialAngularVelocity[index], initialAngularVelocity[particleCount]);
	std::swap(size[index], size[particleCount]);
	std::swap(initialSize[index], initialSize[particleCount]);
	std::swap(color[index], color[particleCount]);
	std::swap(initialColor[index], initialColor[particleCount]);
}
void ParticleCollection::removeDead() {
	for(std::uint32_t index = 0; index < particleCount; ) {
		if(life[index] > 1.0) {
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
		id.data() + index,
		parentId.data() + index,
		life.data() + index,
		lifespan.data() + index,
		position.data() + index,
		globalPosition.data() + index,
		velocity.data() + index,
		force.data() + index,
		rotation.data() + index,
		initialRotation.data() + index,
		initialAngularVelocity.data() + index,
		size.data() + index,
		initialSize.data() + index,
		color.data() + index,
		initialColor.data() + index);
}
ParticleCollection::ReadPtr ParticleCollection::readPtr(std::uint32_t index) const {
	return ReadPtr(
		id.data() + index,
		parentId.data() + index,
		life.data() + index,
		lifespan.data() + index,
		position.data() + index,
		globalPosition.data() + index,
		velocity.data() + index,
		force.data() + index,
		rotation.data() + index,
		initialRotation.data() + index,
		initialAngularVelocity.data() + index,
		size.data() + index,
		initialSize.data() + index,
		color.data() + index,
		initialColor.data() + index);
}

std::uint32_t ParticleCollection::count() const {
	return particleCount;
}
std::uint32_t ParticleCollection::capacity() const {
	return particleCapacity;
}
}