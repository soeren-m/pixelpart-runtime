#include "ParticleVertexGenerator.h"
#include "SpriteVertexGenerator.h"
#include "TrailVertexGenerator.h"
#include "MeshVertexGenerator.h"
#include "../effect/ParticleType.h"

namespace pixelpart {
ParticleVertexGenerator::ParticleVertexGenerator(const Effect& effect, id_t particleEmitterId, id_t particleTypeId,
	const VertexFormat& vertexFormat,
	std::shared_ptr<ThreadPool> threadPool) : VertexGenerator(),
	generatorEffect(effect),
	generatorParticleEmitterId(particleEmitterId), generatorParticleTypeId(particleTypeId),
	generatorVertexFormat(vertexFormat),
	generatorThreadPool(threadPool) {

}

VertexDataBufferDimensions ParticleVertexGenerator::buildGeometry(
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) {
	updateBaseGenerator();
	if(!generatorBaseGenerator) {
		return VertexDataBufferDimensions();
	}

	return generatorBaseGenerator->buildGeometry(particles, particleCount, runtimeContext, sceneContext);
}

void ParticleVertexGenerator::generateVertexData(const VertexDataBufferCollection& dataBuffers,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
	if(!generatorBaseGenerator) {
		return;
	}

	generatorBaseGenerator->generateVertexData(dataBuffers, particles, particleCount, runtimeContext, sceneContext);
}

void ParticleVertexGenerator::updateBaseGenerator() {
	const ParticleType& particleType = generatorEffect.particleTypes().at(generatorParticleTypeId);
	if(generatorBaseGenerator && particleType.renderer() == generatorCurrentRendererType) {
		return;
	}

	generatorBaseGenerator = nullptr;

	switch(particleType.renderer()) {
		case ParticleRendererType::sprite:
			generatorBaseGenerator = std::make_unique<SpriteVertexGenerator>(
				generatorEffect, generatorParticleEmitterId, generatorParticleTypeId,
				generatorVertexFormat,
				generatorThreadPool);
			generatorCurrentRendererType = ParticleRendererType::sprite;
			break;
		case ParticleRendererType::trail:
			generatorBaseGenerator = std::make_unique<TrailVertexGenerator>(
				generatorEffect, generatorParticleEmitterId, generatorParticleTypeId,
				generatorVertexFormat,
				generatorThreadPool);
			generatorCurrentRendererType = ParticleRendererType::trail;
			break;
		case ParticleRendererType::mesh:
			generatorBaseGenerator = std::make_unique<MeshVertexGenerator>(
				generatorEffect, generatorParticleEmitterId, generatorParticleTypeId,
				generatorVertexFormat,
				generatorThreadPool);
			generatorCurrentRendererType = ParticleRendererType::mesh;
			break;
		default:
			break;
	}
}
}
