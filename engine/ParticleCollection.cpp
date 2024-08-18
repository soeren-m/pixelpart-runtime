#include "ParticleCollection.h"
#include <algorithm>

namespace pixelpart {
ParticleCollection::ParticleCollection(uint32_t cap) : capacity(cap) {
	id.resize(capacity);
	parentId.resize(capacity);
	life.resize(capacity);
	lifespan.resize(capacity);
	position.resize(capacity);
	globalPosition.resize(capacity);
	velocity.resize(capacity);
	force.resize(capacity);
	rotation.resize(capacity);
	initialRotation.resize(capacity);
	initialAngularVelocity.resize(capacity);
	size.resize(capacity);
	initialSize.resize(capacity);
	color.resize(capacity);
	initialColor.resize(capacity);
}

uint32_t ParticleCollection::add(uint32_t number) {
	number = (count + number > capacity)
		? capacity - count
		: number;

	count += number;

	return number;
}
void ParticleCollection::remove(uint32_t index) {
	count--;

	std::swap(id[index], id[count]);
	std::swap(parentId[index], parentId[count]);
	std::swap(life[index], life[count]);
	std::swap(lifespan[index], lifespan[count]);
	std::swap(position[index], position[count]);
	std::swap(globalPosition[index], globalPosition[count]);
	std::swap(velocity[index], velocity[count]);
	std::swap(force[index], force[count]);
	std::swap(rotation[index], rotation[count]);
	std::swap(initialRotation[index], initialRotation[count]);
	std::swap(initialAngularVelocity[index], initialAngularVelocity[count]);
	std::swap(size[index], size[count]);
	std::swap(initialSize[index], initialSize[count]);
	std::swap(color[index], color[count]);
	std::swap(initialColor[index], initialColor[count]);
}
void ParticleCollection::removeAll() {
	count = 0u;
}

ParticleCollection::WritePtr ParticleCollection::getWritePtr(uint32_t index) {
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
ParticleCollection::ReadPtr ParticleCollection::getReadPtr(uint32_t index) const {
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
uint32_t ParticleCollection::getCapacity() const {
	return capacity;
}
uint32_t ParticleCollection::getCount() const {
	return count;
}
}