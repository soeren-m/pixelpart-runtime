#pragma once

#include "VertexFormat.h"
#include "VertexDataBufferDimensions.h"
#include "VertexDataBufferCollection.h"
#include "SceneContext.h"
#include "../types/Types.h"
#include "../types/Id.h"
#include "../effect/Effect.h"
#include "../effect/EffectRuntimeContext.h"
#include "../engine/ParticleCollection.h"
#include <cstdint>
#include <algorithm>

namespace pixelpart {
class VertexGenerator {
public:
	virtual ~VertexGenerator() = default;

	virtual VertexDataBufferDimensions buildGeometry(
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) = 0;

	virtual void generateVertexData(const VertexDataBufferCollection& dataBuffers,
		ParticleCollection::ReadPtr particles, std::uint32_t particleCount,
		const EffectRuntimeContext& runtimeContext, const SceneContext& sceneContext) const = 0;

protected:
	struct ColorUByte {
		std::uint8_t r = 0;
		std::uint8_t g = 0;
		std::uint8_t b = 0;
		std::uint8_t a = 0;

		ColorUByte() = default;

		template <typename T>
		ColorUByte(const math::vector4<T>& rgba) :
			r(static_cast<std::uint8_t>(math::clamp(rgba.x * T(255), T(0), T(255)))),
			g(static_cast<std::uint8_t>(math::clamp(rgba.y * T(255), T(0), T(255)))),
			b(static_cast<std::uint8_t>(math::clamp(rgba.z * T(255), T(0), T(255)))),
			a(static_cast<std::uint8_t>(math::clamp(rgba.w * T(255), T(0), T(255)))) {

		}
	};
};
}
