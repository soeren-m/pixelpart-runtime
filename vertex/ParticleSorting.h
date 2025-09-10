#pragma once

#include "SceneContext.h"
#include "../common/Math.h"
#include "../common/ThreadPool.h"
#include "../effect/ParticleSortCriterion.h"
#include "../engine/ParticleCollection.h"
#include <cstdint>

namespace pixelpart {
void sortParticles(ParticleCollection& resultCollection,
	const ParticleCollection::ReadPtr& particles, std::uint32_t particleCount,
	const SceneContext& sceneContext,
	ParticleSortCriterion sortCriterion);

#ifdef PIXELPART_RUNTIME_MULTITHREADING
void sortParticles(ParticleCollection& resultCollection,
	const ParticleCollection::ReadPtr& particles, std::uint32_t particleCount,
	const SceneContext& sceneContext,
	ParticleSortCriterion sortCriterion, ThreadPool& threadPool);
#endif
}
