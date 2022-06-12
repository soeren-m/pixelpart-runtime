#pragma once

#include "ShaderGraph.h"

namespace pixelpart {
class ShaderGraphBuilder {
public:
	static ShaderGraph buildEmptyShaderGraph();
	static ShaderGraph buildTextureShaderGraph(const std::string& imageResourceId, const std::string& imageResourceFilepath,
		int32_t colorBlendMode = 0, floatd alphaThreshold = 0.0);
	static ShaderGraph buildSpriteSheetStaticShaderGraph(const std::string& imageResourceId, const std::string& imageResourceFilepath,
		uint32_t numFramesRow = 1, uint32_t numFramesColumn = 1, uint32_t textureOrigin = 0, uint32_t frame = 0, bool randomFrame = false,
		int32_t colorBlendMode = 0, floatd alphaThreshold = 0.0);
	static ShaderGraph buildSpriteSheetAnimationShaderGraph(const std::string& imageResourceId, const std::string& imageResourceFilepath,
		uint32_t numFramesRow = 1, uint32_t numFramesColumn = 1, uint32_t textureOrigin = 0, uint32_t frame = 0,
		uint32_t numAnimationFrames = 1, floatd animationDuration = 1.0, bool animationLoop = false,
		int32_t colorBlendMode = 0, floatd alphaThreshold = 0.0);
};
}