#include "EffectRuntimeQuery.h"
#include "../effect/ParticleEmitter.h"

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
}
