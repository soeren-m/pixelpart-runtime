#include "ParticleEmitter.h"

namespace pixelpart {
ParticleEmitter::ParticleEmitter(id_t ownId, id_t parentId) : Node(ownId, parentId) {

}

void ParticleEmitter::shape(Shape shape) {
	emitterShape = shape;
}
ParticleEmitter::Shape ParticleEmitter::shape() const {
	return emitterShape;
}

Curve<float3_t>& ParticleEmitter::path() {
	return emitterPath;
}
const Curve<float3_t>& ParticleEmitter::path() const {
	return emitterPath;
}

AnimatedProperty<float3_t>& ParticleEmitter::size() {
	return emitterSize;
}
const AnimatedProperty<float3_t>& ParticleEmitter::size() const {
	return emitterSize;
}

AnimatedProperty<float3_t>& ParticleEmitter::orientation() {
	return emitterOrientation;
}
const AnimatedProperty<float3_t>& ParticleEmitter::orientation() const {
	return emitterOrientation;
}

void ParticleEmitter::distribution(Distribution distribution) {
	emitterDistribution = distribution;
}
ParticleEmitter::Distribution ParticleEmitter::distribution() const {
	return emitterDistribution;
}

void ParticleEmitter::gridOrder(GridOrder gridOrder) {
	emitterGridOrder = gridOrder;
}
ParticleEmitter::GridOrder ParticleEmitter::gridOrder() const {
	return emitterGridOrder;
}

void ParticleEmitter::gridSize(uint32_t x, uint32_t y, uint32_t z) {
	emitterGridSize[0] = x;
	emitterGridSize[1] = y;
	emitterGridSize[2] = z;
}
uint32_t ParticleEmitter::gridSizeX() const {
	return emitterGridSize[0];
}
uint32_t ParticleEmitter::gridSizeY() const {
	return emitterGridSize[1];
}
uint32_t ParticleEmitter::gridSizeZ() const {
	return emitterGridSize[2];
}

void ParticleEmitter::emissionMode(EmissionMode emissionMode) {
	emitterEmissionMode = emissionMode;
}
ParticleEmitter::EmissionMode ParticleEmitter::emissionMode() const {
	return emitterEmissionMode;
}

void ParticleEmitter::directionMode(DirectionMode directionMode) {
	emitterDirectionMode = directionMode;
}
ParticleEmitter::DirectionMode ParticleEmitter::directionMode() const {
	return emitterDirectionMode;
}

AnimatedProperty<float3_t>& ParticleEmitter::direction() {
	return emitterDirection;
}
const AnimatedProperty<float3_t>& ParticleEmitter::direction() const {
	return emitterDirection;
}

AnimatedProperty<float_t>& ParticleEmitter::spread() {
	return emitterSpread;
}
const AnimatedProperty<float_t>& ParticleEmitter::spread() const {
	return emitterSpread;
}

void to_json(nlohmann::ordered_json& j, const ParticleEmitter& particleEmitter) {
	j = nlohmann::ordered_json{
		{ "id", particleEmitter.id() },
		{ "parent_id", particleEmitter.parentId() },
		{ "name", particleEmitter.name() },
		{ "lifetime_start", particleEmitter.start() },
		{ "lifetime_duration", particleEmitter.duration() },
		{ "repeat", particleEmitter.repeat() },
		{ "position", particleEmitter.position() },

		{ "shape", particleEmitter.shape() },
		{ "path", particleEmitter.path() },
		{ "size", particleEmitter.size() },
		{ "orientation", particleEmitter.orientation() },
		{ "distribution", particleEmitter.distribution() },
		{ "grid_order", particleEmitter.gridOrder() },
		{ "grid_size_x", particleEmitter.gridSizeX() },
		{ "grid_size_y", particleEmitter.gridSizeY() },
		{ "grid_size_z", particleEmitter.gridSizeZ() },
		{ "emission_mode", particleEmitter.emissionMode() },
		{ "direction_mode", particleEmitter.directionMode() },
		{ "direction", particleEmitter.direction() },
		{ "spread", particleEmitter.spread() },
	};
}
void from_json(const nlohmann::ordered_json& j, ParticleEmitter& particleEmitter) {
	particleEmitter = ParticleEmitter(
		j.at("id"),
		j.value("parent_id", id_t()));

	particleEmitter.name(j.value("name", ""));
	particleEmitter.start(j.value("lifetime_start", 0.0));
	particleEmitter.duration(j.value("lifetime_duration", 1.0));
	particleEmitter.repeat(j.value("repeat", true));
	particleEmitter.position() = j.value("position", AnimatedProperty<float3_t>(0.0, float3_t(0.0)));

	particleEmitter.shape(j.value("shape", ParticleEmitter::Shape::point));
	particleEmitter.path() = j.value("path", Curve<float3_t>());
	particleEmitter.size() = j.value("size", AnimatedProperty<float3_t>(float3_t(1.0)));
	particleEmitter.orientation() = j.value("orientation", AnimatedProperty<float3_t>(float3_t(0.0)));
	particleEmitter.distribution(j.value("distribution", ParticleEmitter::Distribution::uniform));
	particleEmitter.gridOrder(j.value("grid_order", ParticleEmitter::GridOrder::x_y_z));
	particleEmitter.gridSize(
		j.value("grid_size_x", 5u),
		j.value("grid_size_y", 5u),
		j.value("grid_size_z", 5u));
	particleEmitter.emissionMode(j.value("emission_mode", ParticleEmitter::EmissionMode::continuous));
	particleEmitter.directionMode(j.value("direction_mode", ParticleEmitter::DirectionMode::fixed));
	particleEmitter.direction() = j.value("direction", AnimatedProperty<float3_t>(float3_t(0.0)));
	particleEmitter.spread() = j.value("spread", AnimatedProperty<float_t>(0.0));
}
}