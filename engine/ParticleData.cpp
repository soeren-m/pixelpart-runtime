#include "ParticleData.h"

namespace pixelpart {
void ParticleData::resize(std::size_t n) {
	id.resize(n);
	parentId.resize(n);
	life.resize(n);
	lifespan.resize(n);
	position.resize(n);
	globalPosition.resize(n);
	velocity.resize(n);
	force.resize(n);
	rotation.resize(n);
	initialRotation.resize(n);
	initialAngularVelocity.resize(n);
	size.resize(n);
	initialSize.resize(n);
	color.resize(n);
	initialColor.resize(n);
}
void ParticleData::swap(std::size_t i, std::size_t j) {
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