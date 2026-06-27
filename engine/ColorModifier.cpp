#include "ColorModifier.h"
#include "ColorConversion.h"
#include "../effect/ParticleType.h"
#include "../effect/Curve.h"
#include "../types/Types.h"
#include <cmath>
#include <algorithm>

namespace pixelpart {
void ColorModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);

	const Curve<float4_t>& particleColorCurve = particleType.color().resultCurve();
	const Curve<float_t>& particleOpacityCurve = particleType.opacity().resultCurve();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float4_t hsv = rgb2hsv(particleColorCurve.at(particles.life[p]));

		float_t hue = std::fmod(hsv.x + particles.initialColor[p].x, 360.0);
		hsv.x = hue < 0.0 ? hue + 360.0 : hue;
		hsv.y = std::clamp(hsv.y + particles.initialColor[p].y, 0.0, 1.0);
		hsv.z = std::clamp(hsv.z + particles.initialColor[p].z, 0.0, 1.0);
		hsv.w *= particles.initialColor[p].w * particleOpacityCurve.at(particles.life[p]);

		particles.color[p] = hsv2rgb(hsv);
	}
}

void ColorModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {

}
}
