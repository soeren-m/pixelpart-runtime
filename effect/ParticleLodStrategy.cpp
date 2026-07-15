#include "ParticleLodStrategy.h"

namespace pixelpart {
void ParticleLodStrategy::hide(bool hide) {
	strategyHide = hide;
}
bool ParticleLodStrategy::hide() const {
	return strategyHide;
}

void ParticleLodStrategy::emissionFactor(float_t factor) {
	strategyEmissionFactor = factor;
}
float_t ParticleLodStrategy::emissionFactor() const {
	return strategyEmissionFactor;
}

void ParticleLodStrategy::lifetimeFactor(float_t factor) {
	strategyLifetimeFactor = factor;
}
float_t ParticleLodStrategy::lifetimeFactor() const {
	return strategyLifetimeFactor;
}

void to_json(nlohmann::ordered_json& j, const ParticleLodStrategy& lodStrategy) {
	j = nlohmann::ordered_json{
		{ "hide", lodStrategy.hide() },
		{ "emission_factor", lodStrategy.emissionFactor() },
		{ "lifetime_factor", lodStrategy.lifetimeFactor() }
	};
}
void from_json(const nlohmann::ordered_json& j, ParticleLodStrategy& lodStrategy) {
	lodStrategy.hide(j.value("hide", false));
	lodStrategy.emissionFactor(j.value("emission_factor", 1.0));
	lodStrategy.lifetimeFactor(j.value("lifetime_factor", 1.0));
}
}
