#include "ParticleGenerator.h"
#include "../common/Constants.h"
#include "../glm/glm/gtx/euler_angles.hpp"
#include "../glm/glm/gtx/rotate_vector.hpp"

namespace pixelpart {
const float_t pi = 3.14159265358979323846;

ParticleGenerator::ParticleGenerator(const Effect& fx, std::vector<ParticleContainer>& state) : effect(fx), particleState(state) {

}

uint32_t ParticleGenerator::generate(uint32_t count, uint32_t parentParticle, uint32_t particleTypeIndex, uint32_t parentParticleTypeIndex, uint32_t particleEmitterIndex, float_t dt, float_t t) {
	ParticleContainer& particleContainer = particleState[particleTypeIndex];
	count = particleContainer.spawn(count);

	const ParticleType& particleType = effect.particleTypes.getByIndex(particleTypeIndex);
	const ParticleEmitter& particleEmitter = effect.particleEmitters.getByIndex(particleEmitterIndex);
	ParticleWritePtr particles = particleState[particleTypeIndex].getParticleWritePtr();
	ParticleReadPtr parentParticles = parentParticleTypeIndex != nullId ? particleState[parentParticleTypeIndex].getParticleReadPtr() : ParticleReadPtr();
	float_t alpha = t / particleEmitter.lifetimeDuration;

	for(uint32_t i = 0u; i < count; i++) {
		uint32_t p = particleContainer.getNumParticles() - count + i;

		particles.id[p] = nextParticleId++;
		particles.parentId[p] = parentParticle != nullId ? parentParticles.id[parentParticle] : nullId;

		particles.life[p] = 0.0;
		particles.lifespan[p] = std::max(particleType.lifespan.get(alpha) + random::uniform(rng, -particleType.lifespanVariance.get(), +particleType.lifespanVariance.get()), 0.000001);

		vec3_t particleEmitterPosition = particleEmitter.position.get(alpha);
		vec3_t particleEmitterVelocity = (particleEmitterPosition - particleEmitter.position.get(alpha - dt / particleEmitter.lifetimeDuration)) / dt;

		vec3_t particleSpawnPosition = vec3_t(0.0);
		switch(particleEmitter.shape) {
			case ParticleEmitter::Shape::line:
				particleSpawnPosition = emitOnSegment(
					particleEmitter.size.get(alpha).x,
					particleEmitter.distribution,
					particleEmitter.gridOrder,
					particleEmitter.gridSize[0],
					particleEmitterGridIndices[particleEmitterIndex]);
				break;
			case ParticleEmitter::Shape::ellipse:
				particleSpawnPosition = emitInEllipse(
					particleEmitter.size.get(alpha),
					particleEmitter.distribution,
					particleEmitter.gridOrder,
					particleEmitter.gridSize[0], particleEmitter.gridSize[1],
					particleEmitterGridIndices[particleEmitterIndex]);
				break;
			case ParticleEmitter::Shape::rectangle:
				particleSpawnPosition = emitInRectangle(
					particleEmitter.size.get(alpha),
					particleEmitter.distribution,
					particleEmitter.gridOrder,
					particleEmitter.gridSize[0], particleEmitter.gridSize[1],
					particleEmitterGridIndices[particleEmitterIndex]);
				break;
			case ParticleEmitter::Shape::path:
				particleSpawnPosition = emitOnPath(
					particleEmitter.path,
					particleEmitter.distribution,
					particleEmitter.gridOrder,
					particleEmitter.gridSize[0],
					particleEmitterGridIndices[particleEmitterIndex]);
				break;
			case ParticleEmitter::Shape::ellipsoid:
				particleSpawnPosition = emitInEllipsoid(
					particleEmitter.size.get(alpha),
					particleEmitter.distribution,
					particleEmitter.gridOrder,
					particleEmitter.gridSize[0], particleEmitter.gridSize[1], particleEmitter.gridSize[2],
					particleEmitterGridIndices[particleEmitterIndex]);
				break;
			case ParticleEmitter::Shape::cuboid:
				particleSpawnPosition = emitInCuboid(
					particleEmitter.size.get(alpha),
					particleEmitter.distribution,
					particleEmitter.gridOrder,
					particleEmitter.gridSize[0], particleEmitter.gridSize[1], particleEmitter.gridSize[2],
					particleEmitterGridIndices[particleEmitterIndex]);
				break;
			case ParticleEmitter::Shape::cylinder:
				particleSpawnPosition = emitInCylinder(
					particleEmitter.size.get(alpha),
					particleEmitter.distribution,
					particleEmitter.gridOrder,
					particleEmitter.gridSize[0], particleEmitter.gridSize[1], particleEmitter.gridSize[2],
					particleEmitterGridIndices[particleEmitterIndex]);
				break;
			default:
				break;
		}

		particleSpawnPosition = effect.is3d
			? rotate3d(particleSpawnPosition, particleEmitter.orientation.get(alpha))
			: rotate2d(particleSpawnPosition, particleEmitter.orientation.get(alpha).x);

		vec3_t particleSpawnCenter = parentParticle != nullId
			? (parentParticles.globalPosition[parentParticle] + particleEmitterPosition)
			: (particleType.positionRelative ? vec3_t(0.0) : particleEmitterPosition);

		particles.position[p] = particleSpawnCenter + particleSpawnPosition;
		particles.globalPosition[p] = particleType.positionRelative
			? particles.position[p] + particleEmitterPosition
			: particles.position[p];

		vec3_t particleParentVelocity = parentParticle != nullId
			? parentParticles.velocity[parentParticle]
			: particleEmitterVelocity;

		if(effect.is3d) {
			mat3_t directionMatrix = mat3_t(glm::yawPitchRoll(
				glm::radians(particleEmitter.direction.get(alpha).y + particleEmitter.spread.get(alpha) * random::uniform(rng, -0.5, +0.5)),
				glm::radians(particleEmitter.direction.get(alpha).z + particleEmitter.spread.get(alpha) * random::uniform(rng, -0.5, +0.5)),
				glm::radians(particleEmitter.direction.get(alpha).x + particleEmitter.spread.get(alpha) * random::uniform(rng, -0.5, +0.5))));

			switch(particleEmitter.directionMode) {
				case ParticleEmitter::DirectionMode::outwards:
					particles.velocity[p] = directionMatrix *
						((particleSpawnPosition != vec3_t(0.0)) ? glm::normalize(+particleSpawnPosition) : worldUpVector3);
					break;
				case ParticleEmitter::DirectionMode::inwards:
					particles.velocity[p] = directionMatrix *
						((particleSpawnPosition != vec3_t(0.0)) ? glm::normalize(-particleSpawnPosition) : worldUpVector3);
					break;
				case ParticleEmitter::DirectionMode::inherit:
					particles.velocity[p] = directionMatrix * glm::normalize(particleParentVelocity != vec3_t(0.0) ? +particleParentVelocity : worldUpVector3);
					break;
				case ParticleEmitter::DirectionMode::inherit_inverse:
					particles.velocity[p] = directionMatrix * glm::normalize(particleParentVelocity != vec3_t(0.0) ? -particleParentVelocity : worldUpVector3);
					break;
				default:
					particles.velocity[p] = directionMatrix * worldUpVector3;
					break;
			}
		}
		else {
			float_t direction = glm::radians(particleEmitter.direction.get(alpha).x + particleEmitter.spread.get(alpha) * random::uniform(rng,-0.5, +0.5));

			switch(particleEmitter.directionMode) {
				case ParticleEmitter::DirectionMode::outwards:
					particles.velocity[p] = vec3_t(glm::rotate(
						(particleSpawnPosition != vec3_t(0.0)) ? vec2_t(glm::normalize(+particleSpawnPosition)) : worldUpVector2,
						direction), 0.0);
					break;
				case ParticleEmitter::DirectionMode::inwards:
					particles.velocity[p] = vec3_t(glm::rotate(
						(particleSpawnPosition != vec3_t(0.0)) ? vec2_t(glm::normalize(-particleSpawnPosition)) : worldUpVector2,
						direction), 0.0);
					break;
				case ParticleEmitter::DirectionMode::inherit:
					particles.velocity[p] = vec3_t(glm::rotate(vec2_t(glm::normalize(particleParentVelocity != vec3_t(0.0) ? +particleParentVelocity : worldUpVector3)), direction), 0.0);
					break;
				case ParticleEmitter::DirectionMode::inherit_inverse:
					particles.velocity[p] = vec3_t(glm::rotate(vec2_t(glm::normalize(particleParentVelocity != vec3_t(0.0) ? -particleParentVelocity : worldUpVector3)), direction), 0.0);
					break;
				default:
					particles.velocity[p] = vec3_t(glm::rotate(worldUpVector2, direction), 0.0);
					break;
			}
		}

		particles.velocity[p] *= glm::mix(
			particleType.initialVelocity.get(alpha),
			glm::length(particleParentVelocity),
			particleType.inheritedVelocity.get(alpha)) + random::uniform(rng, -particleType.velocityVariance.get(), +particleType.velocityVariance.get());
		particles.force[p] = vec3_t(0.0);

		particles.initialRotation[p] = particleType.initialRotation.get(alpha) + vec3_t(
			random::uniform(rng, -particleType.rotationVariance.get().x, +particleType.rotationVariance.get().x),
			random::uniform(rng, -particleType.rotationVariance.get().y, +particleType.rotationVariance.get().y),
			random::uniform(rng, -particleType.rotationVariance.get().z, +particleType.rotationVariance.get().z));
		particles.initialAngularVelocity[p] = vec3_t(
			random::uniform(rng, -particleType.angularVelocityVariance.get().x, +particleType.angularVelocityVariance.get().x),
			random::uniform(rng, -particleType.angularVelocityVariance.get().y, +particleType.angularVelocityVariance.get().y),
			random::uniform(rng, -particleType.angularVelocityVariance.get().z, +particleType.angularVelocityVariance.get().z));
		particles.rotation[p] = particles.initialRotation[p];

		particles.initialSize[p] = particleType.initialSize.get(alpha) + random::uniform(rng, -particleType.sizeVariance.get(), +particleType.sizeVariance.get());
		particles.size[p] = particleType.size.get() * particles.initialSize[p];

		particles.initialColor[p] = vec4_t(
			random::uniform(rng, -particleType.colorVariance.get().x, +particleType.colorVariance.get().x),
			random::uniform(rng, -particleType.colorVariance.get().y, +particleType.colorVariance.get().y),
			random::uniform(rng, -particleType.colorVariance.get().z, +particleType.colorVariance.get().z),
			particleType.initialOpacity.get(alpha) + random::uniform(rng, -particleType.opacityVariance.get(), +particleType.opacityVariance.get()));
		particles.color[p] = vec4_t(vec3_t(particleType.color.get()), particleType.opacity.get());
	}

	return count;
}

void ParticleGenerator::prepare() {
	if(particleEmitterGridIndices.size() != effect.particleEmitters.getCount()) {
		particleEmitterGridIndices.assign(effect.particleEmitters.getCount(), 0u);
	}
}
void ParticleGenerator::reset() {
	nextParticleId = 0u;
	particleEmitterGridIndices.assign(effect.particleEmitters.getCount(), 0u);
}

void ParticleGenerator::applySeed(uint32_t seed) {
	rng = std::mt19937(seed);
}

float_t ParticleGenerator::sampleGrid1D(uint32_t gridIndex, uint32_t gridSize, float_t min, float_t max) {
	return static_cast<float_t>(gridIndex % gridSize) / static_cast<float_t>(gridSize - 1u) * (max - min) + min;
}
float_t ParticleGenerator::sampleGrid2D(uint32_t gridIndex, uint32_t gridSize1, uint32_t gridSize2, float_t min, float_t max) {
	return static_cast<float_t>(gridIndex / gridSize1 % gridSize2) / static_cast<float_t>(gridSize2 - 1u) * (max - min) + min;
}
float_t ParticleGenerator::sampleGrid3D(uint32_t gridIndex, uint32_t gridSize1, uint32_t gridSize2, uint32_t gridSize3, float_t min, float_t max) {
	return static_cast<float_t>(gridIndex / gridSize1 / gridSize2 % gridSize3) / static_cast<float_t>(gridSize3 - 1u) * (max - min) + min;
}

vec3_t ParticleGenerator::rotate2d(const vec3_t& v, float_t a) {
	return vec3_t(glm::rotate(vec2_t(v), glm::radians(a)), 0.0);
}
vec3_t ParticleGenerator::rotate3d(const vec3_t& v, const vec3_t& a) {
	return vec3_t(glm::yawPitchRoll(glm::radians(a.y), glm::radians(a.z), glm::radians(a.x)) * vec4_t(v, 0.0));
}

vec3_t ParticleGenerator::emitOnSegment(float_t length,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSize, uint32_t& gridIndex) {
	switch(distribution) {
		case ParticleEmitter::Distribution::uniform:
		case ParticleEmitter::Distribution::boundary: {
			return vec3_t(0.0, random::uniform(rng, -length, +length) * 0.5, 0.0);
		}
		case ParticleEmitter::Distribution::center: {
			return vec3_t(0.0, random::normal(rng, -length, +length) * 0.5, 0.0);
		}
		case ParticleEmitter::Distribution::hole: {
			return vec3_t(0.0, random::normalReverse(rng, -length, +length) * 0.5, 0.0);
		}
		case ParticleEmitter::Distribution::grid_random: {
			return vec3_t(0.0, random::uniformGrid(rng, gridSize, -length, +length) * 0.5, 0.0);
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			float_t y = sampleGrid1D(gridIndex, gridSize, -length, +length) * 0.5;
			gridIndex = (gridIndex + 1u) % gridSize;
			return vec3_t(0.0, y, 0.0);
		}
		default: {
			return vec3_t(0.0, 0.0, 0.0);
		}
	}
}

vec3_t ParticleGenerator::emitInEllipse(const vec2_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t& gridIndex) {
	float_t r = 0.0;
	float_t phi = 0.0;
	vec3_t point = vec3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			r = std::sqrt(random::uniform(rng, 0.0, 1.0));
			phi = random::uniform(rng, 0.0, 2.0 * pi);
			point = vec3_t(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * pi);
				point = vec3_t(
					std::cos(phi),
					std::sin(phi),
					0.0) * r;
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 2) < point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * pi);
				point = vec3_t(
					std::cos(phi),
					std::sin(phi),
					0.0) * r;
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 2) > point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			phi = random::uniform(rng, 0.0, 2.0 * pi);
			point = vec3_t(
				std::cos(phi),
				std::sin(phi),
				0.0);

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			r = random::uniformGrid(rng, gridSizeX);
			phi = random::uniformGrid(rng, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
			point = vec3_t(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
				case ParticleEmitter::GridOrder::x_z_y:
				case ParticleEmitter::GridOrder::z_x_y:
					r = sampleGrid1D(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2D(gridIndex, gridSizeX, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					break;
				case ParticleEmitter::GridOrder::y_x_z:
				case ParticleEmitter::GridOrder::y_z_x:
				case ParticleEmitter::GridOrder::z_y_x:
					r = sampleGrid2D(gridIndex, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1D(gridIndex, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1u) % (gridSizeX * gridSizeY);
			point = vec3_t(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

			break;
		}
		default: {
			break;
		}
	}

	point.x *= size.x * 0.5;
	point.y *= size.y * 0.5;

	return point;
}

vec3_t ParticleGenerator::emitInRectangle(const vec2_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t& gridIndex) {
	vec3_t point = vec3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			point = vec3_t(
				random::uniform(rng, -size.x, +size.x),
				random::uniform(rng, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			point = vec3_t(
				random::normal(rng, -size.x, +size.x),
				random::normal(rng, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				point = vec3_t(
					random::uniform(rng, -1.0, +1.0),
					random::uniform(rng, -1.0, +1.0),
					0.0);
			}
			while(random::uniform(rng, 0.0, 1.0) > (point.x * point.x + point.y * point.y) * 0.5);

			point.x *= size.x * 0.5;
			point.y *= size.y * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			float_t r = random::uniform(rng, 0.0, (size.x + size.y) * 2.0);
			point = vec3_t(-size * 0.5, 0.0);

			if(r < size.y) {
				point += vec3_t(0.0, r, 0.0);
			}
			else if(r < size.y + size.x) {
				point += vec3_t(r - size.y, size.y, 0.0);
			}
			else if(r < (size.y + size.x) + size.y) {
				point += vec3_t(size.x, r - (size.y + size.x), 0.0);
			}
			else {
				point += vec3_t(r - (size.y + size.x + size.y), 0.0, 0.0);
			}

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			point = vec3_t(
				random::uniformGrid(rng, gridSizeX, -size.x, +size.x),
				random::uniformGrid(rng, gridSizeY, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
				case ParticleEmitter::GridOrder::x_z_y:
				case ParticleEmitter::GridOrder::z_x_y:
					point = vec3_t(
						sampleGrid1D(gridIndex, gridSizeX, -size.x, +size.x),
						sampleGrid2D(gridIndex, gridSizeX, gridSizeY, -size.y, +size.y),
						0.0) * 0.5;
					break;
				case ParticleEmitter::GridOrder::y_x_z:
				case ParticleEmitter::GridOrder::y_z_x:
				case ParticleEmitter::GridOrder::z_y_x:
					point = vec3_t(
						sampleGrid2D(gridIndex, gridSizeY, gridSizeX, -size.x, +size.x),
						sampleGrid1D(gridIndex, gridSizeY, -size.y, +size.y),
						0.0) * 0.5;
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1u) % (gridSizeX * gridSizeY);

			break;
		}
		default: {
			break;
		}
	}

	return point;
}

vec3_t ParticleGenerator::emitOnPath(const Curve<vec3_t>& path,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSize, uint32_t& gridIndex) {
	float_t x = 0.0;

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform:
		case ParticleEmitter::Distribution::boundary: {
			x = random::uniform(rng, 0.0, 1.0);
			break;
		}
		case ParticleEmitter::Distribution::center: {
			x = random::normal(rng, 0.0, 1.0);
			break;
		}
		case ParticleEmitter::Distribution::hole: {
			x = random::normalReverse(rng, 0.0, 1.0);
			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			x = random::uniformGrid(rng, gridSize);
			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			x = sampleGrid1D(gridIndex, gridSize, 0.0, 1.0);
			gridIndex = (gridIndex + 1u) % gridSize;
			break;
		}
		default: {
			break;
		}
	}

	return path.get(x);
}

vec3_t ParticleGenerator::emitInEllipsoid(const vec3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex) {
	float_t r = 0.0;
	float_t phi = 0.0;
	float_t theta = 0.0;
	vec3_t point = vec3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			r = std::sqrt(random::uniform(rng, 0.0, 1.0));
			phi = random::uniform(rng, 0.0, 2.0 * pi);
			theta = std::acos(random::uniform(rng, -1.0, +1.0));
			point = vec3_t(
				std::sin(theta) * std::cos(phi),
				std::sin(theta) * std::sin(phi),
				std::cos(theta)) * r;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * pi);
				theta = std::acos(random::uniform(rng, -1.0, +1.0));
				point = vec3_t(
					std::sin(theta) * std::cos(phi),
					std::sin(theta) * std::sin(phi),
					std::cos(theta)) * r;
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 3) < point.x * point.x + point.y * point.y + point.z * point.z);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * pi);
				theta = std::acos(random::uniform(rng, -1.0, +1.0));
				point = vec3_t(
					std::sin(theta) * std::cos(phi),
					std::sin(theta) * std::sin(phi),
					std::cos(theta)) * r;
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 3) > point.x * point.x + point.y * point.y + point.z * point.z);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			phi = random::uniform(rng, 0.0, 2.0 * pi);
			theta = std::acos(random::uniform(rng, -1.0, +1.0));
			point = vec3_t(
				std::sin(theta) * std::cos(phi),
				std::sin(theta) * std::sin(phi),
				std::cos(theta));

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			r = random::uniformGrid(rng, gridSizeX);
			phi = random::uniformGrid(rng, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
			theta = random::uniformGrid(rng, gridSizeZ, 0.0, pi);
			point = vec3_t(
				std::sin(theta) * std::cos(phi),
				std::sin(theta) * std::sin(phi),
				std::cos(theta)) * r;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
					r = sampleGrid1D(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2D(gridIndex, gridSizeX, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid3D(gridIndex, gridSizeX, gridSizeY, gridSizeZ, 0.0, pi);
					break;
				case ParticleEmitter::GridOrder::x_z_y:
					r = sampleGrid1D(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3D(gridIndex, gridSizeX, gridSizeZ, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid2D(gridIndex, gridSizeX, gridSizeZ, 0.0, pi);
					break;
				case ParticleEmitter::GridOrder::y_x_z:
					r = sampleGrid2D(gridIndex, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1D(gridIndex, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid3D(gridIndex, gridSizeY, gridSizeX, gridSizeZ, 0.0, pi);
					break;
				case ParticleEmitter::GridOrder::y_z_x:
					r = sampleGrid3D(gridIndex, gridSizeY, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1D(gridIndex, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid2D(gridIndex, gridSizeY, gridSizeZ, 0.0, pi);
					break;
				case ParticleEmitter::GridOrder::z_x_y:
					r = sampleGrid2D(gridIndex, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3D(gridIndex, gridSizeZ, gridSizeX, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid1D(gridIndex, gridSizeZ, 0.0, pi);
					break;
				case ParticleEmitter::GridOrder::z_y_x:
					r = sampleGrid3D(gridIndex, gridSizeZ, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2D(gridIndex, gridSizeZ, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid1D(gridIndex, gridSizeZ, 0.0, pi);
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1u) % (gridSizeX * gridSizeY * gridSizeZ);
			point = vec3_t(
				std::sin(theta) * std::cos(phi),
				std::sin(theta) * std::sin(phi),
				std::cos(theta)) * r;

			break;
		}
		default: {
			break;
		}
	}

	point *= size * 0.5;

	return point;
}

vec3_t ParticleGenerator::emitInCuboid(const vec3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex) {
	vec3_t point = vec3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			point = vec3_t(
				random::uniform(rng, -size.x, +size.x),
				random::uniform(rng, -size.y, +size.y),
				random::uniform(rng, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			point = vec3_t(
				random::normal(rng, -size.x, +size.x),
				random::normal(rng, -size.y, +size.y),
				random::normal(rng, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				point = vec3_t(
					random::uniform(rng, -1.0, +1.0),
					random::uniform(rng, -1.0, +1.0),
					random::uniform(rng, -1.0, +1.0));
			}
			while(random::uniform(rng, 0.0, 1.0) > (point.x * point.x + point.y * point.y + point.z * point.z) * 0.5);

			point *= size * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			int32_t side = random::uniformInt(rng, 0, 5);
			switch(side) {
				case 0:
					point = vec3_t(-size.x, random::uniform(rng, -size.y, +size.y), random::uniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 1:
					point = vec3_t(+size.x, random::uniform(rng, -size.y, +size.y), random::uniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 2:
					point = vec3_t(random::uniform(rng, -size.x, +size.x), -size.y, random::uniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 3:
					point = vec3_t(random::uniform(rng, -size.x, +size.x), +size.y, random::uniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 4:
					point = vec3_t(random::uniform(rng, -size.x, +size.x), random::uniform(rng, -size.y, +size.y), -size.z) * 0.5;
					break;
				case 5:
					point = vec3_t(random::uniform(rng, -size.x, +size.x), random::uniform(rng, -size.y, +size.y), +size.z) * 0.5;
					break;
				default:
					point = vec3_t(0.0);
					break;
			}

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			point = vec3_t(
				random::uniformGrid(rng, gridSizeX, -size.x, +size.x),
				random::uniformGrid(rng, gridSizeY, -size.y, +size.y),
				random::uniformGrid(rng, gridSizeZ, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
					point = vec3_t(
						sampleGrid1D(gridIndex, gridSizeX, -size.x, +size.x),
						sampleGrid2D(gridIndex, gridSizeX, gridSizeY, -size.y, +size.y),
						sampleGrid3D(gridIndex, gridSizeX, gridSizeY, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::x_z_y:
					point = vec3_t(
						sampleGrid1D(gridIndex, gridSizeX, -size.x, +size.x),
						sampleGrid3D(gridIndex, gridSizeX, gridSizeZ, gridSizeY, -size.y, +size.y),
						sampleGrid2D(gridIndex, gridSizeX, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::y_x_z:
					point = vec3_t(
						sampleGrid2D(gridIndex, gridSizeY, gridSizeX, -size.x, +size.x),
						sampleGrid1D(gridIndex, gridSizeY, -size.y, +size.y),
						sampleGrid3D(gridIndex, gridSizeY, gridSizeX, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::y_z_x:
					point = vec3_t(
						sampleGrid3D(gridIndex, gridSizeY, gridSizeZ, gridSizeX, -size.x, +size.x),
						sampleGrid1D(gridIndex, gridSizeY, -size.y, +size.y),
						sampleGrid2D(gridIndex, gridSizeY, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::z_x_y:
					point = vec3_t(
						sampleGrid2D(gridIndex, gridSizeZ, gridSizeX, -size.x, +size.y),
						sampleGrid3D(gridIndex, gridSizeZ, gridSizeX, gridSizeY, -size.y, +size.y),
						sampleGrid1D(gridIndex, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::z_y_x:
					point = vec3_t(
						sampleGrid3D(gridIndex, gridSizeZ, gridSizeY, gridSizeX, -size.x, +size.x),
						sampleGrid2D(gridIndex, gridSizeZ, gridSizeY, -size.y, +size.y),
						sampleGrid1D(gridIndex, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1u) % (gridSizeX * gridSizeY * gridSizeZ);

			break;
		}
		default: {
			break;
		}
	}

	return point;
}

vec3_t ParticleGenerator::emitInCylinder(const vec3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex) {
	float_t h = 0.0;
	float_t r = 0.0;
	float_t phi = 0.0;
	vec3_t point = vec3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			h = random::uniform(rng, -1.0, +1.0);
			phi = random::uniform(rng, 0.0, 2.0 * pi);
			r = std::sqrt(random::uniform(rng, 0.0, 1.0));

			point = vec3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::center: {
			h = random::uniform(rng, -1.0, +1.0);
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * pi);
				point = vec3_t(
					std::cos(phi) * r,
					std::sin(phi) * r,
					h);
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 2) < point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			h = random::uniform(rng, -1.0, +1.0);
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * pi);
				point = vec3_t(
					std::cos(phi) * r,
					std::sin(phi) * r,
					h);
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 2) > point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			float_t baseArea = size.x * size.y * pi;
			float_t lateralArea = pi * (size.x + size.y) * size.z;
			float_t side = random::uniform(rng, 0.0, baseArea * 2.0 + lateralArea);
			if(side < baseArea) {
				h = -1.0;
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
			}
			else if(side < baseArea * 2.0) {
				h = +1.0;
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
			}
			else {
				h = random::uniform(rng, -1.0, +1.0);
				r = 1.0;
			}

			phi = random::uniform(rng, 0.0, 2.0 * pi);
			point = vec3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			r = random::uniformGrid(rng, gridSizeX);
			phi = random::uniformGrid(rng, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
			h = random::uniformGrid(rng, gridSizeZ, -1.0, +1.0);
			point = vec3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
					r = sampleGrid1D(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2D(gridIndex, gridSizeX, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid3D(gridIndex, gridSizeX, gridSizeY, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::x_z_y:
					r = sampleGrid1D(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3D(gridIndex, gridSizeX, gridSizeZ, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid2D(gridIndex, gridSizeX, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::y_x_z:
					r = sampleGrid2D(gridIndex, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1D(gridIndex, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid3D(gridIndex, gridSizeY, gridSizeX, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::y_z_x:
					r = sampleGrid3D(gridIndex, gridSizeY, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1D(gridIndex, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid2D(gridIndex, gridSizeY, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::z_x_y:
					r = sampleGrid2D(gridIndex, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3D(gridIndex, gridSizeZ, gridSizeX, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid1D(gridIndex, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::z_y_x:
					r = sampleGrid3D(gridIndex, gridSizeZ, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2D(gridIndex, gridSizeZ, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid1D(gridIndex, gridSizeZ, -1.0, +1.0);
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1u) % (gridSizeX * gridSizeY * gridSizeZ);
			point = vec3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		default: {
			break;
		}
	}

	point *= size * 0.5;

	return point;
}
}