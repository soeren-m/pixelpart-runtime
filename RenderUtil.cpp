#include "RenderUtil.h"

namespace pixelpart {
vec2d rotate(const vec2d& p, const vec2d& o, floatd a) {
	floatd s = std::sin(glm::radians(a));
	floatd c = std::cos(glm::radians(a));

	return vec2d(
		(p.x - o.x) * c - (p.y - o.y) * s + o.x,
		(p.x - o.x) * s + (p.y - o.y) * c + o.y);
}

std::vector<vec2d> generateSpriteVertices(const vec2d& pivot, const vec2d& position, const vec2d& size, floatd rotation) {
	return std::vector<vec2d>{
		position + rotate(vec2d(-0.5, -0.5) * size, pivot * size, rotation),
		position + rotate(vec2d(+0.5, -0.5) * size, pivot * size, rotation),
		position + rotate(vec2d(+0.5, +0.5) * size, pivot * size, rotation),
		position + rotate(vec2d(-0.5, +0.5) * size, pivot * size, rotation)
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
		sortedParticleIndices.resize(numParticles);
		std::iota(sortedParticleIndices.begin(), sortedParticleIndices.end(), 0);
		std::sort(sortedParticleIndices.begin(), sortedParticleIndices.end(), [this](uint32_t p0, uint32_t p1) {
			return (particles->parentId[p0] != particles->parentId[p1])
				? particles->parentId[p0] > particles->parentId[p1]
				: particles->spawnId[p0] > particles->spawnId[p1];
		});

		numParticlesPerTrail.clear();

		for(uint32_t p = 0, parentId = 0; p < numParticles; p++) {
			if(particles->parentId[sortedParticleIndices[p]] != parentId || p == 0) {
				parentId = particles->parentId[sortedParticleIndices[p]];

				numParticlesPerTrail.push_back(0);
				if(trails.count(parentId) == 0) {
					trails[parentId] = Trail();
					trails[parentId].startTime = t;
				}
				else {
					trails[parentId].life = t - trails[parentId].startTime;
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

std::vector<ParticleMeshBuilder::TrailVertexData> ParticleMeshBuilder::generateTrailVertices() const {
	std::vector<TrailVertexData> trailsVertexData;

	if(emitter->rendererNumTrailSegments > 0) {
		for(uint32_t p = 0, parentParticleId = 0; p < numParticles; p++) {
			if(particles->parentId[sortedParticleIndices[p]] != parentParticleId || p == 0) {
				const uint32_t numParticlesInTrail = numParticlesPerTrail[trailsVertexData.size()];
				parentParticleId = particles->parentId[sortedParticleIndices[p]];

				floatd trailLengthTotal = 0.0;
				std::vector<floatd> trailLength(numParticlesInTrail, 0.0);
				for(uint32_t pTrail = 1; pTrail < numParticlesInTrail; pTrail++) {
					trailLengthTotal += glm::length(particles->globalPosition[sortedParticleIndices[p + pTrail]] - particles->globalPosition[sortedParticleIndices[p + pTrail - 1]]);
					trailLength[pTrail] = trailLengthTotal;
				}
				for(uint32_t pTrail = 0; pTrail < numParticlesInTrail; pTrail++) {
					trailLength[pTrail] /= trailLengthTotal;
				}

				Curve<vec2d> curvePosition(CurveInterpolation::spline);
				Curve<vec2d> curveSize(CurveInterpolation::spline);
				Curve<vec4d> curveColor(CurveInterpolation::spline);
				Curve<vec2d> curveVelocity(CurveInterpolation::spline);
				Curve<vec2d> curveForce(CurveInterpolation::spline);

				curvePosition.enableFixedCache(emitter->rendererNumTrailSegments);
				curvePosition.setPointsOrdered<uint32_t>(trailLength.data(), particles->globalPosition.data(), sortedParticleIndices.data() + p, numParticlesInTrail);
				curveSize.enableFixedCache(emitter->rendererNumTrailSegments);
				curveSize.setPointsOrdered<uint32_t>(trailLength.data(), particles->size.data(), sortedParticleIndices.data() + p, numParticlesInTrail);
				curveColor.enableFixedCache(emitter->rendererNumTrailSegments);
				curveColor.setPointsOrdered<uint32_t>(trailLength.data(), particles->color.data(), sortedParticleIndices.data() + p,numParticlesInTrail);
				curveVelocity.enableFixedCache(emitter->rendererNumTrailSegments);
				curveVelocity.setPointsOrdered<uint32_t>(trailLength.data(), particles->velocity.data(), sortedParticleIndices.data() + p, numParticlesInTrail);
				curveForce.enableFixedCache(emitter->rendererNumTrailSegments);
				curveForce.setPointsOrdered<uint32_t>(trailLength.data(), particles->force.data(), sortedParticleIndices.data() + p, numParticlesInTrail);

				trailsVertexData.emplace_back();
				trailsVertexData.back().id = static_cast<int32_t>(parentParticleId);
				trailsVertexData.back().life = trails.at(parentParticleId).life;
				trailsVertexData.back().positions = curvePosition.getCache();
				trailsVertexData.back().sizes = curveSize.getCache();
				trailsVertexData.back().colors = curveColor.getCache();
				trailsVertexData.back().velocities = curveVelocity.getCache();
				trailsVertexData.back().forces = curveForce.getCache();
			}
		}
	}
	else {
		for(uint32_t p = 0, pTrail = 0, parentParticleId = 0; p < numParticles; p++, pTrail++) {
			if(particles->parentId[sortedParticleIndices[p]] != parentParticleId || p == 0) {
				const uint32_t numParticlesInTrail = numParticlesPerTrail[trailsVertexData.size()];
				parentParticleId = particles->parentId[sortedParticleIndices[p]];
				pTrail = 0;

				trailsVertexData.emplace_back();
				trailsVertexData.back().id = static_cast<int32_t>(parentParticleId);
				trailsVertexData.back().life = trails.at(parentParticleId).life;
				trailsVertexData.back().positions.resize(numParticlesInTrail);
				trailsVertexData.back().sizes.resize(numParticlesInTrail);
				trailsVertexData.back().colors.resize(numParticlesInTrail);
				trailsVertexData.back().velocities.resize(numParticlesInTrail);
				trailsVertexData.back().forces.resize(numParticlesInTrail);
			}

			trailsVertexData.back().positions[pTrail] = particles->globalPosition[sortedParticleIndices[p]];
			trailsVertexData.back().sizes[pTrail] = particles->size[sortedParticleIndices[p]];
			trailsVertexData.back().colors[pTrail] = particles->color[sortedParticleIndices[p]];
			trailsVertexData.back().velocities[pTrail] = particles->velocity[sortedParticleIndices[p]];
			trailsVertexData.back().forces[pTrail] = particles->force[sortedParticleIndices[p]];
		}
	}

	return trailsVertexData;
}

SpriteMeshBuilder::SpriteMeshBuilder() {

}
SpriteMeshBuilder::SpriteMeshBuilder(const Sprite& spriteRef, floatd t) : sprite(&spriteRef), time(t) {

}

void SpriteMeshBuilder::prepare(vec2d& position, vec2d& size, floatd& orientation, vec4d& color, floatd& life) const {
	life = std::fmod(time - sprite->lifetimeStart, sprite->lifetimeDuration) / sprite->lifetimeDuration;
	position = sprite->motionPath.get(life);
	size = vec2d(
		sprite->width.get(life),
		sprite->height.get(life));
	orientation = sprite->orientation.get(life) + ((sprite->alignWithPath && position != sprite->motionPath.get(life + 0.01))
		? glm::degrees(glm::orientedAngle(vec2d(0.0, 1.0), glm::normalize(sprite->motionPath.get(life + 0.01) - position)))
		: 0.0);
	color = vec4d(
		vec3d(sprite->color.get(life)),
		sprite->opacity.get(life));
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