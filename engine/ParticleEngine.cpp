#include "ParticleEngine.h"

namespace pixelpart {
ParticleEngine::ParticleEngine(const Effect& fx) : particleEffect(fx) {

}

const Effect& ParticleEngine::effect() const {
	return particleEffect;
}
}