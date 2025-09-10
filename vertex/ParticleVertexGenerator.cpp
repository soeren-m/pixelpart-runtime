#include "ParticleVertexGenerator.h"
#include "SpriteVertexGenerator.h"
#include "TrailVertexGenerator.h"
#include "MeshVertexGenerator.h"
#include "../effect/ParticleType.h"

namespace pixelpart {
ParticleVertexGenerator::ParticleVertexGenerator(const Effect& effect, id_t particleEmitterId, id_t particleTypeId,
	const VertexFormat& vertexFormat) : VertexGenerator(),
	generatorEffect(effect),
	generatorParticleEmitterId(particleEmitterId), generatorParticleTypeId(particleTypeId),
	generatorVertexFormat(vertexFormat) {

}

VertexDataBufferDimensions ParticleVertexGenerator::buildGeometry(
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const RuntimeContext& runtimeContext, const SceneContext& sceneContext) {
	updateBaseGenerator();
	if(!generatorBaseGenerator) {
		return VertexDataBufferDimensions();
	}

	return generatorBaseGenerator->buildGeometry(particles, particleCount, runtimeContext, sceneContext);
}

void ParticleVertexGenerator::generateVertexData(const VertexDataBufferCollection& dataBuffers,
	ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
	const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const {
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
			generatorBaseGenerator = std::unique_ptr<VertexGenerator>(new SpriteVertexGenerator(
				generatorEffect, generatorParticleEmitterId, generatorParticleTypeId,
				generatorVertexFormat));
			generatorCurrentRendererType = ParticleRendererType::sprite;
			break;
		case ParticleRendererType::trail:
			generatorBaseGenerator = std::unique_ptr<VertexGenerator>(new TrailVertexGenerator(
				generatorEffect, generatorParticleEmitterId, generatorParticleTypeId,
				generatorVertexFormat));
			generatorCurrentRendererType = ParticleRendererType::trail;
			break;
		case ParticleRendererType::mesh:
			generatorBaseGenerator = std::unique_ptr<VertexGenerator>(new MeshVertexGenerator(
				generatorEffect, generatorParticleEmitterId, generatorParticleTypeId,
				generatorVertexFormat));
			generatorCurrentRendererType = ParticleRendererType::mesh;
			break;
		default:
			break;
	}
}
}
