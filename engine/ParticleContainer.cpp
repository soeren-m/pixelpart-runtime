#include "ParticleContainer.h"

namespace pixelpart {
ParticleContainer::ParticleContainer() {

}
ParticleContainer::ParticleContainer(uint32_t capacity) {
	reserve(capacity);
}

void ParticleContainer::reserve(uint32_t capacity) {
	particleCapacity = capacity;
	numParticles = 0u;

	data.resize(particleCapacity);
}

uint32_t ParticleContainer::spawn(uint32_t count) {
	count = (numParticles + count > particleCapacity)
		? particleCapacity - numParticles
		: count;

	numParticles += count;

	return count;
}
void ParticleContainer::kill(uint32_t i) {
	numParticles--;
	data.swap(i, numParticles);
}
void ParticleContainer::kill() {
	numParticles = 0u;
}

ParticleData& ParticleContainer::get() {
	return data;
}
const ParticleData& ParticleContainer::get() const {
	return data;
}

uint32_t ParticleContainer::getNumParticles() const {
	return numParticles;
}
}