#include "ParticleRuntimePair.h"

namespace pixelpart {
ParticleRuntimePair::ParticleRuntimePair(id_t emitter, id_t type) :
	emitterId(emitter), typeId(type) {

}

bool operator==(const ParticleRuntimePair& lhs, const ParticleRuntimePair& rhs) {
	return lhs.emitterId == rhs.emitterId && lhs.typeId == rhs.typeId;
}
}