#include "RenderUtil.h"

namespace pixelpart {
vec2d rotate(const vec2d& p, const vec2d& o, floatd a) {
	floatd s = std::sin(glm::radians(a));
	floatd c = std::cos(glm::radians(a));

	return vec2d(
		(p.x - o.x) * c - (p.y - o.y) * s + o.x,
		(p.x - o.x) * s + (p.y - o.y) * c + o.y);
}

ParticleMeshBuildInfo::ParticleMeshBuildInfo(const ParticleEmitter& e, const ParticleData& p, uint32_t n) : emitter(e), particles(p), numParticles(n) {
	if(emitter.renderer == ParticleEmitter::RendererType::sprite || numParticles <= 1) {
		numIndices = numParticles * 6;
		numVertices = numParticles * 4;
	}
	else if(emitter.renderer == ParticleEmitter::RendererType::trail) {
		particleIndices.resize(numParticles);
		std::iota(particleIndices.begin(), particleIndices.end(), 0);
		std::sort(particleIndices.begin(), particleIndices.end(), [this](uint32_t p0, uint32_t p1) {
			return (particles.parentId[p0] != particles.parentId[p1])
				? particles.parentId[p0] > particles.parentId[p1]
				: particles.spawnId[p0] > particles.spawnId[p1];
		});

		for(uint32_t p = 0, parentId = 0; p < numParticles; p++) {
			if(particles.parentId[particleIndices[p]] != parentId || p == 0) {
				parentId = particles.parentId[particleIndices[p]];
				numParticlesPerTrail.push_back(0);
			}

			numParticlesPerTrail.back()++;
		}

		if(emitter.rendererNumTrailSegments > 0) {
			numIndices = static_cast<uint32_t>(numParticlesPerTrail.size()) * (emitter.rendererNumTrailSegments - 1) * 6;
			numVertices = static_cast<uint32_t>(numParticlesPerTrail.size()) * emitter.rendererNumTrailSegments * 2;
		}
		else {
			for(uint32_t n : numParticlesPerTrail) {
				if(n < 2) {
					continue;
				}

				numIndices += (n - 1) * 6;
				numVertices += n * 2;
			}
		}
	}
}

SpriteMeshBuildInfo::SpriteMeshBuildInfo(const Sprite& s, const ImageResource& r, floatd t) : sprite(s), imageResource(r), time(t) {
	aspectRatio = static_cast<floatd>(imageResource.width) / static_cast<floatd>(imageResource.height);
	numIndices = 6;
	numVertices = 4;
}

std::vector<vec2d> generateSpriteVertices(const vec2d& position, const vec2d& size, const vec2d& pivot, floatd rotation) {
	return std::vector<vec2d>{
		position + rotate(vec2d(-0.5, -0.5) * size, pivot * size, rotation),
		position + rotate(vec2d(+0.5, -0.5) * size, pivot * size, rotation),
		position + rotate(vec2d(+0.5, +0.5) * size, pivot * size, rotation),
		position + rotate(vec2d(-0.5, +0.5) * size, pivot * size, rotation)
	};
}
std::vector<vec2d> generateSpriteTextureCoords(int32_t frame, int32_t framesRow, int32_t framesColumn, TextureOrigin textureOrigin) {
	vec2d sz = vec2d(
		1.0 / static_cast<floatd>(framesRow),
		1.0 / static_cast<floatd>(framesColumn));
	vec2d uv = vec2d(
		static_cast<floatd>(frame % framesRow) * sz.x,
		static_cast<floatd>(frame / framesRow) * sz.y);

	switch(textureOrigin) {
		case TextureOrigin::bottom_right:
			uv.x = 1.0 - static_cast<floatd>(frame % framesRow) * sz.x - sz.x;
			break;
		case TextureOrigin::top_left:
			uv.y = 1.0 - static_cast<floatd>(frame / framesRow) * sz.y - sz.y;
			break;
		case TextureOrigin::top_right:
			uv.x = 1.0 - static_cast<floatd>(frame % framesRow) * sz.x - sz.x;
			uv.y = 1.0 - static_cast<floatd>(frame / framesRow) * sz.y - sz.y;
			break;
		default:
			break;
	}

	return std::vector<vec2d>{
		vec2d(uv.x, uv.y),
		vec2d(uv.x + sz.x, uv.y),
		vec2d(uv.x + sz.x, uv.y + sz.y),
		vec2d(uv.x, uv.y + sz.y)
	};
}
std::vector<ParticleTrailVertexData> generateParticleTrails(const ParticleMeshBuildInfo& meshBuildInfo) {
	std::vector<ParticleTrailVertexData> trails;

	if(meshBuildInfo.emitter.rendererNumTrailSegments > 0) {
		for(uint32_t p = 0, parentParticleId = 0; p < meshBuildInfo.numParticles; p++) {
			if(meshBuildInfo.particles.parentId[meshBuildInfo.particleIndices[p]] != parentParticleId || p == 0) {
				const uint32_t trailIndex = static_cast<uint32_t>(trails.size());
				trails.push_back(ParticleTrailVertexData());

				floatd trailLengthTotal = 0.0;
				std::vector<floatd> trailLength(meshBuildInfo.numParticlesPerTrail[trailIndex]);
				for(uint32_t pTrail = 0; pTrail < meshBuildInfo.numParticlesPerTrail[trailIndex] - 1; pTrail++) {
					trailLength[pTrail] = trailLengthTotal;
					trailLengthTotal += glm::length(meshBuildInfo.particles.globalPosition[meshBuildInfo.particleIndices[p + pTrail + 1]] - meshBuildInfo.particles.globalPosition[meshBuildInfo.particleIndices[p + pTrail]]);
				}

				trailLength.back() = trailLengthTotal;

				for(uint32_t pTrail = 0; pTrail < meshBuildInfo.numParticlesPerTrail[trailIndex]; pTrail++) {
					trailLength[pTrail] /= trailLengthTotal;
				}

				Curve<vec2d> curvePosition(CurveInterpolation::spline_catmullrom);
				Curve<vec2d> curveSize(CurveInterpolation::spline_catmullrom);
				Curve<vec4d> curveColor(CurveInterpolation::spline_catmullrom);

				curvePosition.enableFixedCache(meshBuildInfo.emitter.rendererNumTrailSegments);
				curvePosition.setPointsOrdered<uint32_t>(trailLength.data(), meshBuildInfo.particles.globalPosition.data(), meshBuildInfo.particleIndices.data() + p, meshBuildInfo.numParticlesPerTrail[trailIndex]);
				curveSize.enableFixedCache(meshBuildInfo.emitter.rendererNumTrailSegments);
				curveSize.setPointsOrdered<uint32_t>(trailLength.data(), meshBuildInfo.particles.size.data(), meshBuildInfo.particleIndices.data() + p, meshBuildInfo.numParticlesPerTrail[trailIndex]);
				curveColor.enableFixedCache(meshBuildInfo.emitter.rendererNumTrailSegments);
				curveColor.setPointsOrdered<uint32_t>(trailLength.data(), meshBuildInfo.particles.color.data(), meshBuildInfo.particleIndices.data() + p, meshBuildInfo.numParticlesPerTrail[trailIndex]);

				trails.back().positions = curvePosition.getCache();
				trails.back().sizes = curveSize.getCache();
				trails.back().colors = curveColor.getCache();

				parentParticleId = meshBuildInfo.particles.parentId[meshBuildInfo.particleIndices[p]];
			}
		}
	}
	else {
		for(uint32_t p = 0, pTrail = 0, parentParticleId = 0; p < meshBuildInfo.numParticles; p++, pTrail++) {
			if(meshBuildInfo.particles.parentId[meshBuildInfo.particleIndices[p]] != parentParticleId || p == 0) {
				const uint32_t trailIndex = static_cast<uint32_t>(trails.size());
				trails.push_back(ParticleTrailVertexData());
				trails.back().positions.resize(meshBuildInfo.numParticlesPerTrail[trailIndex]);
				trails.back().sizes.resize(meshBuildInfo.numParticlesPerTrail[trailIndex]);
				trails.back().colors.resize(meshBuildInfo.numParticlesPerTrail[trailIndex]);
				parentParticleId = meshBuildInfo.particles.parentId[meshBuildInfo.particleIndices[p]];
				pTrail = 0;
			}

			ParticleTrailVertexData& trail = trails.back();
			trail.positions[pTrail] = meshBuildInfo.particles.globalPosition[meshBuildInfo.particleIndices[p]];
			trail.sizes[pTrail] = meshBuildInfo.particles.size[meshBuildInfo.particleIndices[p]];
			trail.colors[pTrail] = meshBuildInfo.particles.color[meshBuildInfo.particleIndices[p]];
		}
	}

	return trails;
}
}