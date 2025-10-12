#include "ParticleEmissionPair.h"

namespace pixelpart {
ParticleEmissionPair::ParticleEmissionPair(id_t emitter, id_t type) :
	emitterId(emitter), typeId(type) {

}

bool operator==(const ParticleEmissionPair& lhs, const ParticleEmissionPair& rhs) {
	return lhs.emitterId == rhs.emitterId && lhs.typeId == rhs.typeId;
}
}
