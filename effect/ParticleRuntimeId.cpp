#include "ParticleRuntimeId.h"

namespace pixelpart {
ParticleRuntimeId::ParticleRuntimeId(id_t emitter, id_t type) :
	emitterId(emitter), typeId(type) {

}

bool operator==(const ParticleRuntimeId& lhs, const ParticleRuntimeId& rhs) {
	return lhs.emitterId == rhs.emitterId && lhs.typeId == rhs.typeId;
}
}