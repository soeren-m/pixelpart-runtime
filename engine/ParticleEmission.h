#pragma once

#include "../effect/ParticleEmitter.h"
#include "../common/Random.h"

namespace pixelpart {
vec3_t rotate2d(const vec3_t& v, float_t a);
vec3_t rotate3d(const vec3_t& v, const vec3_t& a);

vec3_t emitOnSegment(std::mt19937& rng, float_t length,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSize, uint32_t& gridIndex);

vec3_t emitInEllipse(std::mt19937& rng, const vec2_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t& gridIndex);

vec3_t emitInRectangle(std::mt19937& rng, const vec2_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t& gridIndex);

vec3_t emitOnPath(std::mt19937& rng, const Curve<vec3_t>& path,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSize, uint32_t& gridIndex);

vec3_t emitInEllipsoid(std::mt19937& rng, const vec3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex);

vec3_t emitInCuboid(std::mt19937& rng, const vec3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex);

vec3_t emitInCylinder(std::mt19937& rng, const vec3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex);
}