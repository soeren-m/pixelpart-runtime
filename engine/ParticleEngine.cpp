#include "ParticleEngine.h"

namespace pixelpart {
ParticleEngine::ParticleEngine(const Effect& fx) : effect(fx) {

}

const Effect& ParticleEngine::getEffect() const {
	return effect;
}
}