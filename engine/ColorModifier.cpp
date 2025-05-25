#include "ColorModifier.h"
#include "../common/Math.h"
#include "../common/Color.h"
#include "../effect/ParticleType.h"
#include <cmath>

namespace pixelpart {
void ColorModifier::run(const Effect* effect, RuntimeContext runtimeContext, ParticleRuntimeId runtimeId,
	ParticleCollection::WritePtr particles, std::uint32_t particleCount) const {
	const ParticleType& particleType = effect->particleTypes().at(runtimeId.typeId);

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float4_t hsv = rgb2hsv(particleType.color().at(particles.life[p]));

		float_t hue = std::fmod(hsv.x + particles.initialColor[p].x, 360.0);
		hsv.x = hue < 0.0 ? hue + 360.0 : hue;
		hsv.y = glm::clamp(hsv.y + particles.initialColor[p].y, 0.0, 1.0);
		hsv.z = glm::clamp(hsv.z + particles.initialColor[p].z, 0.0, 1.0);
		hsv.w = hsv.w * particles.initialColor[p].w * particleType.opacity().at(particles.life[p]);

		particles.color[p] = hsv2rgb(hsv);
	}
}

void ColorModifier::prepare(const Effect& effect, const RuntimeContext& runtimeContext) {

}
}