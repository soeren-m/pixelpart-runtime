#include "ParticleContainer.h"

namespace pixelpart {
ParticleContainer::ParticleContainer(uint32_t capacity) : particleCapacity(capacity) {
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

uint32_t ParticleContainer::spawn(uint32_t count) {
	count = (numParticles + count > particleCapacity)
		? particleCapacity - numParticles
		: count;

	numParticles += count;

	return count;
}

void ParticleContainer::kill(uint32_t index) {
	numParticles--;
	swap(index, numParticles);
}
void ParticleContainer::killAll() {
	numParticles = 0u;
}

ParticleWritePtr ParticleContainer::getParticleWritePtr(uint32_t start) {
	return ParticleWritePtr{
		id.data() + start,
		parentId.data() + start,
		life.data() + start,
		lifespan.data() + start,
		position.data() + start,
		globalPosition.data() + start,
		velocity.data() + start,
		force.data() + start,
		rotation.data() + start,
		initialRotation.data() + start,
		initialAngularVelocity.data() + start,
		size.data() + start,
		initialSize.data() + start,
		color.data() + start,
		initialColor.data() + start
	};
}
ParticleReadPtr ParticleContainer::getParticleReadPtr(uint32_t start) const {
	return ParticleReadPtr{
		id.data() + start,
		parentId.data() + start,
		life.data() + start,
		lifespan.data() + start,
		position.data() + start,
		globalPosition.data() + start,
		velocity.data() + start,
		force.data() + start,
		rotation.data() + start,
		initialRotation.data() + start,
		initialAngularVelocity.data() + start,
		size.data() + start,
		initialSize.data() + start,
		color.data() + start,
		initialColor.data() + start
	};
}
uint32_t ParticleContainer::getParticleCapacity() const {
	return particleCapacity;
}
uint32_t ParticleContainer::getNumParticles() const {
	return numParticles;
}

void ParticleContainer::swap(std::size_t i, std::size_t j) {
	std::swap(id[i], id[j]);
	std::swap(parentId[i], parentId[j]);
	std::swap(life[i], life[j]);
	std::swap(lifespan[i], lifespan[j]);
	std::swap(position[i], position[j]);
	std::swap(globalPosition[i], globalPosition[j]);
	std::swap(velocity[i], velocity[j]);
	std::swap(force[i], force[j]);
	std::swap(rotation[i], rotation[j]);
	std::swap(initialRotation[i], initialRotation[j]);
	std::swap(initialAngularVelocity[i], initialAngularVelocity[j]);
	std::swap(size[i], size[j]);
	std::swap(initialSize[i], initialSize[j]);
	std::swap(color[i], color[j]);
	std::swap(initialColor[i], initialColor[j]);
}
}