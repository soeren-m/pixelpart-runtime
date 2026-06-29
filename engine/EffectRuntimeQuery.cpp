#include "EffectRuntimeQuery.h"
#include "../effect/ParticleEmitter.h"
#include "../math/Geometry.h"

namespace pixelpart {
bool isEffectSimulationFinished(const Effect& effect, const EffectRuntimeState& state, const EffectRuntimeContext& context) {
	for(const auto* particleEmitter : effect.sceneGraph().nodesWithType<ParticleEmitter>()) {
		if(!particleEmitter->primary()) {
			continue;
		}

		if(particleEmitter->active(context) ||
			particleEmitter->repeat() ||
			context.time() < particleEmitter->start() + particleEmitter->duration()) {
			return false;
		}
	}

	for(const auto& [emissionPair, particleCollection] : state.particleCollections()) {
		if(particleCollection.count() > 0) {
			return false;
		}
	}

	return true;
}

std::uint32_t queryEffectLod(const Effect& effect, const EffectRuntimeContext& runtimeContext, const float3_t& cameraPosition) {
	float3_t effectPosition = float3_t(0.0);
	for(const auto& node : effect.sceneGraph()) {
		if(!node->parentId()) {
			effectPosition = effect.sceneGraph().globalTransform(node->id(), runtimeContext).position();
			break;
		}
	}

	const auto& lods = effect.lods();

	float_t distance = math::distance(effectPosition, cameraPosition);

	for(std::int32_t lodIndex = static_cast<std::int32_t>(lods.size()) - 1; lodIndex >= 0; lodIndex--) {
		if(distance > lods[lodIndex].distanceThreshold()) {
			return static_cast<std::uint32_t>(lodIndex);
		}
	}

	return 0;
}
}
