#pragma once

#include "../effect/ParticleEmitter.h"
#include "../common/Random.h"

namespace pixelpart {
vec3d rotate2d(const vec3d& v, floatd a);
vec3d rotate3d(const vec3d& v, const vec3d& a);

vec3d generateOnSegment(std::mt19937& rng, floatd length, ParticleEmitter::Distribution distribution);
vec3d generateInEllipse(std::mt19937& rng, const vec2d& size, ParticleEmitter::Distribution distribution);
vec3d generateInRectangle(std::mt19937& rng, const vec2d& size, ParticleEmitter::Distribution distribution);
vec3d generateOnPath(std::mt19937& rng, const Curve<vec3d>& path, ParticleEmitter::Distribution distribution);
vec3d generateInEllipsoid(std::mt19937& rng, const vec3d& size, ParticleEmitter::Distribution distribution);
vec3d generateInCuboid(std::mt19937& rng, const vec3d& size, ParticleEmitter::Distribution distribution);
vec3d generateInCylinder(std::mt19937& rng, const vec3d& size, ParticleEmitter::Distribution distribution);
}