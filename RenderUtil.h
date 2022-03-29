#pragma once

#include "ParticleData.h"
#include "ParticleEmitter.h"
#include "Sprite.h"

namespace pixelpart {
struct ParticleMeshBuildInfo {
	const ParticleEmitter& emitter;
	const ParticleData& particles;
	uint32_t numParticles;
	uint32_t numIndices;
	uint32_t numVertices;
	std::vector<uint32_t> particleIndices;
	std::vector<uint32_t> numParticlesPerTrail;

	ParticleMeshBuildInfo(const ParticleEmitter& e, const ParticleData& p, uint32_t n);
};
struct ParticleTrailVertexData {
	std::vector<vec2d> positions;
	std::vector<vec2d> sizes;
	std::vector<vec4d> colors;
};

std::vector<vec2d> generateSpriteVertices(const vec2d& position, const vec2d& size, const vec2d& pivot, floatd rotation);
std::vector<vec2d> generateSpriteTextureCoords(int32_t frame, int32_t framesRow, int32_t framesColumn, TextureOrigin textureOrigin);
std::vector<ParticleTrailVertexData> generateParticleTrails(const ParticleMeshBuildInfo& meshBuildInfo);

template <typename IntT, typename FloatT>
void generateParticleSpriteTriangles(IntT* indices, FloatT* positions, FloatT* textureCoords, FloatT* colors, const ParticleMeshBuildInfo& meshBuildInfo, floatd scaleX, floatd scaleY) {
	if(indices) {
		for(uint32_t p = 0; p < meshBuildInfo.numParticles; p++) {
			indices[p * 6 + 0] = p * 4 + 0;
			indices[p * 6 + 1] = p * 4 + 1;
			indices[p * 6 + 2] = p * 4 + 3;
			indices[p * 6 + 3] = p * 4 + 1;
			indices[p * 6 + 4] = p * 4 + 2;
			indices[p * 6 + 5] = p * 4 + 3;
		}
	}

	if(positions) {
		for(uint32_t p = 0; p < meshBuildInfo.numParticles; p++) {
			std::vector<vec2d> pos = generateSpriteVertices(
				meshBuildInfo.particles.globalPosition[p],
				meshBuildInfo.particles.size[p],
				meshBuildInfo.emitter.particleSprite.texturePivot,
				meshBuildInfo.particles.rotation[p]);

			positions[p * 4 * 2 + 0] = pos[0].x * scaleX;
			positions[p * 4 * 2 + 1] = pos[0].y * scaleY;
			positions[p * 4 * 2 + 2] = pos[1].x * scaleX;
			positions[p * 4 * 2 + 3] = pos[1].y * scaleY;
			positions[p * 4 * 2 + 4] = pos[2].x * scaleX;
			positions[p * 4 * 2 + 5] = pos[2].y * scaleY;
			positions[p * 4 * 2 + 6] = pos[3].x * scaleX;
			positions[p * 4 * 2 + 7] = pos[3].y * scaleY;
		}
	}

	if(textureCoords) {
		for(uint32_t p = 0; p < meshBuildInfo.numParticles; p++) {
			std::vector<vec2d> uv = generateSpriteTextureCoords(
				meshBuildInfo.particles.frame[p],
				meshBuildInfo.emitter.particleSprite.framesRow,
				meshBuildInfo.emitter.particleSprite.framesColumn,
				meshBuildInfo.emitter.particleSprite.textureOrigin);

			textureCoords[p * 4 * 2 + 0] = uv[0].x;
			textureCoords[p * 4 * 2 + 1] = uv[0].y;
			textureCoords[p * 4 * 2 + 2] = uv[1].x;
			textureCoords[p * 4 * 2 + 3] = uv[1].y;
			textureCoords[p * 4 * 2 + 4] = uv[2].x;
			textureCoords[p * 4 * 2 + 5] = uv[2].y;
			textureCoords[p * 4 * 2 + 6] = uv[3].x;
			textureCoords[p * 4 * 2 + 7] = uv[3].y;
		}
	}

	if(colors) {
		for(uint32_t p = 0; p < meshBuildInfo.numParticles; p++) {
			colors[p * 4 * 4 + 0] = meshBuildInfo.particles.color[p].r;
			colors[p * 4 * 4 + 1] = meshBuildInfo.particles.color[p].g;
			colors[p * 4 * 4 + 2] = meshBuildInfo.particles.color[p].b;
			colors[p * 4 * 4 + 3] = meshBuildInfo.particles.color[p].a;
			colors[p * 4 * 4 + 4] = meshBuildInfo.particles.color[p].r;
			colors[p * 4 * 4 + 5] = meshBuildInfo.particles.color[p].g;
			colors[p * 4 * 4 + 6] = meshBuildInfo.particles.color[p].b;
			colors[p * 4 * 4 + 7] = meshBuildInfo.particles.color[p].a;
			colors[p * 4 * 4 + 8] = meshBuildInfo.particles.color[p].r;
			colors[p * 4 * 4 + 9] = meshBuildInfo.particles.color[p].g;
			colors[p * 4 * 4 + 10] = meshBuildInfo.particles.color[p].b;
			colors[p * 4 * 4 + 11] = meshBuildInfo.particles.color[p].a;
			colors[p * 4 * 4 + 12] = meshBuildInfo.particles.color[p].r;
			colors[p * 4 * 4 + 13] = meshBuildInfo.particles.color[p].g;
			colors[p * 4 * 4 + 14] = meshBuildInfo.particles.color[p].b;
			colors[p * 4 * 4 + 15] = meshBuildInfo.particles.color[p].a;
		}
	}
}

template <typename IntT, typename FloatT>
void generateParticleTrailTriangles(IntT* indices, FloatT* positions, FloatT* textureCoords, FloatT* colors, const ParticleMeshBuildInfo& meshBuildInfo, floatd scaleX, floatd scaleY) {
	std::vector<ParticleTrailVertexData> trails = generateParticleTrails(meshBuildInfo);

	uint32_t i0 = 0;
	uint32_t v0 = 0;

	for(const ParticleTrailVertexData& trail : trails) {
		const uint32_t numVertices = static_cast<uint32_t>(trail.positions.size());
		if(numVertices < 2) {
			continue;
		}

		if(indices) {
			for(uint32_t i = 0; i < numVertices - 1; i++) {
				indices[i0 + i * 6 + 0] = v0 + i * 2 + 0;
				indices[i0 + i * 6 + 1] = v0 + i * 2 + 2;
				indices[i0 + i * 6 + 2] = v0 + i * 2 + 1;
				indices[i0 + i * 6 + 3] = v0 + i * 2 + 2;
				indices[i0 + i * 6 + 4] = v0 + i * 2 + 3;
				indices[i0 + i * 6 + 5] = v0 + i * 2 + 1;
			}
		}

		if(positions) {
			for(uint32_t i = 0; i < numVertices - 1; i++) {
				vec2d d = glm::normalize(trail.positions[i + 1] - trail.positions[i]);
				positions[(v0 + i * 2) * 2 + 0] = (trail.positions[i].x + d.y * std::max(trail.sizes[i].x, trail.sizes[i].y) * 0.5) * scaleX;
				positions[(v0 + i * 2) * 2 + 1] = (trail.positions[i].y - d.x * std::max(trail.sizes[i].x, trail.sizes[i].y) * 0.5) * scaleY;
				positions[(v0 + i * 2) * 2 + 2] = (trail.positions[i].x - d.y * std::max(trail.sizes[i].x, trail.sizes[i].y) * 0.5) * scaleX;
				positions[(v0 + i * 2) * 2 + 3] = (trail.positions[i].y + d.x * std::max(trail.sizes[i].x, trail.sizes[i].y) * 0.5) * scaleY;
			}

			vec2d d = glm::normalize(trail.positions[numVertices - 2] - trail.positions[numVertices - 1]);
			positions[(v0 + (numVertices - 1) * 2) * 2 + 0] = (trail.positions[numVertices - 1].x - d.y * std::max(trail.sizes[numVertices - 1].x, trail.sizes[numVertices - 1].y) * 0.5) * scaleX;
			positions[(v0 + (numVertices - 1) * 2) * 2 + 1] = (trail.positions[numVertices - 1].y + d.x * std::max(trail.sizes[numVertices - 1].x, trail.sizes[numVertices - 1].y) * 0.5) * scaleY;
			positions[(v0 + (numVertices - 1) * 2) * 2 + 2] = (trail.positions[numVertices - 1].x + d.y * std::max(trail.sizes[numVertices - 1].x, trail.sizes[numVertices - 1].y) * 0.5) * scaleX;
			positions[(v0 + (numVertices - 1) * 2) * 2 + 3] = (trail.positions[numVertices - 1].y - d.x * std::max(trail.sizes[numVertices - 1].x, trail.sizes[numVertices - 1].y) * 0.5) * scaleY;
		}

		if(textureCoords) {
			floatd trailLengthTotal = 0.0;
			std::vector<floatd> trailLength(numVertices);
			for(uint32_t i = 0; i < numVertices - 1; i++) {
				trailLength[i] = trailLengthTotal;
				trailLengthTotal += glm::length(trail.positions[i + 1] - trail.positions[i]);
			}

			trailLength.back() = trailLengthTotal;

			switch(meshBuildInfo.emitter.rendererTrailUVRotation) {
				case 1:
					for(uint32_t i = 0; i < numVertices; i++) {
						textureCoords[(v0 + i * 2) * 2 + 0] = trailLength[i] / trailLengthTotal * meshBuildInfo.emitter.rendererTrailUVFactor;
						textureCoords[(v0 + i * 2) * 2 + 1] = 0.0;
						textureCoords[(v0 + i * 2) * 2 + 2] = trailLength[i] / trailLengthTotal * meshBuildInfo.emitter.rendererTrailUVFactor;
						textureCoords[(v0 + i * 2) * 2 + 3] = 1.0;
					}
					break;

				case 2:
					for(uint32_t i = 0; i < numVertices; i++) {
						textureCoords[(v0 + i * 2) * 2 + 0] = 0.0;
						textureCoords[(v0 + i * 2) * 2 + 1] = meshBuildInfo.emitter.rendererTrailUVFactor - trailLength[i] / trailLengthTotal * meshBuildInfo.emitter.rendererTrailUVFactor;
						textureCoords[(v0 + i * 2) * 2 + 2] = 1.0;
						textureCoords[(v0 + i * 2) * 2 + 3] = meshBuildInfo.emitter.rendererTrailUVFactor - trailLength[i] / trailLengthTotal * meshBuildInfo.emitter.rendererTrailUVFactor;
					}
					break;

				case 3:
					for(uint32_t i = 0; i < numVertices; i++) {
						textureCoords[(v0 + i * 2) * 2 + 0] = trailLength[i] / trailLengthTotal * meshBuildInfo.emitter.rendererTrailUVFactor;
						textureCoords[(v0 + i * 2) * 2 + 1] = 1.0;
						textureCoords[(v0 + i * 2) * 2 + 2] = trailLength[i] / trailLengthTotal * meshBuildInfo.emitter.rendererTrailUVFactor;
						textureCoords[(v0 + i * 2) * 2 + 3] = 0.0;
					}
					break;
				
				default:
					for(uint32_t i = 0; i < numVertices; i++) {
						textureCoords[(v0 + i * 2) * 2 + 0] = 0.0;
						textureCoords[(v0 + i * 2) * 2 + 1] = trailLength[i] / trailLengthTotal * meshBuildInfo.emitter.rendererTrailUVFactor;
						textureCoords[(v0 + i * 2) * 2 + 2] = 1.0;
						textureCoords[(v0 + i * 2) * 2 + 3] = trailLength[i] / trailLengthTotal * meshBuildInfo.emitter.rendererTrailUVFactor;
					}
					break;
			}
		}

		if(colors) {
			for(uint32_t i = 0; i < numVertices; i++) {
				colors[(v0 + i * 2) * 4 + 0] = trail.colors[i].r;
				colors[(v0 + i * 2) * 4 + 1] = trail.colors[i].g;
				colors[(v0 + i * 2) * 4 + 2] = trail.colors[i].b;
				colors[(v0 + i * 2) * 4 + 3] = trail.colors[i].a;
				colors[(v0 + i * 2) * 4 + 4] = trail.colors[i].r;
				colors[(v0 + i * 2) * 4 + 5] = trail.colors[i].g;
				colors[(v0 + i * 2) * 4 + 6] = trail.colors[i].b;
				colors[(v0 + i * 2) * 4 + 7] = trail.colors[i].a;
			}
		}

		i0 += (numVertices - 1) * 6;
		v0 += numVertices * 2;
	}
}

template <typename IntT, typename FloatT>
void generateSpriteTriangles(IntT* indices, FloatT* positions, FloatT* textureCoords, FloatT* colors, const Sprite& sprite, floatd aspectRatio, floatd time, floatd scaleX, floatd scaleY) {
	const floatd alpha = std::fmod(time - sprite.lifetimeStart, sprite.lifetimeDuration) / sprite.lifetimeDuration;
	const vec4d color = vec4d(vec3d(sprite.color.get(alpha)), sprite.opacity.get(alpha));
	const vec2d position = sprite.motionPath.get(alpha);
	const vec2d nextPosition = sprite.motionPath.get(alpha + 0.01);
	const floatd orientation = sprite.orientation.get(alpha) + ((sprite.alignWithPath && position != nextPosition) ? glm::degrees(glm::orientedAngle(vec2d(0.0, 1.0), glm::normalize(nextPosition - position))) : 0.0);
	const vec2d size = vec2d(sprite.width.get(alpha) * aspectRatio, sprite.height.get(alpha));
	const int32_t frameAdvance = static_cast<int32_t>((time - sprite.lifetimeStart) / sprite.imageAnimation.duration * static_cast<floatd>(sprite.imageAnimation.frames) + 0.5);
	const int32_t frame = sprite.image.frame + sprite.imageAnimation.loop
		? frameAdvance % static_cast<int32_t>(sprite.imageAnimation.frames)
		: std::min(frameAdvance, static_cast<int32_t>(sprite.imageAnimation.frames) - 1);

	std::vector<vec2d> pos = generateSpriteVertices(
		position,
		size,
		sprite.image.texturePivot,
		orientation);
	std::vector<vec2d> uv = generateSpriteTextureCoords(
		frame,
		sprite.image.framesRow,
		sprite.image.framesColumn,
		sprite.image.textureOrigin);

	if(indices) {
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 3;
		indices[3] = 1;
		indices[4] = 2;
		indices[5] = 3;
	}

	if(positions) {
		positions[0] = pos[0].x * scaleX;
		positions[1] = pos[0].y * scaleY;
		positions[2] = pos[1].x * scaleX;
		positions[3] = pos[1].y * scaleY;
		positions[4] = pos[2].x * scaleX;
		positions[5] = pos[2].y * scaleY;
		positions[6] = pos[3].x * scaleX;
		positions[7] = pos[3].y * scaleY;
	}

	if(textureCoords) {
		textureCoords[0] = uv[0].x;
		textureCoords[1] = uv[0].y;
		textureCoords[2] = uv[1].x;
		textureCoords[3] = uv[1].y;
		textureCoords[4] = uv[2].x;
		textureCoords[5] = uv[2].y;
		textureCoords[6] = uv[3].x;
		textureCoords[7] = uv[3].y;
	}

	if(colors) {
		colors[0] = color.r;
		colors[1] = color.g;
		colors[2] = color.b;
		colors[3] = color.a;
		colors[4] = color.r;
		colors[5] = color.g;
		colors[6] = color.b;
		colors[7] = color.a;
		colors[8] = color.r;
		colors[9] = color.g;
		colors[10] = color.b;
		colors[11] = color.a;
		colors[12] = color.r;
		colors[13] = color.g;
		colors[14] = color.b;
		colors[15] = color.a;
	}
}
}