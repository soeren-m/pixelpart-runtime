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

ParticleMeshBuilder::ParticleMeshBuilder() {

}
ParticleMeshBuilder::ParticleMeshBuilder(const ParticleEmitter& particleEmitter, const ParticleData& particleData, uint32_t num, floatd t) {
	update(particleEmitter, particleData, num, t);
}

void ParticleMeshBuilder::update(const ParticleEmitter& particleEmitter, const ParticleData& particleData, uint32_t num, floatd t) {
	emitter = &particleEmitter;
	particles = &particleData;
	numParticles = num;

	if(emitter->renderer == ParticleEmitter::RendererType::sprite || numParticles <= 1) {
		numIndices = numParticles * 6;
		numVertices = numParticles * 4;
	}
	else if(emitter->renderer == ParticleEmitter::RendererType::trail) {
		numParticlesPerTrail.clear();
		sortedParticleIndices.resize(numParticles);
		std::iota(sortedParticleIndices.begin(), sortedParticleIndices.end(), 0);
		std::sort(sortedParticleIndices.begin(), sortedParticleIndices.end(), [this](uint32_t p0, uint32_t p1) {
			return (particles->parentId[p0] != particles->parentId[p1])
				? particles->parentId[p0] > particles->parentId[p1]
				: particles->spawnId[p0] > particles->spawnId[p1];
		});

		for(uint32_t p = 0, parentId = 0; p < numParticles; p++) {
			if(particles->parentId[sortedParticleIndices[p]] != parentId || p == 0) {
				parentId = particles->parentId[sortedParticleIndices[p]];

				numParticlesPerTrail.push_back(0);

				if(trailAnimation.count(parentId) == 0) {
					TrailAnimationData animation;
					animation.startTime = t;
					animation.initialFrame = particles->initialFrame[sortedParticleIndices[p]];
					animation.frame = animation.initialFrame;
					trailAnimation[parentId] = animation;
				}
				else {
					TrailAnimationData& animation = trailAnimation[parentId];
					animation.frame = animation.initialFrame;
					animation.frame += emitter->particleSpriteAnimation.loop
							? static_cast<int32_t>((t - animation.startTime) / emitter->particleSpriteAnimation.duration * static_cast<floatd>(emitter->particleSpriteAnimation.frames)) % emitter->particleSpriteAnimation.frames
							: std::min(static_cast<int32_t>((t - animation.startTime) / emitter->particleSpriteAnimation.duration * static_cast<floatd>(emitter->particleSpriteAnimation.frames)), static_cast<int32_t>(emitter->particleSpriteAnimation.frames) - 1);
					animation.frame %= static_cast<int32_t>(emitter->particleSprite.framesRow * emitter->particleSprite.framesColumn);
				}
			}

			numParticlesPerTrail.back()++;
		}

		numIndices = 0;
		numVertices = 0;

		for(uint32_t numTrailParticles : numParticlesPerTrail) {
			uint32_t sections = static_cast<uint32_t>(emitter->rendererTrailUVFactor - 0.000001);

			if(emitter->rendererNumTrailSegments > 0) {
				numIndices += (emitter->rendererNumTrailSegments + sections - 1) * 6;
				numVertices += (emitter->rendererNumTrailSegments + sections) * 2;
			}
			else if(numTrailParticles > 1) {
				numIndices += (numTrailParticles + sections - 1) * 6;
				numVertices += (numTrailParticles + sections) * 2;
			}
		}
	}
}

const ParticleEmitter* ParticleMeshBuilder::getEmitter() const {
	return emitter;
}
const ParticleData* ParticleMeshBuilder::getParticles() const {
	return particles;
}
uint32_t ParticleMeshBuilder::getNumParticles() const {
	return numParticles;
}
uint32_t ParticleMeshBuilder::getNumIndices() const {
	return numIndices;
}
uint32_t ParticleMeshBuilder::getNumVertices() const {
	return numVertices;
}

std::vector<ParticleMeshBuilder::TrailVertexData> ParticleMeshBuilder::generateTrails() const {
	std::vector<TrailVertexData> trails;

	if(emitter->rendererNumTrailSegments > 0) {
		for(uint32_t p = 0, parentParticleId = 0; p < numParticles; p++) {
			if(particles->parentId[sortedParticleIndices[p]] != parentParticleId || p == 0) {
				parentParticleId = particles->parentId[sortedParticleIndices[p]];

				floatd trailLengthTotal = 0.0;
				std::vector<floatd> trailLength(numParticlesPerTrail[trails.size()]);

				trailLength[0] = 0.0;
				for(uint32_t pTrail = 1; pTrail < numParticlesPerTrail[trails.size()]; pTrail++) {
					trailLengthTotal += glm::length(particles->globalPosition[sortedParticleIndices[p + pTrail]] - particles->globalPosition[sortedParticleIndices[p + pTrail - 1]]);
					trailLength[pTrail] = trailLengthTotal;
				}
				for(uint32_t pTrail = 0; pTrail < numParticlesPerTrail[trails.size()]; pTrail++) {
					trailLength[pTrail] /= trailLengthTotal;
				}

				Curve<vec2d> curvePosition(CurveInterpolation::spline_catmullrom);
				Curve<vec2d> curveSize(CurveInterpolation::spline_catmullrom);
				Curve<vec4d> curveColor(CurveInterpolation::spline_catmullrom);

				curvePosition.enableFixedCache(emitter->rendererNumTrailSegments);
				curvePosition.setPointsOrdered<uint32_t>(trailLength.data(), particles->globalPosition.data(), sortedParticleIndices.data() + p, numParticlesPerTrail[trails.size()]);
				curveSize.enableFixedCache(emitter->rendererNumTrailSegments);
				curveSize.setPointsOrdered<uint32_t>(trailLength.data(), particles->size.data(), sortedParticleIndices.data() + p, numParticlesPerTrail[trails.size()]);
				curveColor.enableFixedCache(emitter->rendererNumTrailSegments);
				curveColor.setPointsOrdered<uint32_t>(trailLength.data(), particles->color.data(), sortedParticleIndices.data() + p, numParticlesPerTrail[trails.size()]);

				trails.emplace_back();
				trails.back().positions = curvePosition.getCache();
				trails.back().sizes = curveSize.getCache();
				trails.back().colors = curveColor.getCache();
				trails.back().frame = trailAnimation.at(parentParticleId).frame;
			}
		}
	}
	else {
		for(uint32_t p = 0, pTrail = 0, parentParticleId = 0; p < numParticles; p++, pTrail++) {
			if(particles->parentId[sortedParticleIndices[p]] != parentParticleId || p == 0) {
				parentParticleId = particles->parentId[sortedParticleIndices[p]];
				pTrail = 0;

				trails.emplace_back();
				trails.back().positions.resize(numParticlesPerTrail[trails.size() - 1]);
				trails.back().sizes.resize(numParticlesPerTrail[trails.size() - 1]);
				trails.back().colors.resize(numParticlesPerTrail[trails.size() - 1]);
				trails.back().frame = trailAnimation.at(parentParticleId).frame;
			}

			trails.back().positions[pTrail] = particles->globalPosition[sortedParticleIndices[p]];
			trails.back().sizes[pTrail] = particles->size[sortedParticleIndices[p]];
			trails.back().colors[pTrail] = particles->color[sortedParticleIndices[p]];
		}
	}

	return trails;
}

SpriteMeshBuilder::SpriteMeshBuilder() {

}
SpriteMeshBuilder::SpriteMeshBuilder(const Sprite& spriteRef, uint32_t width, uint32_t height, floatd t) {
	sprite = &spriteRef;
	aspectRatio = static_cast<floatd>(width) / static_cast<floatd>(height);
	time = t;
}
SpriteMeshBuilder::SpriteMeshBuilder(const Sprite& spriteRef, const ImageResource& imageResource, floatd t) {
	sprite = &spriteRef;
	aspectRatio = static_cast<floatd>(imageResource.width) / static_cast<floatd>(imageResource.height);
	time = t;
}

void SpriteMeshBuilder::prepare(vec2d& position, vec2d& size, floatd& orientation, vec4d& color, int32_t& frame) const {
	const floatd alpha = std::fmod(time - sprite->lifetimeStart, sprite->lifetimeDuration) / sprite->lifetimeDuration;

	position = sprite->motionPath.get(alpha);
	size = vec2d(
		sprite->width.get(alpha) * aspectRatio,
		sprite->height.get(alpha));
	orientation = sprite->orientation.get(alpha) + ((sprite->alignWithPath && position != sprite->motionPath.get(alpha + 0.01))
		? glm::degrees(glm::orientedAngle(vec2d(0.0, 1.0), glm::normalize(sprite->motionPath.get(alpha + 0.01) - position)))
		: 0.0);
	color = vec4d(
		vec3d(sprite->color.get(alpha)),
		sprite->opacity.get(alpha));
	frame = sprite->image.frame + sprite->imageAnimation.loop
		? static_cast<int32_t>((time - sprite->lifetimeStart) / sprite->imageAnimation.duration * static_cast<floatd>(sprite->imageAnimation.frames) + 0.5) % static_cast<int32_t>(sprite->imageAnimation.frames)
		: std::min(static_cast<int32_t>((time - sprite->lifetimeStart) / sprite->imageAnimation.duration * static_cast<floatd>(sprite->imageAnimation.frames) + 0.5), static_cast<int32_t>(sprite->imageAnimation.frames) - 1);
}

const Sprite* SpriteMeshBuilder::getSprite() const {
	return sprite;
}
uint32_t SpriteMeshBuilder::getNumIndices() const {
	return 6;
}
uint32_t SpriteMeshBuilder::getNumVertices() const {
	return 4;
}
}