#include "SpawnPositionGeneration.h"
#include "../math/Common.h"
#include "../math/Constants.h"
#include <cmath>
#include <random>

namespace pixelpart {
namespace {
float_t randomCentered(pcg32& rng, float_t min, float_t max) {
	float_t u1 = rng.next();
	float_t u2 = rng.next();
	float_t u3 = rng.next();

	return (u1 + u2 + u3) / 3.0 * (max - min) + min;
}
float_t randomInverseCentered(pcg32& rng, float_t min, float_t max) {
	float_t u1 = rng.next();
	float_t u2 = rng.next();
	float_t u3 = rng.next();

	float_t x = (u1 + u2 + u3) / 3.0;

	return (0.5 + std::copysign(0.5 - std::abs(x - 0.5), x - 0.5)) * (max - min) + min;
}
float_t randomUniformGrid(pcg32& rng, std::uint32_t size, float_t min, float_t max) {
	std::uniform_int_distribution<std::uint32_t> distrib(0, size - 1);

	return static_cast<float_t>(distrib(rng)) / static_cast<float_t>(size - 1) * (max - min) + min;
}

float_t sampleGrid1d(std::uint32_t gridIndex, std::uint32_t gridSize, float_t min, float_t max) {
	return static_cast<float_t>(gridIndex % gridSize) / static_cast<float_t>(gridSize - 1) * (max - min) + min;
}
float_t sampleGrid2d(std::uint32_t gridIndex, std::uint32_t gridSize1, std::uint32_t gridSize2, float_t min, float_t max) {
	return static_cast<float_t>(gridIndex / gridSize1 % gridSize2) / static_cast<float_t>(gridSize2 - 1) * (max - min) + min;
}
float_t sampleGrid3d(std::uint32_t gridIndex, std::uint32_t gridSize1, std::uint32_t gridSize2, std::uint32_t gridSize3, float_t min, float_t max) {
	return static_cast<float_t>(gridIndex / gridSize1 / gridSize2 % gridSize3) / static_cast<float_t>(gridSize3 - 1) * (max - min) + min;
}

float3_t generateLineSpawnPosition(float_t length,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSize, std::uint32_t& gridIndex,
	pcg32& rng) {
	switch(distribution) {
		case ParticleEmitter::Distribution::uniform:
		case ParticleEmitter::Distribution::boundary: {
			return float3_t(rng.next(-length, +length) * 0.5, 0.0, 0.0);
		}
		case ParticleEmitter::Distribution::center: {
			return float3_t(randomCentered(rng, -length, +length) * 0.5, 0.0, 0.0);
		}
		case ParticleEmitter::Distribution::hole: {
			return float3_t(randomInverseCentered(rng, -length, +length) * 0.5, 0.0, 0.0);
		}
		case ParticleEmitter::Distribution::grid_random: {
			return float3_t(randomUniformGrid(rng, gridSize, -length, +length) * 0.5, 0.0, 0.0);
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			float_t x = sampleGrid1d(gridIndex, gridSize, -length, +length) * 0.5;
			gridIndex = (gridIndex + 1) % gridSize;
			return float3_t(x, 0.0, 0.0);
		}
		default: {
			return float3_t(0.0, 0.0, 0.0);
		}
	}
}

float3_t generateEllipseSpawnPosition(const float2_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t& gridIndex,
	pcg32& rng) {
	float_t r = 0.0;
	float_t phi = 0.0;
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			r = std::sqrt(rng.next());
			phi = rng.next(0.0, math::twoPi);
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			r = rng.next();
			r *= r;
			phi = rng.next(0.0, math::twoPi);
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			r = std::sqrt(std::sqrt(std::sqrt(rng.next())));
			phi = rng.next(0.0, math::twoPi);
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			phi = rng.next(0.0, math::twoPi);
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0);

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			r = randomUniformGrid(rng, gridSizeX, 0.0, 1.0);
			phi = randomUniformGrid(rng, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
			point = float3_t(
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
					r = sampleGrid1d(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2d(gridIndex, gridSizeX, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					break;
				case ParticleEmitter::GridOrder::y_x_z:
				case ParticleEmitter::GridOrder::y_z_x:
				case ParticleEmitter::GridOrder::z_y_x:
					r = sampleGrid2d(gridIndex, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1d(gridIndex, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1) % (gridSizeX * gridSizeY);
			point = float3_t(
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

float3_t generateRectangleSpawnPosition(const float2_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t& gridIndex,
	pcg32& rng) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			point = float3_t(
				rng.next(-size.x, +size.x),
				rng.next(-size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			point = float3_t(
				randomCentered(rng, -size.x, +size.x),
				randomCentered(rng, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			float_t r = rng.next(0.0, (size.x + size.y) * 2.0);
			float_t d = rng.next();
			d = d * d * d * d * 0.5;

			point = float3_t(-size * 0.5, 0.0);

			if(r < size.y) {
				point += float3_t(d * size.x, r, 0.0);
			}
			else if(r < size.y + size.x) {
				point += float3_t(r - size.y, size.y - d * size.y, 0.0);
			}
			else if(r < (size.y + size.x) + size.y) {
				point += float3_t(size.x - d * size.x, r - (size.y + size.x), 0.0);
			}
			else {
				point += float3_t(r - (size.y + size.x + size.y), d * size.y, 0.0);
			}

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			float_t r = rng.next(0.0, (size.x + size.y) * 2.0);
			point = float3_t(-size * 0.5, 0.0);

			if(r < size.y) {
				point += float3_t(0.0, r, 0.0);
			}
			else if(r < size.y + size.x) {
				point += float3_t(r - size.y, size.y, 0.0);
			}
			else if(r < (size.y + size.x) + size.y) {
				point += float3_t(size.x, r - (size.y + size.x), 0.0);
			}
			else {
				point += float3_t(r - (size.y + size.x + size.y), 0.0, 0.0);
			}

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			point = float3_t(
				randomUniformGrid(rng, gridSizeX, -size.x, +size.x),
				randomUniformGrid(rng, gridSizeY, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
				case ParticleEmitter::GridOrder::x_z_y:
				case ParticleEmitter::GridOrder::z_x_y:
					point = float3_t(
						sampleGrid1d(gridIndex, gridSizeX, -size.x, +size.x),
						sampleGrid2d(gridIndex, gridSizeX, gridSizeY, -size.y, +size.y),
						0.0) * 0.5;
					break;
				case ParticleEmitter::GridOrder::y_x_z:
				case ParticleEmitter::GridOrder::y_z_x:
				case ParticleEmitter::GridOrder::z_y_x:
					point = float3_t(
						sampleGrid2d(gridIndex, gridSizeY, gridSizeX, -size.x, +size.x),
						sampleGrid1d(gridIndex, gridSizeY, -size.y, +size.y),
						0.0) * 0.5;
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1) % (gridSizeX * gridSizeY);

			break;
		}
		default: {
			break;
		}
	}

	return point;
}

float3_t generatePathSpawnPosition(const float3_t& size,
	const Polyline& path,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSize, std::uint32_t& gridIndex,
	pcg32& rng) {
	float_t d = 0.0;

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform:
		case ParticleEmitter::Distribution::boundary: {
			d = rng.next(0.0, path.length());
			break;
		}
		case ParticleEmitter::Distribution::center: {
			d = randomCentered(rng, 0.0, path.length());
			break;
		}
		case ParticleEmitter::Distribution::hole: {
			d = randomInverseCentered(rng, 0.0, path.length());
			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			d = randomUniformGrid(rng, gridSize, 0.0, path.length());
			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			d = sampleGrid1d(gridIndex, gridSize, 0.0, path.length());
			gridIndex = (gridIndex + 1) % gridSize;
			break;
		}
		default: {
			break;
		}
	}

	return path.sample(d) * size;
}

float3_t generateEllipsoidSpawnPosition(const float3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex,
	pcg32& rng) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			float_t r = std::sqrt(rng.next());
			float_t phi = rng.next(0.0, math::twoPi);
			float_t ctheta = rng.next(-1.0, +1.0);
			float_t theta = std::acos(ctheta);
			float_t stheta = std::sin(theta);

			point = float3_t(
				stheta * std::cos(phi),
				stheta * std::sin(phi),
				ctheta) * r;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			float_t r = rng.next();
			r *= r;
			float_t phi = rng.next(0.0, math::twoPi);
			float_t ctheta = rng.next(-1.0, +1.0);
			float_t theta = std::acos(ctheta);
			float_t stheta = std::sin(theta);

			point = float3_t(
				stheta * std::cos(phi),
				stheta * std::sin(phi),
				ctheta) * r;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			float_t r = std::sqrt(std::sqrt(std::sqrt(rng.next())));
			float_t phi = rng.next(0.0, math::twoPi);
			float_t ctheta = rng.next(-1.0, +1.0);
			float_t theta = std::acos(ctheta);
			float_t stheta = std::sin(theta);

			point = float3_t(
				stheta * std::cos(phi),
				stheta * std::sin(phi),
				ctheta) * r;

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			float_t phi = rng.next(0.0, math::twoPi);
			float_t ctheta = rng.next(-1.0, +1.0);
			float_t theta = std::acos(ctheta);
			float_t stheta = std::sin(theta);

			point = float3_t(
				stheta * std::cos(phi),
				stheta * std::sin(phi),
				ctheta);

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			float_t r = randomUniformGrid(rng, gridSizeX, 0.0, 1.0);
			float_t phi = randomUniformGrid(rng, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
			float_t theta = randomUniformGrid(rng, gridSizeZ, 0.0, math::pi);
			float_t stheta = std::sin(theta);
			float_t ctheta = std::cos(theta);

			point = float3_t(
				stheta * std::cos(phi),
				stheta * std::sin(phi),
				ctheta) * r;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			float_t r = 0.0;
			float_t phi = 0.0;
			float_t theta = 0.0;
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
					r = sampleGrid1d(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2d(gridIndex, gridSizeX, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid3d(gridIndex, gridSizeX, gridSizeY, gridSizeZ, 0.0, math::pi);
					break;
				case ParticleEmitter::GridOrder::x_z_y:
					r = sampleGrid1d(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3d(gridIndex, gridSizeX, gridSizeZ, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid2d(gridIndex, gridSizeX, gridSizeZ, 0.0, math::pi);
					break;
				case ParticleEmitter::GridOrder::y_x_z:
					r = sampleGrid2d(gridIndex, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1d(gridIndex, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid3d(gridIndex, gridSizeY, gridSizeX, gridSizeZ, 0.0, math::pi);
					break;
				case ParticleEmitter::GridOrder::y_z_x:
					r = sampleGrid3d(gridIndex, gridSizeY, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1d(gridIndex, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid2d(gridIndex, gridSizeY, gridSizeZ, 0.0, math::pi);
					break;
				case ParticleEmitter::GridOrder::z_x_y:
					r = sampleGrid2d(gridIndex, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3d(gridIndex, gridSizeZ, gridSizeX, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid1d(gridIndex, gridSizeZ, 0.0, math::pi);
					break;
				case ParticleEmitter::GridOrder::z_y_x:
					r = sampleGrid3d(gridIndex, gridSizeZ, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2d(gridIndex, gridSizeZ, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid1d(gridIndex, gridSizeZ, 0.0, math::pi);
					break;
				default:
					break;
			}

			float_t stheta = std::sin(theta);
			float_t ctheta = std::cos(theta);

			gridIndex = (gridIndex + 1) % (gridSizeX * gridSizeY * gridSizeZ);
			point = float3_t(
				stheta * std::cos(phi),
				stheta * std::sin(phi),
				ctheta) * r;

			break;
		}
		default: {
			break;
		}
	}

	point *= size * 0.5;

	return point;
}

float3_t generateCuboidSpawnPosition(const float3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex,
	pcg32& rng) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			point = float3_t(
				rng.next(-size.x, +size.x),
				rng.next(-size.y, +size.y),
				rng.next(-size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			point = float3_t(
				randomCentered(rng, -size.x, +size.x),
				randomCentered(rng, -size.y, +size.y),
				randomCentered(rng, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			std::int32_t side = std::uniform_int_distribution<std::int32_t>(0, 5)(rng);
			float_t d = rng.next();
			d = d * d * d * d;

			switch(side) {
				case 0:
					point = float3_t(-size.x + d * size.x, rng.next(-size.y, +size.y), rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 1:
					point = float3_t(+size.x - d * size.x, rng.next(-size.y, +size.y), rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 2:
					point = float3_t(rng.next(-size.x, +size.x), -size.y + d * size.y, rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 3:
					point = float3_t(rng.next(-size.x, +size.x), +size.y - d * size.y, rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 4:
					point = float3_t(rng.next(-size.x, +size.x), rng.next(-size.y, +size.y), -size.z + d * size.z) * 0.5;
					break;
				case 5:
					point = float3_t(rng.next(-size.x, +size.x), rng.next(-size.y, +size.y), +size.z - d * size.z) * 0.5;
					break;
				default:
					break;
			}

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			std::int32_t side = std::uniform_int_distribution<std::int32_t>(0, 5)(rng);

			switch(side) {
				case 0:
					point = float3_t(-size.x, rng.next(-size.y, +size.y), rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 1:
					point = float3_t(+size.x, rng.next(-size.y, +size.y), rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 2:
					point = float3_t(rng.next(-size.x, +size.x), -size.y, rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 3:
					point = float3_t(rng.next(-size.x, +size.x), +size.y, rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 4:
					point = float3_t(rng.next(-size.x, +size.x), rng.next(-size.y, +size.y), -size.z) * 0.5;
					break;
				case 5:
					point = float3_t(rng.next(-size.x, +size.x), rng.next(-size.y, +size.y), +size.z) * 0.5;
					break;
				default:
					break;
			}

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			point = float3_t(
				randomUniformGrid(rng, gridSizeX, -size.x, +size.x),
				randomUniformGrid(rng, gridSizeY, -size.y, +size.y),
				randomUniformGrid(rng, gridSizeZ, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
					point = float3_t(
						sampleGrid1d(gridIndex, gridSizeX, -size.x, +size.x),
						sampleGrid2d(gridIndex, gridSizeX, gridSizeY, -size.y, +size.y),
						sampleGrid3d(gridIndex, gridSizeX, gridSizeY, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::x_z_y:
					point = float3_t(
						sampleGrid1d(gridIndex, gridSizeX, -size.x, +size.x),
						sampleGrid3d(gridIndex, gridSizeX, gridSizeZ, gridSizeY, -size.y, +size.y),
						sampleGrid2d(gridIndex, gridSizeX, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::y_x_z:
					point = float3_t(
						sampleGrid2d(gridIndex, gridSizeY, gridSizeX, -size.x, +size.x),
						sampleGrid1d(gridIndex, gridSizeY, -size.y, +size.y),
						sampleGrid3d(gridIndex, gridSizeY, gridSizeX, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::y_z_x:
					point = float3_t(
						sampleGrid3d(gridIndex, gridSizeY, gridSizeZ, gridSizeX, -size.x, +size.x),
						sampleGrid1d(gridIndex, gridSizeY, -size.y, +size.y),
						sampleGrid2d(gridIndex, gridSizeY, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::z_x_y:
					point = float3_t(
						sampleGrid2d(gridIndex, gridSizeZ, gridSizeX, -size.x, +size.y),
						sampleGrid3d(gridIndex, gridSizeZ, gridSizeX, gridSizeY, -size.y, +size.y),
						sampleGrid1d(gridIndex, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::z_y_x:
					point = float3_t(
						sampleGrid3d(gridIndex, gridSizeZ, gridSizeY, gridSizeX, -size.x, +size.x),
						sampleGrid2d(gridIndex, gridSizeZ, gridSizeY, -size.y, +size.y),
						sampleGrid1d(gridIndex, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1) % (gridSizeX * gridSizeY * gridSizeZ);

			break;
		}
		default: {
			break;
		}
	}

	return point;
}

float3_t generateCylinderSpawnPosition(const float3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex,
	pcg32& rng) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			float_t h = rng.next(-1.0, +1.0);
			float_t phi = rng.next(0.0, math::twoPi);
			float_t r = std::sqrt(rng.next());

			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::center: {
			float_t h = randomCentered(rng, -1.0, +1.0);
			float_t r = rng.next();
			r *= r;
			float_t phi = rng.next(0.0, math::twoPi);

			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			float_t d = rng.next();
			d = d * d * d * d;

			float_t h = 0.0;
			float_t r = 0.0;
			float_t baseArea = size.x * size.y * math::pi;
			float_t lateralArea = math::pi * (size.x + size.y) * size.z;
			float_t side = rng.next(0.0, baseArea * 2.0 + lateralArea);
			if(side < baseArea) {
				h = -1.0 + d;
				r = std::sqrt(rng.next());
			}
			else if(side < baseArea * 2.0) {
				h = 1.0 - d;
				r = std::sqrt(rng.next());
			}
			else {
				h = rng.next(-1.0, +1.0);
				r = std::sqrt(std::sqrt(std::sqrt(rng.next())));
			}

			float_t phi = rng.next(0.0, math::twoPi);
			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			float_t h = 0.0;
			float_t r = 0.0;
			float_t baseArea = size.x * size.y * math::pi;
			float_t lateralArea = math::pi * (size.x + size.y) * size.z;
			float_t side = rng.next(0.0, baseArea * 2.0 + lateralArea);
			if(side < baseArea) {
				h = -1.0;
				r = std::sqrt(rng.next());
			}
			else if(side < baseArea * 2.0) {
				h = +1.0;
				r = std::sqrt(rng.next());
			}
			else {
				h = rng.next(-1.0, +1.0);
				r = 1.0;
			}

			float_t phi = rng.next(0.0, math::twoPi);
			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			float_t r = randomUniformGrid(rng, gridSizeX, 0.0, 1.0);
			float_t phi = randomUniformGrid(rng, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
			float_t h = randomUniformGrid(rng, gridSizeZ, -1.0, +1.0);
			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			float_t r = 0.0;
			float_t phi = 0.0;
			float_t h = 0.0;
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
					r = sampleGrid1d(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2d(gridIndex, gridSizeX, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid3d(gridIndex, gridSizeX, gridSizeY, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::x_z_y:
					r = sampleGrid1d(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3d(gridIndex, gridSizeX, gridSizeZ, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid2d(gridIndex, gridSizeX, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::y_x_z:
					r = sampleGrid2d(gridIndex, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1d(gridIndex, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid3d(gridIndex, gridSizeY, gridSizeX, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::y_z_x:
					r = sampleGrid3d(gridIndex, gridSizeY, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1d(gridIndex, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid2d(gridIndex, gridSizeY, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::z_x_y:
					r = sampleGrid2d(gridIndex, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3d(gridIndex, gridSizeZ, gridSizeX, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid1d(gridIndex, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::z_y_x:
					r = sampleGrid3d(gridIndex, gridSizeZ, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2d(gridIndex, gridSizeZ, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid1d(gridIndex, gridSizeZ, -1.0, +1.0);
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1) % (gridSizeX * gridSizeY * gridSizeZ);
			point = float3_t(
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

float3_t generateParticleSpawnPosition(
	const float3_t& scale,
	ParticleEmitter::Shape shape,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	const std::uint32_t gridSize[],
	const Polyline& path,
	std::uint32_t& gridIndex,
	pcg32& rng) {
	switch(shape) {
		case ParticleEmitter::Shape::line:
			return generateLineSpawnPosition(
				scale.x,
				distribution,
				gridOrder,
				gridSize[0],
				gridIndex,
				rng);
		case ParticleEmitter::Shape::ellipse:
			return generateEllipseSpawnPosition(
				float2_t(scale),
				distribution,
				gridOrder,
				gridSize[0], gridSize[1],
				gridIndex,
				rng);
		case ParticleEmitter::Shape::rectangle:
			return generateRectangleSpawnPosition(
				float2_t(scale),
				distribution,
				gridOrder,
				gridSize[0], gridSize[1],
				gridIndex,
				rng);
		case ParticleEmitter::Shape::path:
			return generatePathSpawnPosition(
				scale,
				path,
				distribution,
				gridOrder,
				gridSize[0],
				gridIndex,
				rng);
		case ParticleEmitter::Shape::ellipsoid:
			return generateEllipsoidSpawnPosition(
				scale,
				distribution,
				gridOrder,
				gridSize[0], gridSize[1], gridSize[2],
				gridIndex,
				rng);
		case ParticleEmitter::Shape::cuboid:
			return generateCuboidSpawnPosition(
				scale,
				distribution,
				gridOrder,
				gridSize[0], gridSize[1], gridSize[2],
				gridIndex,
				rng);
		case ParticleEmitter::Shape::cylinder:
			return generateCylinderSpawnPosition(
				scale,
				distribution,
				gridOrder,
				gridSize[0], gridSize[1], gridSize[2],
				gridIndex,
				rng);
		default:
			return float3_t(0.0);
	}
}
}
