#pragma once

#include "EffectRuntimeState.h"
#include "../effect/Effect.h"
#include "../effect/EffectRuntimeContext.h"
#include "../types/Types.h"
#include <cstdint>

namespace pixelpart {
bool isEffectSimulationFinished(const Effect& effect, const EffectRuntimeState& state, const EffectRuntimeContext& context);

std::uint32_t queryEffectLod(const Effect& effect, const EffectRuntimeContext& runtimeContext, const float3_t& cameraPosition);
}
