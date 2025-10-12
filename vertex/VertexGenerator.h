#pragma once

#include "VertexFormat.h"
#include "VertexDataBufferDimensions.h"
#include "VertexDataBufferCollection.h"
#include "SceneContext.h"
#include "../common/Math.h"
#include "../common/Id.h"
#include "../effect/Effect.h"
#include "../effect/EffectRuntimeContext.h"
#include "../engine/ParticleCollection.h"
#include <cstdint>

namespace pixelpart {
class VertexGenerator {
public:
	virtual ~VertexGenerator() = default;

	virtual VertexDataBufferDimensions buildGeometry(
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) = 0;

	virtual void generateVertexData(const VertexDataBufferCollection& dataBuffers,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const = 0;
};
}
