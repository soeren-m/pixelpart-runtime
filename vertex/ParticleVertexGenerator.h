#pragma once

#include "VertexGenerator.h"
#include "../effect/ParticleRendererType.h"
#include <memory>

namespace pixelpart {
class ParticleVertexGenerator : public VertexGenerator {
public:
	ParticleVertexGenerator(const Effect& effect, id_t particleEmitterId, id_t particleTypeId,
		const VertexFormat& vertexFormat);

	virtual VertexDataBufferDimensions buildGeometry(
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const RuntimeContext& runtimeContext, const SceneContext& sceneContext) override;

	virtual void generateVertexData(const VertexDataBufferCollection& dataBuffers,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const RuntimeContext& runtimeContext, const SceneContext& sceneContext) const override;

private:
	void updateBaseGenerator();

	const Effect& generatorEffect;
	id_t generatorParticleEmitterId;
	id_t generatorParticleTypeId;

	VertexFormat generatorVertexFormat;

	std::unique_ptr<VertexGenerator> generatorBaseGenerator;
	ParticleRendererType generatorCurrentRendererType = ParticleRendererType::sprite;
};
}
