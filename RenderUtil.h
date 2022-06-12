#pragma once

#include "ParticleData.h"
#include "ParticleEmitter.h"
#include "Sprite.h"
#include "ImageResource.h"
#include <unordered_map>

namespace pixelpart {
std::vector<vec2d> generateSpriteVertices(const vec2d& pivot, const vec2d& position, const vec2d& size, floatd rotation);

class ParticleMeshBuilder {
public:
	ParticleMeshBuilder();
	ParticleMeshBuilder(const ParticleEmitter& particleEmitter, const ParticleData& particleData, uint32_t num, floatd t);

	template <typename IntT, typename FloatT>
	void build(IntT* indices, FloatT* positions, FloatT* textureCoords, FloatT* colors, FloatT* velocities, FloatT* forces, FloatT* lives, IntT* ids, floatd scaleX, floatd scaleY) const {
		if(!emitter) {
			return;
		}

		if(emitter->renderer == ParticleEmitter::RendererType::trail && numParticles > 1) {
			buildTrails(indices, positions, textureCoords, colors, velocities, forces, lives, ids, scaleX, scaleY);
		}
		else {
			buildSprites(indices, positions, textureCoords, colors, velocities, forces, lives, ids, scaleX, scaleY);
		}
	}

	template <typename IntT, typename FloatT>
	void buildSprites(IntT* indices, FloatT* positions, FloatT* textureCoords, FloatT* colors, FloatT* velocities, FloatT* forces, FloatT* lives, IntT* ids, floatd scaleX, floatd scaleY) const {
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
					emitter->particlePivot,
					particles->globalPosition[p],
					particles->size[p],
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
				textureCoords[p * 4 * 2 + 0] = 0.0;
				textureCoords[p * 4 * 2 + 1] = 0.0;
				textureCoords[p * 4 * 2 + 2] = 1.0;
				textureCoords[p * 4 * 2 + 3] = 0.0;
				textureCoords[p * 4 * 2 + 4] = 1.0;
				textureCoords[p * 4 * 2 + 5] = 1.0;
				textureCoords[p * 4 * 2 + 6] = 0.0;
				textureCoords[p * 4 * 2 + 7] = 1.0;
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

		if(velocities) {
			for(uint32_t p = 0; p < numParticles; p++) {
				velocities[p * 4 * 2 + 0] = particles->velocity[p].x;
				velocities[p * 4 * 2 + 1] = particles->velocity[p].y;
				velocities[p * 4 * 2 + 2] = particles->velocity[p].x;
				velocities[p * 4 * 2 + 3] = particles->velocity[p].y;
				velocities[p * 4 * 2 + 4] = particles->velocity[p].x;
				velocities[p * 4 * 2 + 5] = particles->velocity[p].y;
				velocities[p * 4 * 2 + 6] = particles->velocity[p].x;
				velocities[p * 4 * 2 + 7] = particles->velocity[p].y;
			}
		}

		if(forces) {
			for(uint32_t p = 0; p < numParticles; p++) {
				forces[p * 4 * 2 + 0] = particles->force[p].x;
				forces[p * 4 * 2 + 1] = particles->force[p].y;
				forces[p * 4 * 2 + 2] = particles->force[p].x;
				forces[p * 4 * 2 + 3] = particles->force[p].y;
				forces[p * 4 * 2 + 4] = particles->force[p].x;
				forces[p * 4 * 2 + 5] = particles->force[p].y;
				forces[p * 4 * 2 + 6] = particles->force[p].x;
				forces[p * 4 * 2 + 7] = particles->force[p].y;
			}
		}

		if(lives) {
			for(uint32_t p = 0; p < numParticles; p++) {
				lives[p * 4 + 0] = particles->life[p];
				lives[p * 4 + 1] = particles->life[p];
				lives[p * 4 + 2] = particles->life[p];
				lives[p * 4 + 3] = particles->life[p];
			}
		}

		if(ids) {
			for(uint32_t p = 0; p < numParticles; p++) {
				ids[p * 4 + 0] = static_cast<IntT>(particles->spawnId[p]);
				ids[p * 4 + 1] = static_cast<IntT>(particles->spawnId[p]);
				ids[p * 4 + 2] = static_cast<IntT>(particles->spawnId[p]);
				ids[p * 4 + 3] = static_cast<IntT>(particles->spawnId[p]);
			}
		}
	}

	template <typename IntT, typename FloatT>
	void buildTrails(IntT* indices, FloatT* positions, FloatT* textureCoords, FloatT* colors, FloatT* velocities, FloatT* forces, FloatT* lives, IntT* ids, floatd scaleX, floatd scaleY) const {
		if(!emitter || !particles) {
			return;
		}

		std::vector<TrailVertexData> trailsVertexData = generateTrailVertices();
		uint32_t i0 = 0;
		uint32_t v0 = 0;

		for(const TrailVertexData& trail : trailsVertexData) {
			uint32_t numTrailPoints = static_cast<uint32_t>(trail.positions.size());
			if(numTrailPoints < 2) {
				continue;
			}

			std::vector<floatd> trailLength(numTrailPoints, 0.0);
			floatd trailLengthTotal = 0.0;
			for(uint32_t i = 1; i < numTrailPoints; i++) {
				trailLengthTotal += glm::length(trail.positions[i] - trail.positions[i - 1]);
				trailLength[i] = trailLengthTotal;
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
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 0] = trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s);
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 1] = 0.0;
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 2] = trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s);
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 3] = 1.0;
							}
							break;

						case 2:
							for(uint32_t i = trailSectionStart; i < trailSectionEnd; i++) {
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 0] = 0.0;
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 1] = 1.0 - (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 2] = 1.0;
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 3] = 1.0 - (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
							}
							break;

						case 3:
							for(uint32_t i = trailSectionStart; i < trailSectionEnd; i++) {
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 0] = 1.0 - (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 1] = 1.0;
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 2] = 1.0 - (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 3] = 0.0;
							}
							break;
						
						default:
							for(uint32_t i = trailSectionStart; i < trailSectionEnd; i++) {
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 0] = 1.0;
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 1] = 0.0 + (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 2] = 0.0;
								textureCoords[(v0 + (i - trailSectionStart) * 2) * 2 + 3] = 0.0 + (trailLength[i] / trailLengthTotal * emitter->rendererTrailUVFactor - static_cast<floatd>(s));
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

				if(velocities) {
					for(uint32_t i = trailSectionStart; i < trailSectionEnd; i++) {
						velocities[(v0 + (i - trailSectionStart) * 2) * 2 + 0] = trail.velocities[i].x;
						velocities[(v0 + (i - trailSectionStart) * 2) * 2 + 1] = trail.velocities[i].y;
						velocities[(v0 + (i - trailSectionStart) * 2) * 2 + 2] = trail.velocities[i].x;
						velocities[(v0 + (i - trailSectionStart) * 2) * 2 + 3] = trail.velocities[i].y;
					}
				}

				if(forces) {
					for(uint32_t i = trailSectionStart; i < trailSectionEnd; i++) {
						forces[(v0 + (i - trailSectionStart) * 2) * 2 + 0] = trail.forces[i].x;
						forces[(v0 + (i - trailSectionStart) * 2) * 2 + 1] = trail.forces[i].y;
						forces[(v0 + (i - trailSectionStart) * 2) * 2 + 2] = trail.forces[i].x;
						forces[(v0 + (i - trailSectionStart) * 2) * 2 + 3] = trail.forces[i].y;
					}
				}

				if(lives) {
					for(uint32_t i = trailSectionStart; i < trailSectionEnd; i++) {
						lives[(v0 + (i - trailSectionStart) * 2) + 0] = trail.life;
						lives[(v0 + (i - trailSectionStart) * 2) + 1] = trail.life;
					}
				}

				if(ids) {
					for(uint32_t i = trailSectionStart; i < trailSectionEnd; i++) {
						ids[(v0 + (i - trailSectionStart) * 2) + 0] = trail.id;
						ids[(v0 + (i - trailSectionStart) * 2) + 1] = trail.id;
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
	struct Trail {
		floatd startTime = 0.0;
		floatd life = 0.0;
	};
	struct TrailVertexData {
		int32_t id = 0;
		floatd life = 0.0;
		std::vector<vec2d> positions;
		std::vector<vec2d> sizes;
		std::vector<vec4d> colors;
		std::vector<vec2d> velocities;
		std::vector<vec2d> forces;
	};

	std::vector<TrailVertexData> generateTrailVertices() const;

	const ParticleEmitter* emitter = nullptr;
	const ParticleData* particles = nullptr;
	uint32_t numParticles = 0;

	uint32_t numIndices = 0;
	uint32_t numVertices = 0;

	std::vector<uint32_t> sortedParticleIndices;
	std::vector<uint32_t> numParticlesPerTrail;
	std::unordered_map<uint32_t, Trail> trails;
};

class SpriteMeshBuilder {
public:
	SpriteMeshBuilder();
	SpriteMeshBuilder(const Sprite& spriteRef, floatd t);

	template <typename IntT, typename FloatT>
	void build(IntT* indices, FloatT* positions, FloatT* textureCoords, FloatT* colors, FloatT* lives, floatd scaleX, floatd scaleY) const {
		vec2d position = vec2d(0.0);
		vec2d size = vec2d(0.0);
		floatd orientation = 0.0;
		vec4d color = vec4d(0.0);
		floatd life = 0.0;
		prepare(position, size, orientation, color, life);

		std::vector<vec2d> pos = generateSpriteVertices(
			sprite->pivot,
			position,
			size,
			orientation);

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
			textureCoords[0] = 0.0;
			textureCoords[1] = 0.0;
			textureCoords[2] = 1.0;
			textureCoords[3] = 0.0;
			textureCoords[4] = 1.0;
			textureCoords[5] = 1.0;
			textureCoords[6] = 0.0;
			textureCoords[7] = 1.0;
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

		if(lives) {
			lives[0] = life;
			lives[1] = life;
			lives[2] = life;
			lives[3] = life;
		}
	}

	void prepare(vec2d& position, vec2d& size, floatd& orientation, vec4d& color, floatd& life) const;

	const Sprite* getSprite() const;
	uint32_t getNumIndices() const;
	uint32_t getNumVertices() const;

private:
	const Sprite* sprite = nullptr;
	floatd time = 0.0;
};
}