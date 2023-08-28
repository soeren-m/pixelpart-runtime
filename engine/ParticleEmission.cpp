#define _USE_MATH_DEFINES
#include "ParticleEmission.h"
#include <cmath>

namespace pixelpart {
floatd sample(std::mt19937& rng, ParticleEmitter::Distribution distribution, floatd min, floatd max) {
	switch(distribution) {
		case ParticleEmitter::Distribution::uniform:
		case ParticleEmitter::Distribution::boundary:
			return random::uniform(rng, min, max);
		case ParticleEmitter::Distribution::center:
			return random::normal(rng, min, max);
		case ParticleEmitter::Distribution::hole:
			return random::normalReverse(rng, min, max);
		default:
			return 0.0;
	}
}

vec3d rotate2d(const vec3d& v, floatd a) {
	return vec3d(glm::rotate(vec2d(v), glm::radians(a)), 0.0);
}
vec3d rotate3d(const vec3d& v, const vec3d& a) {
	return vec3d(glm::yawPitchRoll(glm::radians(a.y), glm::radians(a.z), glm::radians(a.x)) * vec4d(v, 0.0));
}

vec3d generateOnSegment(std::mt19937& rng, floatd length, ParticleEmitter::Distribution distribution) {
	return vec3d(0.0, sample(rng, distribution, -length, +length) * 0.5, 0.0);
}

vec3d generateInEllipse(std::mt19937& rng, const vec2d& size, ParticleEmitter::Distribution distribution) {
	floatd r = 0.0;
	floatd phi = 0.0;
	vec3d point = vec3d(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			r = std::sqrt(random::uniform(rng, 0.0, 1.0));
			phi = random::uniform(rng, 0.0, 2.0 * M_PI);
			point = vec3d(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * M_PI);
				point = vec3d(
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
				phi = random::uniform(rng, 0.0, 2.0 * M_PI);
				point = vec3d(
					std::cos(phi),
					std::sin(phi),
					0.0) * r;
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 2) > point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			phi = random::uniform(rng, 0.0, 2.0 * M_PI);
			point = vec3d(
				std::cos(phi),
				std::sin(phi),
				0.0);

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

vec3d generateInRectangle(std::mt19937& rng, const vec2d& size, ParticleEmitter::Distribution distribution) {
	vec3d point = vec3d(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			point = vec3d(
				random::uniform(rng, -size.x, +size.x),
				random::uniform(rng, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			point = vec3d(
				random::normal(rng, -size.x, +size.x),
				random::normal(rng, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				point = vec3d(
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
			floatd r = random::uniform(rng, 0.0, (size.x + size.y) * 2.0);
			point = vec3d(-size * 0.5, 0.0);

			if(r < size.y) {
				point += vec3d(0.0, r, 0.0);
			}
			else if(r < size.y + size.x) {
				point += vec3d(r - size.y, size.y, 0.0);
			}
			else if(r < (size.y + size.x) + size.y) {
				point += vec3d(size.x, r - (size.y + size.x), 0.0);
			}
			else {
				point += vec3d(r - (size.y + size.x + size.y), 0.0, 0.0);
			}

			break;
		}
		default: {
			break;
		}
	}

	return point;
}

vec3d generateOnPath(std::mt19937& rng, const Curve<vec3d>& path, ParticleEmitter::Distribution distribution) {
	return path.get(sample(rng, distribution, 0.0, 1.0));
}

vec3d generateInEllipsoid(std::mt19937& rng, const vec3d& size, ParticleEmitter::Distribution distribution) {
	floatd r = 0.0;
	floatd phi = 0.0;
	floatd theta = 0.0;
	vec3d point = vec3d(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			r = std::sqrt(random::uniform(rng, 0.0, 1.0));
			phi = random::uniform(rng, 0.0, 2.0 * M_PI);
			theta = std::acos(random::uniform(rng, -1.0, +1.0));
			point = vec3d(
				std::sin(theta) * std::cos(phi),
				std::sin(theta) * std::sin(phi),
				std::cos(theta)) * r;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * M_PI);
				theta = std::acos(random::uniform(rng, -1.0, +1.0));
				point = vec3d(
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
				phi = random::uniform(rng, 0.0, 2.0 * M_PI);
				theta = std::acos(random::uniform(rng, -1.0, +1.0));
				point = vec3d(
					std::sin(theta) * std::cos(phi),
					std::sin(theta) * std::sin(phi),
					std::cos(theta)) * r;
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 3) > point.x * point.x + point.y * point.y + point.z * point.z);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			phi = random::uniform(rng, 0.0, 2.0 * M_PI);
			theta = std::acos(random::uniform(rng, -1.0, +1.0));
			point = vec3d(
				std::sin(theta) * std::cos(phi),
				std::sin(theta) * std::sin(phi),
				std::cos(theta));

			break;
		}
		default: {
			break;
		}
	}

	point *= size * 0.5;

	return point;
}

vec3d generateInCuboid(std::mt19937& rng, const vec3d& size, ParticleEmitter::Distribution distribution) {
	vec3d point = vec3d(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			point = vec3d(
				random::uniform(rng, -size.x, +size.x),
				random::uniform(rng, -size.y, +size.y),
				random::uniform(rng, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			point = vec3d(
				random::normal(rng, -size.x, +size.x),
				random::normal(rng, -size.y, +size.y),
				random::normal(rng, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				point = vec3d(
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
					point = vec3d(-size.x, random::uniform(rng, -size.y, +size.y), random::uniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 1:
					point = vec3d(+size.x, random::uniform(rng, -size.y, +size.y), random::uniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 2:
					point = vec3d(random::uniform(rng, -size.x, +size.x), -size.y, random::uniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 3:
					point = vec3d(random::uniform(rng, -size.x, +size.x), +size.y, random::uniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 4:
					point = vec3d(random::uniform(rng, -size.x, +size.x), random::uniform(rng, -size.y, +size.y), -size.z) * 0.5;
					break;
				case 5:
					point = vec3d(random::uniform(rng, -size.x, +size.x), random::uniform(rng, -size.y, +size.y), +size.z) * 0.5;
					break;
				default:
					point = vec3d(0.0);
					break;
			}

			break;
		}
		default: {
			break;
		}
	}

	return point;
}

vec3d generateInCylinder(std::mt19937& rng, const vec3d& size, ParticleEmitter::Distribution distribution) {
	floatd h = 0.0;
	floatd r = 0.0;
	floatd phi = 0.0;
	vec3d point = vec3d(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			h = random::uniform(rng, -1.0, +1.0);
			phi = random::uniform(rng, 0.0, 2.0 * M_PI);
			r = std::sqrt(random::uniform(rng, 0.0, 1.0));

			point = vec3d(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::center: {
			h = random::uniform(rng, -1.0, +1.0);
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * M_PI);
				point = vec3d(
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
				phi = random::uniform(rng, 0.0, 2.0 * M_PI);
				point = vec3d(
					std::cos(phi) * r,
					std::sin(phi) * r,
					h);
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 2) > point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			floatd baseArea = size.x * size.y * M_PI;
			floatd lateralArea = M_PI * (size.x + size.y) * size.z;
			floatd side = random::uniform(rng, 0.0, baseArea * 2.0 + lateralArea);
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

			phi = random::uniform(rng, 0.0, 2.0 * M_PI);
			point = vec3d(
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