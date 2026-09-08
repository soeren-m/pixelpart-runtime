#pragma once

#include "EffectEngine.h"
#include "EffectRuntimeState.h"

namespace pixelpart {
class CpuEffectEngine : public EffectEngine {
public:
	virtual const EffectRuntimeState& state() const = 0;
};
}
