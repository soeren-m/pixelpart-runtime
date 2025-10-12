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

ParticleCollection::ParticleCollection(std::uint32_t capacity) : collectionCapacity(capacity) {
	id.resize(collectionCapacity);
	parentId.resize(collectionCapacity);
	life.resize(collectionCapacity);
	lifespan.resize(collectionCapacity);
	position.resize(collectionCapacity);
	globalPosition.resize(collectionCapacity);
	velocity.resize(collectionCapacity);
	force.resize(collectionCapacity);
	rotation.resize(collectionCapacity);
	initialRotation.resize(collectionCapacity);
	initialAngularVelocity.resize(collectionCapacity);
	size.resize(collectionCapacity);
	initialSize.resize(collectionCapacity);
	color.resize(collectionCapacity);
	initialColor.resize(collectionCapacity);
}

std::uint32_t ParticleCollection::add(std::uint32_t number) {
	number = (collectionCount + number > collectionCapacity)
		? collectionCapacity - collectionCount
		: number;

	collectionCount += number;

	return number;
}
void ParticleCollection::remove(std::uint32_t index) {
	collectionCount--;

	std::swap(id[index], id[collectionCount]);
	std::swap(parentId[index], parentId[collectionCount]);
	std::swap(life[index], life[collectionCount]);
	std::swap(lifespan[index], lifespan[collectionCount]);
	std::swap(position[index], position[collectionCount]);
	std::swap(globalPosition[index], globalPosition[collectionCount]);
	std::swap(velocity[index], velocity[collectionCount]);
	std::swap(force[index], force[collectionCount]);
	std::swap(rotation[index], rotation[collectionCount]);
	std::swap(initialRotation[index], initialRotation[collectionCount]);
	std::swap(initialAngularVelocity[index], initialAngularVelocity[collectionCount]);
	std::swap(size[index], size[collectionCount]);
	std::swap(initialSize[index], initialSize[collectionCount]);
	std::swap(color[index], color[collectionCount]);
	std::swap(initialColor[index], initialColor[collectionCount]);
}
void ParticleCollection::removeDead() {
	for(std::uint32_t index = 0; index < collectionCount; ) {
		if(life[index] > 1.0) {
			remove(index);
		}
		else {
			index++;
		}
	}
}
void ParticleCollection::clear() {
	collectionCount = 0;
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
	return collectionCount;
}
std::uint32_t ParticleCollection::capacity() const {
	return collectionCapacity;
}
}
