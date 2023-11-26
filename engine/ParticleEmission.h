#pragma once

#include "../effect/ParticleEmitter.h"
#include "../common/Random.h"

namespace pixelpart {
vec3d rotate2d(const vec3d& v, floatd a);
vec3d rotate3d(const vec3d& v, const vec3d& a);

vec3d emitOnSegment(std::mt19937& rng, floatd length,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSize, uint32_t& gridIndex);

vec3d emitInEllipse(std::mt19937& rng, const vec2d& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t& gridIndex);

vec3d emitInRectangle(std::mt19937& rng, const vec2d& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t& gridIndex);

vec3d emitOnPath(std::mt19937& rng, const Curve<vec3d>& path,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSize, uint32_t& gridIndex);

vec3d emitInEllipsoid(std::mt19937& rng, const vec3d& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex);

vec3d emitInCuboid(std::mt19937& rng, const vec3d& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex);

vec3d emitInCylinder(std::mt19937& rng, const vec3d& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex);
}