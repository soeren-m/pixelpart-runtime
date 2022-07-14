#include "ShaderGraphBuilder.h"

namespace pixelpart {
ShaderGraph ShaderGraphBuilder::buildEmptyShaderGraph() {
	ShaderGraph shader;
	uint32_t outputNodeId = shader.addNode("shader_node_output");
	uint32_t colorNodeId = shader.addNode("shader_node_color");

	shader.linkNodes(colorNodeId, outputNodeId, 0, 0);

	shader.setNodePosition(outputNodeId, vec2d(300.0, 0.0));
	shader.setNodePosition(colorNodeId, vec2d(0.0, 0.0));

	return shader;
}

ShaderGraph ShaderGraphBuilder::buildTextureShaderGraph(const std::string& imageResourceId, const std::string& imageResourceFilepath,
int32_t colorBlendMode, floatd alphaThreshold) {
	ShaderGraph shader;
	uint32_t outputNodeId = shader.addNode("shader_node_output");
	uint32_t vertexTextureCoordNodeId = shader.addNode("shader_node_vertex_texturecoord");
	uint32_t vertexColorNodeId = shader.addNode("shader_node_vertex_color");
	uint32_t textureNodeId = shader.addNode("shader_node_texture_image");
	uint32_t blendNodeId = shader.addNode("shader_node_blend");

	shader.linkNodes(vertexTextureCoordNodeId, textureNodeId, 0, 0);
	shader.linkNodes(textureNodeId, blendNodeId, 0, 0);
	shader.linkNodes(vertexColorNodeId, blendNodeId, 0, 1);

	shader.setNodeParameter(textureNodeId, "shader_param_image", ShaderParameter::Value::ImageResourceValue(imageResourceId, imageResourceFilepath));
	shader.setNodeParameter(blendNodeId, "shader_param_blendmode", ShaderParameter::Value::EnumValue(colorBlendMode));

	shader.setNodePosition(outputNodeId, vec2d(700.0, 0.0));
	shader.setNodePosition(vertexTextureCoordNodeId, vec2d(0.0, 0.0));
	shader.setNodePosition(vertexColorNodeId, vec2d(0.0, 200.0));
	shader.setNodePosition(textureNodeId, vec2d(250.0, 0.0));
	shader.setNodePosition(blendNodeId, vec2d(400.0, 200.0));

	if(alphaThreshold > 0.0) {
		uint32_t splitNodeId = shader.addNode("shader_node_split4");
		uint32_t mergeNodeId = shader.addNode("shader_node_merge4");
		uint32_t thresholdNodeId = shader.addNode("shader_node_threshold");

		shader.linkNodes(blendNodeId, splitNodeId, 0, 0);
		shader.linkNodes(splitNodeId, mergeNodeId, 0, 0);
		shader.linkNodes(splitNodeId, mergeNodeId, 1, 1);
		shader.linkNodes(splitNodeId, mergeNodeId, 2, 2);
		shader.linkNodes(splitNodeId, thresholdNodeId, 3, 0);
		shader.linkNodes(thresholdNodeId, mergeNodeId, 0, 3);
		shader.linkNodes(mergeNodeId, outputNodeId, 0, 0);

		shader.setNodeParameter(thresholdNodeId, "shader_param_threshold", ShaderParameter::Value::FloatValue(alphaThreshold));
		shader.setNodeParameter(thresholdNodeId, "shader_param_output", ShaderParameter::Value::FloatValue(0.0));

		shader.setNodePosition(splitNodeId, vec2d(250.0, 400.0));
		shader.setNodePosition(thresholdNodeId, vec2d(400.0, 400.0));
		shader.setNodePosition(mergeNodeId, vec2d(700.0, 400.0));
	}
	else {
		shader.linkNodes(blendNodeId, outputNodeId, 0, 0);
	}

	return shader;
}

ShaderGraph ShaderGraphBuilder::buildSpriteSheetStaticShaderGraph(const std::string& imageResourceId, const std::string& imageResourceFilepath,
uint32_t numFramesRow, uint32_t numFramesColumn, uint32_t textureOrigin, uint32_t frame, bool randomFrame,
int32_t colorBlendMode, floatd alphaThreshold) {
	ShaderGraph shader;
	uint32_t outputNodeId = shader.addNode("shader_node_output");
	uint32_t vertexTextureCoordNodeId = shader.addNode("shader_node_vertex_texturecoord");
	uint32_t vertexColorNodeId = shader.addNode("shader_node_vertex_color");
	uint32_t textureNodeId = shader.addNode("shader_node_texture_spritesheet");
	uint32_t blendNodeId = shader.addNode("shader_node_blend");

	shader.linkNodes(vertexTextureCoordNodeId, textureNodeId, 0, 0);
	shader.linkNodes(vertexColorNodeId, blendNodeId, 0, 1);
	shader.linkNodes(textureNodeId, blendNodeId, 0, 0);

	shader.setNodeParameter(textureNodeId, "shader_param_image", ShaderParameter::Value::ImageResourceValue(imageResourceId, imageResourceFilepath));
	shader.setNodeParameter(textureNodeId, "shader_param_rownum", ShaderParameter::Value::IntValue(static_cast<int32_t>(numFramesRow)));
	shader.setNodeParameter(textureNodeId, "shader_param_columnnum", ShaderParameter::Value::IntValue(static_cast<int32_t>(numFramesColumn)));
	shader.setNodeParameter(textureNodeId, "shader_param_texorigin", ShaderParameter::Value::EnumValue(static_cast<int32_t>(textureOrigin)));
	shader.setNodeParameter(blendNodeId, "shader_param_blendmode", ShaderParameter::Value::EnumValue(colorBlendMode));

	shader.setNodePosition(outputNodeId, vec2d(900.0, 00.0));
	shader.setNodePosition(vertexTextureCoordNodeId, vec2d(00.0, 00.0));
	shader.setNodePosition(vertexColorNodeId, vec2d(0.0, 200.0));
	shader.setNodePosition(textureNodeId, vec2d(250.0, 0.0));
	shader.setNodePosition(blendNodeId, vec2d(600.0, 350.0));

	if(randomFrame) {
		uint32_t vertexObjectIdNodeId = shader.addNode("shader_node_vertex_objectid");
		uint32_t frameCountNumberNodeId = shader.addNode("shader_node_integer");
		uint32_t modNodeId = shader.addNode("shader_node_mod"); 

		shader.linkNodes(vertexObjectIdNodeId, modNodeId, 0, 0);
		shader.linkNodes(frameCountNumberNodeId, modNodeId, 0, 1);
		shader.linkNodes(modNodeId, textureNodeId, 0, 1);

		shader.setNodeParameter(frameCountNumberNodeId, "shader_param_integer", ShaderParameter::Value::IntValue(static_cast<int32_t>(numFramesRow * numFramesColumn)));
	
		shader.setNodePosition(vertexObjectIdNodeId, vec2d(0.0, 350.0));
		shader.setNodePosition(frameCountNumberNodeId, vec2d(0.0, 500.0));
		shader.setNodePosition(modNodeId, vec2d(250.0, 350.0));
	}
	else {
		uint32_t frameNumberNodeId = shader.addNode("shader_node_integer");

		shader.linkNodes(frameNumberNodeId, textureNodeId, 0, 1);

		shader.setNodeParameter(frameNumberNodeId, "shader_param_integer", ShaderParameter::Value::IntValue(static_cast<int32_t>(frame)));

		shader.setNodePosition(frameNumberNodeId, vec2d(250.0, 350.0));
	}

	if(alphaThreshold > 0.0) {
		uint32_t splitNodeId = shader.addNode("shader_node_split4");
		uint32_t mergeNodeId = shader.addNode("shader_node_merge4");
		uint32_t thresholdNodeId = shader.addNode("shader_node_threshold");

		shader.linkNodes(blendNodeId, splitNodeId, 0, 0);
		shader.linkNodes(splitNodeId, mergeNodeId, 0, 0);
		shader.linkNodes(splitNodeId, mergeNodeId, 1, 1);
		shader.linkNodes(splitNodeId, mergeNodeId, 2, 2);
		shader.linkNodes(splitNodeId, thresholdNodeId, 3, 0);
		shader.linkNodes(thresholdNodeId, mergeNodeId, 0, 3);
		shader.linkNodes(mergeNodeId, outputNodeId, 0, 0);

		shader.setNodeParameter(thresholdNodeId, "shader_param_threshold", ShaderParameter::Value::FloatValue(alphaThreshold));
		shader.setNodeParameter(thresholdNodeId, "shader_param_output", ShaderParameter::Value::FloatValue(0.0));

		shader.setNodePosition(splitNodeId, vec2d(250.0, 600.0));
		shader.setNodePosition(thresholdNodeId, vec2d(400.0, 600.0));
		shader.setNodePosition(mergeNodeId, vec2d(700.0, 600.0));
	}
	else {
		shader.linkNodes(blendNodeId, outputNodeId, 0, 0);
	}

	return shader;
}

ShaderGraph ShaderGraphBuilder::buildSpriteSheetAnimationShaderGraph(const std::string& imageResourceId, const std::string& imageResourceFilepath,
uint32_t numFramesRow, uint32_t numFramesColumn, uint32_t textureOrigin, uint32_t frame,
uint32_t numAnimationFrames, floatd animationDuration, bool animationLoop,
int32_t colorBlendMode, floatd alphaThreshold) {
	ShaderGraph shader;
	uint32_t outputNodeId = shader.addNode("shader_node_output");
	uint32_t vertexTextureCoordNodeId = shader.addNode("shader_node_vertex_texturecoord");
	uint32_t vertexColorNodeId = shader.addNode("shader_node_vertex_color");
	uint32_t vertexLifeNodeId = shader.addNode("shader_node_vertex_life");
	uint32_t textureNodeId = shader.addNode("shader_node_texture_spritesheet");
	uint32_t animationNodeId = shader.addNode("shader_node_frameanimation");
	uint32_t blendNodeId = shader.addNode("shader_node_blend");

	shader.linkNodes(vertexTextureCoordNodeId, textureNodeId, 0, 0);
	shader.linkNodes(vertexColorNodeId, blendNodeId, 0, 1);
	shader.linkNodes(vertexLifeNodeId, animationNodeId, 0, 0);
	shader.linkNodes(animationNodeId, textureNodeId, 0, 1);
	shader.linkNodes(textureNodeId, blendNodeId, 0, 0);

	shader.setNodeParameter(textureNodeId, "shader_param_image", ShaderParameter::Value::ImageResourceValue(imageResourceId, imageResourceFilepath));
	shader.setNodeParameter(textureNodeId, "shader_param_rownum", ShaderParameter::Value::IntValue(static_cast<int32_t>(numFramesRow)));
	shader.setNodeParameter(textureNodeId, "shader_param_columnnum", ShaderParameter::Value::IntValue(static_cast<int32_t>(numFramesColumn)));
	shader.setNodeParameter(textureNodeId, "shader_param_texorigin", ShaderParameter::Value::EnumValue(static_cast<int32_t>(textureOrigin)));
	shader.setNodeParameter(animationNodeId, "shader_param_frames", ShaderParameter::Value::IntValue(static_cast<int32_t>(numAnimationFrames)));
	shader.setNodeParameter(animationNodeId, "shader_param_startframe", ShaderParameter::Value::IntValue(static_cast<int32_t>(frame)));
	shader.setNodeParameter(animationNodeId, "shader_param_duration", ShaderParameter::Value::FloatValue(animationDuration));
	shader.setNodeParameter(animationNodeId, "shader_param_loop", ShaderParameter::Value::BoolValue(animationLoop));
	shader.setNodeParameter(blendNodeId, "shader_param_blendmode", ShaderParameter::Value::EnumValue(colorBlendMode));

	if(alphaThreshold > 0.0) {
		uint32_t splitNodeId = shader.addNode("shader_node_split4");
		uint32_t mergeNodeId = shader.addNode("shader_node_merge4");
		uint32_t thresholdNodeId = shader.addNode("shader_node_threshold");

		shader.linkNodes(blendNodeId, splitNodeId, 0, 0);
		shader.linkNodes(splitNodeId, mergeNodeId, 0, 0);
		shader.linkNodes(splitNodeId, mergeNodeId, 1, 1);
		shader.linkNodes(splitNodeId, mergeNodeId, 2, 2);
		shader.linkNodes(splitNodeId, thresholdNodeId, 3, 0);
		shader.linkNodes(thresholdNodeId, mergeNodeId, 0, 3);
		shader.linkNodes(mergeNodeId, outputNodeId, 0, 0);

		shader.setNodeParameter(thresholdNodeId, "shader_param_threshold", ShaderParameter::Value::FloatValue(alphaThreshold));
		shader.setNodeParameter(thresholdNodeId, "shader_param_output", ShaderParameter::Value::FloatValue(0.0));

		shader.setNodePosition(splitNodeId, vec2d(250.0, 600.0));
		shader.setNodePosition(thresholdNodeId, vec2d(400.0, 600.0));
		shader.setNodePosition(mergeNodeId, vec2d(700.0, 600.0));
	}
	else {
		shader.linkNodes(blendNodeId, outputNodeId, 0, 0);
	}

	shader.setNodePosition(outputNodeId, vec2d(900.0, 00.0));
	shader.setNodePosition(vertexTextureCoordNodeId, vec2d(00.0, 00.0));
	shader.setNodePosition(vertexColorNodeId, vec2d(0.0, 200.0));
	shader.setNodePosition(vertexLifeNodeId, vec2d(0.0, 400.0));
	shader.setNodePosition(animationNodeId, vec2d(250.0, 350.0));
	shader.setNodePosition(textureNodeId, vec2d(250.0, 0.0));
	shader.setNodePosition(blendNodeId, vec2d(600.0, 350.0));

	return shader;
}
}