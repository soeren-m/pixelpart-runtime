#pragma once

#include "../effect/Effect.h"
#include "../effect/EffectRuntimeContext.h"
#include "EffectRuntimeState.h"

namespace pixelpart {
bool isEffectSimulationFinished(const Effect& effect, const EffectRuntimeState& state, const EffectRuntimeContext& context);
}
