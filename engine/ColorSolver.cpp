#include "ColorSolver.h"
#include "../common/Color.h"

namespace pixelpart {
ColorSolver::ColorSolver() {

}

void ColorSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleWritePtr particles, uint32_t numParticles, float_t t, float_t dt) const {
	for(uint32_t p = 0u; p < numParticles; p++) {
		vec4_t hsv = rgb2hsv(particleType.color.get(particles.life[p]));

		float_t hue = std::fmod(hsv.x + particles.initialColor[p].x, 360.0);
		hsv.x = (hue < 0.0) ? hue + 360.0 : hue;
		hsv.y = glm::clamp(hsv.y + particles.initialColor[p].y, 0.0, 1.0);
		hsv.z = glm::clamp(hsv.z + particles.initialColor[p].z, 0.0, 1.0);
		hsv.w = hsv.w * particles.initialColor[p].w * particleType.opacity.get(particles.life[p]);

		particles.color[p] = hsv2rgb(hsv);
	}
}

void ColorSolver::refresh(const Effect& effect) {

}
}