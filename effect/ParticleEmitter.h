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
		boundary = 3
	};
	enum class EmissionMode : uint32_t {
		continuous = 0,
		burst_start = 1,
		burst_end = 2
	};
	enum class DirectionMode : uint32_t {
		fixed = 0,
		outwards = 1,
		inwards = 2
	};

	Shape shape = Shape::point;
	Curve<vec3d> path = Curve<vec3d>();
	Curve<vec3d> size = Curve<vec3d>(vec3d(1.0));
	Curve<vec3d> orientation = Curve<vec3d>(vec3d(0.0));

	Distribution distribution = Distribution::uniform;
	EmissionMode emissionMode = EmissionMode::continuous;
	DirectionMode directionMode = DirectionMode::fixed;
	Curve<vec3d> direction = Curve<vec3d>(vec3d(0.0));
	Curve<floatd> spread = Curve<floatd>(0.0);
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
	{ ParticleEmitter::Distribution::boundary, "boundary" }
})

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleEmitter::EmissionMode, {
	{ ParticleEmitter::EmissionMode::continuous, "continuous" },
	{ ParticleEmitter::EmissionMode::burst_start, "burst_start" },
	{ ParticleEmitter::EmissionMode::burst_end, "burst_end" }
})

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleEmitter::DirectionMode, {
	{ ParticleEmitter::DirectionMode::fixed, "fixed" },
	{ ParticleEmitter::DirectionMode::outwards, "outwards" },
	{ ParticleEmitter::DirectionMode::inwards, "inwards" }
})

void to_json(nlohmann::ordered_json& j, const ParticleEmitter& emitter);
void from_json(const nlohmann::ordered_json& j, ParticleEmitter& emitter);
}