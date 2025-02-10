#include "ParticleCollection.h"
#include <algorithm>

namespace pixelpart {
ParticleCollection::ParticleCollection(uint32_t capacity) : particleCapacity(capacity) {
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

uint32_t ParticleCollection::add(uint32_t number) {
	number = (particleCount + number > particleCapacity)
		? particleCapacity - particleCount
		: number;

	particleCount += number;

	return number;
}
void ParticleCollection::remove(uint32_t index) {
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
	for(uint32_t index = 0u; index < particleCount; ) {
		if(life[index] > 1.0) {
			remove(index);
		}
		else {
			index++;
		}
	}
}
void ParticleCollection::clear() {
	particleCount = 0u;
}

ParticleCollection::WritePtr ParticleCollection::writePtr(uint32_t index) {
	return WritePtr{
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
		initialColor.data() + index
	};
}
ParticleCollection::ReadPtr ParticleCollection::readPtr(uint32_t index) const {
	return ReadPtr{
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
		initialColor.data() + index
	};
}

uint32_t ParticleCollection::count() const {
	return particleCount;
}
uint32_t ParticleCollection::capacity() const {
	return particleCapacity;
}
}