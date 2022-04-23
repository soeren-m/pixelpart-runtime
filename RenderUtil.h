#pragma once

#include "ParticleData.h"
#include "ParticleEmitter.h"
#include "Sprite.h"
#include "ImageResource.h"
#include <unordered_map>

namespace pixelpart {
std::vector<vec2d> generateSpriteVertices(const vec2d& position, const vec2d& size, const vec2d& pivot, floatd rotation);
std::vector<vec2d> generateSpriteTextureCoords(int32_t frame, int32_t framesRow, int32_t framesColumn, TextureOrigin textureOrigin);

class ParticleMeshBuilder {
public:
	ParticleMeshBuilder();
	ParticleMeshBuilder(const ParticleEmitter& particleEmitter, const ParticleData& particleData, uint32_t num, floatd t);

	template <typename IntT, typename FloatT>
	void build(IntT* indices, FloatT* positions, FloatT* textureCoords, FloatT* colors, floatd scaleX, floatd scaleY) const {
		if(!emitter) {
			return;
		}

		if(emitter->renderer == ParticleEmitter::RendererType::trail && numParticles > 1) {
			buildTrails(indices, positions, textureCoords, colors, scaleX, scaleY);
		}
		else {
			buildSprites(indices, positions, textureCoords, colors, scaleX, scaleY);
		}
	}

	template <typename IntT, typename FloatT>
	void buildSprites(IntT* indices, FloatT* positions, FloatT* textureCoords, FloatT* colors, floatd scaleX, floatd scaleY) const {
		if(!emitter || !particles) {
			return;
		}

		if(indices) {
			for(uint32_t p = 0; p < numParticles; p++) {
				indices[p * 6 + 0] = p * 4 + 0;
				indices[p * 6 + 1] = p * 4 + 1;
				indices[p * 6 + 2] = p * 4 + 3;
				indices[p * 6 + 3] = p * 4 + 1;
				indices[p * 6 + 4] = p * 4 + 2;
				indices[p * 6 + 5] = p * 4 + 3;
			}
		}

		if(positions) {
			for(uint32_t p = 0; p < numParticles; p++) {
				std::vector<vec2d> pos = generateSpriteVertices(
					particles->globalPosition[p],
					particles->size[p],
					emitter->particleSprite.texturePivot,
					particles->rotation[p]);

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
			for(uint32_t p = 0; p < numParticles; p++) {
				std::vector<vec2d> uv = generateSpriteTextureCoords(
					particles->frame[p],
					emitter->particleSprite.framesRow,
					emitter->particleSprite.framesColumn,
					emitter->particleSprite.textureOrigin);

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
			for(uint32_t p = 0; p < numParticles; p++) {
				colors[p * 4 * 4 + 0] = particles->color[p].r;
				colors[p * 4 * 4 + 1] = particles->color[p].g;
				colors[p * 4 * 4 + 2] = particles->color[p].b;
				colors[p * 4 * 4 + 3] = particles->color[p].a;
				colors[p * 4 * 4 + 4] = particles->color[p].r;
				colors[p * 4 * 4 + 5] = particles->color[p].g;
				colors[p * 4 * 4 + 6] = particles->color[p].b;
				colors[p * 4 * 4 + 7] = particles->color[p].a;
				colors[p * 4 * 4 + 8] = particles->color[p].r;
				colors[p * 4 * 4 + 9] = particles->color[p].g;
				colors[p * 4 * 4 + 10] = particles->color[p].b;
				colors[p * 4 * 4 + 11] = particles->color[p].a;
				colors[p * 4 * 4 + 12] = particles->color[p].r;
				colors[p * 4 * 4 + 13] = particles->color[p].g;
				colors[p * 4 * 4 + 14] = particles->color[p].b;
				colors[p * 4 * 4 + 15] = particles->color[p].a;
			}
		}
	}

	template <typename IntT, typename FloatT>
	void buildTrails(IntT* indices, FloatT* positions, FloatT* textureCoords, FloatT* colors, floatd scaleX, floatd scaleY) const {
		if(!emitter || !particles) {
			return;
		}

		std::vector<TrailVertexData> trails = generateTrails();
		uint32_t i0 = 0;
		uint32_t v0 = 0;

		for(const TrailVertexData& trail : trails) {
			uint32_t numTrailPoints = static_cast<uint32_t>(trail.positions.size());
			if(numTrailPoints < 2) {
				continue;
			}

			std::vector<floatd> trailLength(numTrailPoints);
			floatd trailLengthTotal = 0.0;
			trailLength[0] = 0.0;
			for(uint32_t i = 1; i < numTrailPoints; i++) {
				trailLengthTotal += glm::length(trail.positions[i] - trail.positions[i - 1]);
				trailLength[i] = trailLengthTotal;
			}

			vec2d uvSize = vec2d(
				1.0 / static_cast<floatd>(emitter->particleSprite.framesRow),
				1.0 / static_cast<floatd>(emitter->particleSprite.framesColumn));
			vec2d uvStart = vec2d(
				static_cast<floatd>(trail.frame % emitter->particleSprite.framesRow) * uvSize.x,
				static_cast<floatd>(trail.frame / emitter->particleSprite.framesRow) * uvSize.y);
			switch(emitter->particleSprite.textureOrigin) {
				case TextureOrigin::bottom_right:
					uvStart.x = 1.0 - static_cast<floatd>(trail.frame % emitter->particleSprite.framesRow) * uvSize.x - uvSize.x;
					break;
				case TextureOrigin::top_left:
					uvStart.y = 1.0 - static_cast<floatd>(trail.frame / emitter->particleSprite.framesRow) * uvSize.y - uvSize.y;
					break;
				case TextureOrigin::top_right:
					uvStart.x = 1.0 - static_cast<floatd>(trail.frame % emitter->particleSprite.framesRow) * uvSize.x - uvSize.x;
					uvStart.y = 1.0 - static_cast<floatd>(trail.frame / emitter->particleSprite.framesRow) * uvSize.y - uvSize.y;
					break;
				default:
					break;
			}

			uint32_t trailSections = static_cast<uint32_t>(emitter->rendererTrailUVFactor - 0.000001) + 1;
			uint32_t trailSectionStart = 0;
			uint32_t trailSectionEnd = 0;

			for(uint32_t s = 0; s < trailSections; s++) {
				trailSectionEnd = numTrailPoints;
				for(uint32_t i = trailSectionStart; i < numTrailPoints; i++) {
					if(trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor > static_cast<floatd>(s + 1)) {
						trailSectionEnd = i + 1;
						break;
					}
				}

				if(indices) {
					for(uint32_t i = 0; i < trailSectionEnd - trailSectionStart - 1; i++) {
						indices[i0 + i * 6 + 0] = v0 + i * 2 + 0;
						indices[i0 + i * 6 + 1] = v0 + i * 2 + 2;
						indices[i0 + i * 6 + 2] = v0 + i * 2 + 1;
						indices[i0 + i * 6 + 3] = v0 + i * 2 + 2;
						indices[i0 + i * 6 + 4] = v0 + i * 2 + 3;
						indices[i0 + i * 6 + 5] = v0 + i * 2 + 1;
					}
				}

				if(positions) {
					for(uint32_t i = trailSectionStart; i < std::min(trailSectionEnd, numTrailPoints - 1); i++) {
						vec2d d = glm::normalize(trail.positions[i + 1] - trail.positions[i]);
						positions[(v0 + (i - trailSectionStart) * 2) * 2 + 0] = (trail.positions[i].x + d.y * std::max(trail.sizes[i].x, trail.sizes[i].y) * 0.5) * scaleX;
						positions[(v0 + (i - trailSectionStart) * 2) * 2 + 1] = (trail.positions[i].y - d.x * std::max(trail.sizes[i].x, trail.sizes[i].y) * 0.5) * scaleY;
						positions[(v0 + (i - trailSectionStart) * 2) * 2 + 2] = (trail.positions[i].x - d.y * std::max(trail.sizes[i].x, trail.sizes[i].y) * 0.5) * scaleX;
						positions[(v0 + (i - trailSectionStart) * 2) * 2 + 3] = (trail.positions[i].y + d.x * std::max(trail.sizes[i].x, trail.sizes[i].y) * 0.5) * scaleY;
					}

					if(trailSectionEnd == numTrailPoints) {
						vec2d d = glm::normalize(trail.positions[numTrailPoints - 2] - trail.positions[numTrailPoints - 1]);
						positions[(v0 + (numTrailPoints - trailSectionStart - 1) * 2) * 2 + 0] = (trail.positions[numTrailPoints - 1].x - d.y * std::max(trail.sizes[numTrailPoints - 1].x, trail.sizes[trailSectionEnd - 1].y) * 0.5) * scaleX;
						positions[(v0 + (numTrailPoints - trailSectionStart - 1) * 2) * 2 + 1] = (trail.positions[numTrailPoints - 1].y + d.x * std::max(trail.sizes[numTrailPoints - 1].x, trail.sizes[trailSectionEnd - 1].y) * 0.5) * scaleY;
						positions[(v0 + (numTrailPoints - trailSectionStart - 1) * 2) * 2 + 2] = (trail.positions[numTrailPoints - 1].x + d.y * std::max(trail.sizes[numTrailPoints - 1].x, trail.sizes[trailSectionEnd - 1].y) * 0.5) * scaleX;
						positions[(v0 + (numTrailPoints - trailSectionStart - 1) * 2) * 2 + 3] = (trail.positions[numTrailPoints - 1].y - d.x * std::max(trail.sizes[numTrailPoints - 1].x, trail.sizes[trailSectionEnd - 1].y) * 0.5) * scaleY;
					}
				}

				if(textureCoords) {
					switch(emitter->rendererTrailUVRotation) {
						case 1:
							for(uint32_t i = trailSectionStart; i < trailSectionEnd; i++) {
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 0] = uvStart.x + uvSize.x * (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 1] = uvStart.y;
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 2] = uvStart.x + uvSize.x * (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 3] = uvStart.y + uvSize.y;
							}
							break;

						case 2:
							for(uint32_t i = trailSectionStart; i < trailSectionEnd; i++) {
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 0] = uvStart.x;
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 1] = uvStart.y + uvSize.y - uvSize.y * (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 2] = uvStart.x + uvSize.x;
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 3] = uvStart.y + uvSize.y - uvSize.y * (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
							}
							break;

						case 3:
							for(uint32_t i = trailSectionStart; i < trailSectionEnd; i++) {
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 0] = uvStart.x + uvSize.x - uvSize.x * (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 1] = uvStart.y + uvSize.y;
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 2] = uvStart.x + uvSize.x - uvSize.x * (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 3] = uvStart.y;
							}
							break;
						
						default:
							for(uint32_t i = trailSectionStart; i < trailSectionEnd; i++) {
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 0] = uvStart.x + uvSize.x;
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 1] = uvStart.y + uvSize.y * (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 2] = uvStart.x;
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 3] = uvStart.y + uvSize.y * (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
							}
							break;
					}
				}

				if(colors) {
					for(uint32_t i = trailSectionStart; i < trailSectionEnd; i++) {
						colors[(v0 + (i - trailSectionStart) * 2) * 4 + 0] = trail.colors[i].r;
						colors[(v0 + (i - trailSectionStart) * 2) * 4 + 1] = trail.colors[i].g;
						colors[(v0 + (i - trailSectionStart) * 2) * 4 + 2] = trail.colors[i].b;
						colors[(v0 + (i - trailSectionStart) * 2) * 4 + 3] = trail.colors[i].a;
						colors[(v0 + (i - trailSectionStart) * 2) * 4 + 4] = trail.colors[i].r;
						colors[(v0 + (i - trailSectionStart) * 2) * 4 + 5] = trail.colors[i].g;
						colors[(v0 + (i - trailSectionStart) * 2) * 4 + 6] = trail.colors[i].b;
						colors[(v0 + (i - trailSectionStart) * 2) * 4 + 7] = trail.colors[i].a;
					}
				}

				i0 += (trailSectionEnd - trailSectionStart - 1) * 6;
				v0 += (trailSectionEnd - trailSectionStart) * 2;
				trailSectionStart = trailSectionEnd - 1;
			}
		}
	}

	void update(const ParticleEmitter& particleEmitter, const ParticleData& particleData, uint32_t num, floatd t);

	const ParticleEmitter* getEmitter() const;
	const ParticleData* getParticles() const;
	uint32_t getNumParticles() const;
	uint32_t getNumIndices() const;
	uint32_t getNumVertices() const;

private:
	struct TrailAnimationData {
		floatd startTime = 0.0;
		int32_t initialFrame = 0;
		int32_t frame = 0;
	};
	struct TrailVertexData {
		std::vector<vec2d> positions;
		std::vector<vec2d> sizes;
		std::vector<vec4d> colors;
		int32_t frame = 0;
	};

	std::vector<TrailVertexData> generateTrails() const;

	const ParticleEmitter* emitter = nullptr;
	const ParticleData* particles = nullptr;
	uint32_t numParticles = 0;

	uint32_t numIndices = 0;
	uint32_t numVertices = 0;

	std::vector<uint32_t> sortedParticleIndices;
	std::vector<uint32_t> numParticlesPerTrail;
	std::unordered_map<uint32_t, TrailAnimationData> trailAnimation;
};

class SpriteMeshBuilder {
public:
	SpriteMeshBuilder();
	SpriteMeshBuilder(const Sprite& spriteRef, uint32_t width, uint32_t height, floatd t);
	SpriteMeshBuilder(const Sprite& spriteRef, const ImageResource& imageResource, floatd t);

	template <typename IntT, typename FloatT>
	void build(IntT* indices, FloatT* positions, FloatT* textureCoords, FloatT* colors, floatd scaleX, floatd scaleY) const {
		vec2d position = vec2d(0.0);
		vec2d size = vec2d(0.0);
		floatd orientation = 0.0;
		vec4d color = vec4d(0.0);
		int32_t frame = 0;
		prepare(position, size, orientation, color, frame);

		std::vector<vec2d> pos = generateSpriteVertices(
			position,
			size,
			sprite->image.texturePivot,
			orientation);
		std::vector<vec2d> uv = generateSpriteTextureCoords(
			frame,
			sprite->image.framesRow,
			sprite->image.framesColumn,
			sprite->image.textureOrigin);

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

	void prepare(vec2d& position, vec2d& size, floatd& orientation, vec4d& color, int32_t& frame) const;

	const Sprite* getSprite() const;
	uint32_t getNumIndices() const;
	uint32_t getNumVertices() const;

private:
	const Sprite* sprite = nullptr;
	floatd aspectRatio = 1.0;
	floatd time = 0.0;
};
}