#include "ColorSolver.h"
#include "../common/Math.h"
#include "../common/Color.h"
#include <cmath>

namespace pixelpart {
ColorSolver::ColorSolver() {

}

void ColorSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const {
	for(uint32_t p = 0u; p < particleCount; p++) {
		float4_t hsv = rgb2hsv(particleType.color().at(particles.life[p]));

		float_t hue = std::fmod(hsv.x + particles.initialColor[p].x, 360.0);
		hsv.x = (hue < 0.0) ? hue + 360.0 : hue;
		hsv.y = glm::clamp(hsv.y + particles.initialColor[p].y, 0.0, 1.0);
		hsv.z = glm::clamp(hsv.z + particles.initialColor[p].z, 0.0, 1.0);
		hsv.w = hsv.w * particles.initialColor[p].w * particleType.opacity().at(particles.life[p]);

		particles.color[p] = hsv2rgb(hsv);
	}
}

void ColorSolver::prepare(const Effect& effect) {

}
}