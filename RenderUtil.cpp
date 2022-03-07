#include "RenderUtil.h"

namespace pixelpart {
vec2d rotate(const vec2d& p, const vec2d& o, floatd a) {
	floatd s = std::sin(glm::radians(a));
	floatd c = std::cos(glm::radians(a));

	return vec2d(
		(p.x - o.x) * c - (p.y - o.y) * s + o.x,
		(p.x - o.x) * s + (p.y - o.y) * c + o.y);
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
void generateParticleTrail(std::vector<vec2d>& trailPosition, std::vector<vec2d>& trailSize, std::vector<vec4d>& trailColor, const ParticleEmitter& emitter, const ParticleData& particles, uint32_t numParticles) {
	std::vector<uint32_t> particleIndices(numParticles);
	std::iota(particleIndices.begin(), particleIndices.end(), 0);
	std::sort(particleIndices.begin(), particleIndices.end(), [&particles](uint32_t p0, uint32_t p1) {
		return particles.spawnId[p0] > particles.spawnId[p1];
	});
	
	if(emitter.rendererNumTrailSegments > 0) {
		floatd trailLengthTotal = 0.0;
		std::vector<floatd> trailLength(numParticles);
		for(uint32_t p = 0; p < numParticles - 1; p++) {
			trailLength[p] = trailLengthTotal;
			trailLengthTotal += glm::length(particles.globalPosition[particleIndices[p + 1]] - particles.globalPosition[particleIndices[p]]);			
		}

		trailLength.back() = trailLengthTotal;

		for(uint32_t p = 0; p < numParticles; p++) {
			trailLength[p] /= trailLengthTotal;
		}

		Curve<vec2d> curvePosition(CurveInterpolation::spline_catmullrom);
		Curve<vec2d> curveSize(CurveInterpolation::spline_catmullrom);
		Curve<vec4d> curveColor(CurveInterpolation::spline_catmullrom);

		curvePosition.enableFixedCache(emitter.rendererNumTrailSegments);
		curvePosition.setPointsOrdered<uint32_t>(trailLength.data(), particles.globalPosition.data(), particleIndices.data(), numParticles);	
		curveSize.enableFixedCache(emitter.rendererNumTrailSegments);
		curveSize.setPointsOrdered<uint32_t>(trailLength.data(), particles.size.data(), particleIndices.data(), numParticles);	
		curveColor.enableFixedCache(emitter.rendererNumTrailSegments);
		curveColor.setPointsOrdered<uint32_t>(trailLength.data(), particles.color.data(), particleIndices.data(), numParticles);

		trailPosition = curvePosition.getCache();
		trailSize = curveSize.getCache();
		trailColor = curveColor.getCache();
	}
	else {
		trailPosition.resize(numParticles);
		trailSize.resize(numParticles);
		trailColor.resize(numParticles);

		for(uint32_t p = 0; p < numParticles; p++) {
			trailPosition[p] = particles.globalPosition[particleIndices[p]];
			trailSize[p] = particles.size[particleIndices[p]];
			trailColor[p] = particles.color[particleIndices[p]];
		}
	}
}

uint32_t getNumParticleIndices(const ParticleEmitter& emitter, uint32_t numParticles) {
	if(emitter.renderer == pixelpart::ParticleEmitter::RendererType::sprite) {
		return numParticles * 6;
	}
	else if(emitter.renderer == pixelpart::ParticleEmitter::RendererType::trail) {
		if(emitter.rendererNumTrailSegments > 0) {
			return (emitter.rendererNumTrailSegments - 1) * 6;
		}
		else {
			return (numParticles - 1) * 6;
		}
	}

	return 0;
}
uint32_t getNumParticleVertices(const ParticleEmitter& emitter, uint32_t numParticles) {
	if(emitter.renderer == pixelpart::ParticleEmitter::RendererType::sprite) {
		return numParticles * 4;
	}
	else if(emitter.renderer == pixelpart::ParticleEmitter::RendererType::trail) {
		if(emitter.rendererNumTrailSegments > 0) {
			return emitter.rendererNumTrailSegments * 2;
		}
		else {
			return numParticles * 2;
		}
	}

	return 0;
}
uint32_t getNumSpriteIndices(const Sprite& sprite) {
	return 6;
}
uint32_t getNumSpriteVertices(const Sprite& sprite) {
	return 4;
}
}