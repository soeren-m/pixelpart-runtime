#include "ParticleEmitter.h"
#include "NodeType.h"
#include <algorithm>

namespace pixelpart {
ParticleEmitter::ParticleEmitter(id_t ownId, id_t parentId) : Node(ownId, parentId) {

}

void ParticleEmitter::applyInputs(const ComputeGraph::InputSet& inputs) {
	Node::applyInputs(inputs);

	emitterDirection.input(inputs);
	emitterSpread.input(inputs);
}

void ParticleEmitter::primary(bool mode) {
	primaryEmitter = mode;
}
bool ParticleEmitter::primary() const {
	return primaryEmitter;
}

void ParticleEmitter::particleTypes(const std::vector<id_t>& particleTypes) {
	emitterParticleTypes = particleTypes;
}
void ParticleEmitter::addParticleType(id_t particleTypeId) {
	emitterParticleTypes.push_back(particleTypeId);
}
void ParticleEmitter::removeParticleType(id_t particleTypeId) {
	emitterParticleTypes.erase(
		std::remove(emitterParticleTypes.begin(), emitterParticleTypes.end(), particleTypeId), emitterParticleTypes.end());
}
bool ParticleEmitter::hasParticleType(id_t particleTypeId) const {
	return std::find(emitterParticleTypes.begin(), emitterParticleTypes.end(), particleTypeId) != emitterParticleTypes.end();
}
const std::vector<id_t>& ParticleEmitter::particleTypes() const {
	return emitterParticleTypes;
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

void ParticleEmitter::gridSize(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
	emitterGridSize[0] = x;
	emitterGridSize[1] = y;
	emitterGridSize[2] = z;
}
std::uint32_t ParticleEmitter::gridSizeX() const {
	return emitterGridSize[0];
}
std::uint32_t ParticleEmitter::gridSizeY() const {
	return emitterGridSize[1];
}
std::uint32_t ParticleEmitter::gridSizeZ() const {
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

Node* ParticleEmitter::cloneImpl() const {
	return new ParticleEmitter(*this);
}

void to_json(nlohmann::ordered_json& j, const ParticleEmitter& particleEmitter) {
	to_json(j, static_cast<const Node&>(particleEmitter));
	j.update(nlohmann::ordered_json{
		{ "node_type", NodeType::particle_emitter },
		{ "primary", particleEmitter.primary() },
		{ "particle_types", particleEmitter.particleTypes() },
		{ "shape", particleEmitter.shape() },
		{ "path", particleEmitter.path() },
		{ "distribution", particleEmitter.distribution() },
		{ "grid_order", particleEmitter.gridOrder() },
		{ "grid_size_x", particleEmitter.gridSizeX() },
		{ "grid_size_y", particleEmitter.gridSizeY() },
		{ "grid_size_z", particleEmitter.gridSizeZ() },
		{ "emission_mode", particleEmitter.emissionMode() },
		{ "direction_mode", particleEmitter.directionMode() },
		{ "direction", particleEmitter.direction() },
		{ "spread", particleEmitter.spread() },
	});
}
void from_json(const nlohmann::ordered_json& j, ParticleEmitter& particleEmitter) {
	particleEmitter = ParticleEmitter(j.at("id").get<id_t>());

	from_json(j, static_cast<Node&>(particleEmitter));

	particleEmitter.primary(j.value("primary", true));
	particleEmitter.particleTypes(j.value("particle_types", std::vector<id_t>()));
	particleEmitter.shape(j.value("shape", ParticleEmitter::Shape::point));
	particleEmitter.path() = j.value("path", Curve<float3_t>());
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