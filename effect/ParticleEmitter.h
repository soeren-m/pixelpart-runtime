#pragma once

#include "Node.h"

namespace pixelpart {
struct ParticleEmitter : public Node {
	enum class Shape : uint32_t {
		point = 0,
		line = 1,
		ellipse = 2,
		rectangle = 3,
		path = 4,
		ellipsoid = 5,
		cuboid = 6,
		cylinder = 7
	};
	enum class Distribution : uint32_t {
		uniform = 0,
		center = 1,
		hole = 2,
		boundary = 3,
		grid_random = 4,
		grid_ordered = 5
	};
	enum class GridOrder : uint32_t {
		x_y_z = 0,
		x_z_y = 1,
		y_x_z = 2,
		y_z_x = 3,
		z_x_y = 4,
		z_y_x = 5
	};
	enum class EmissionMode : uint32_t {
		continuous = 0,
		burst_start = 1,
		burst_end = 2
	};
	enum class DirectionMode : uint32_t {
		fixed = 0,
		outwards = 1,
		inwards = 2,
		inherit = 3,
		inherit_inverse = 4
	};

	Shape shape = Shape::point;
	Curve<vec3_t> path = Curve<vec3_t>();
	AnimatedProperty<vec3_t> size = AnimatedProperty<vec3_t>(vec3_t(1.0));
	AnimatedProperty<vec3_t> orientation = AnimatedProperty<vec3_t>(vec3_t(0.0));

	Distribution distribution = Distribution::uniform;
	GridOrder gridOrder = GridOrder::x_y_z;
	uint32_t gridSize[3] = { 5u, 5u, 5u };
	EmissionMode emissionMode = EmissionMode::continuous;
	DirectionMode directionMode = DirectionMode::fixed;
	AnimatedProperty<vec3_t> direction = AnimatedProperty<vec3_t>(vec3_t(0.0));
	AnimatedProperty<float_t> spread = AnimatedProperty<float_t>(0.0);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleEmitter::Shape, {
	{ ParticleEmitter::Shape::point, "point" },
	{ ParticleEmitter::Shape::line, "line" },
	{ ParticleEmitter::Shape::ellipse, "ellipse" },
	{ ParticleEmitter::Shape::rectangle, "rectangle" },
	{ ParticleEmitter::Shape::path, "path" },
	{ ParticleEmitter::Shape::ellipsoid, "ellipsoid" },
	{ ParticleEmitter::Shape::cuboid, "cuboid" },
	{ ParticleEmitter::Shape::cylinder, "cylinder" }
})

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleEmitter::Distribution, {
	{ ParticleEmitter::Distribution::uniform, "uniform" },
	{ ParticleEmitter::Distribution::center, "center" },
	{ ParticleEmitter::Distribution::hole, "hole" },
	{ ParticleEmitter::Distribution::boundary, "boundary" },
	{ ParticleEmitter::Distribution::grid_random, "grid_random" },
	{ ParticleEmitter::Distribution::grid_ordered, "grid_ordered" }
})

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleEmitter::GridOrder, {
	{ ParticleEmitter::GridOrder::x_y_z, "x_y_z" },
	{ ParticleEmitter::GridOrder::x_z_y, "x_z_y" },
	{ ParticleEmitter::GridOrder::y_x_z, "y_x_z" },
	{ ParticleEmitter::GridOrder::y_z_x, "y_z_x" },
	{ ParticleEmitter::GridOrder::z_x_y, "z_x_y" },
	{ ParticleEmitter::GridOrder::z_y_x, "z_y_x" }
})

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleEmitter::EmissionMode, {
	{ ParticleEmitter::EmissionMode::continuous, "continuous" },
	{ ParticleEmitter::EmissionMode::burst_start, "burst_start" },
	{ ParticleEmitter::EmissionMode::burst_end, "burst_end" }
})

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleEmitter::DirectionMode, {
	{ ParticleEmitter::DirectionMode::fixed, "fixed" },
	{ ParticleEmitter::DirectionMode::outwards, "outwards" },
	{ ParticleEmitter::DirectionMode::inwards, "inwards" },
	{ ParticleEmitter::DirectionMode::inherit, "inherit" },
	{ ParticleEmitter::DirectionMode::inherit_inverse, "inherit_inverse" }
})

void to_json(nlohmann::ordered_json& j, const ParticleEmitter& emitter);
void from_json(const nlohmann::ordered_json& j, ParticleEmitter& emitter);
}