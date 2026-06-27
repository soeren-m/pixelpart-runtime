#include "EffectAsset.h"
#include "../common/Serialization.h"
#include "../common/Transform.h"
#include "../math/MatrixCommon.h"
#include "../effect/Node.h"
#include "../effect/ParticleType.h"
#include <memory>
#include <unordered_set>

namespace pixelpart {
namespace {
void migrateEffectAssetJson(nlohmann::ordered_json& jsonData) {
	std::uint32_t version = jsonData.value("version", 0u);
	if(jsonData.contains("project")) {
		version = jsonData.at("project").value("version", version);
	}

	if(version == EffectAsset::version) {
		return;
	}
	else if(version < 7) {
		throw DeserializationException("Unsupported effect asset version " + std::to_string(version));
	}

	// Migration 1.6.x -> 1.7.x
	if(version == 7) {
		const auto jsonValueToStaticProperty = [](const nlohmann::ordered_json& jValue) {
			return nlohmann::ordered_json({
				{ "value", jValue },
				{ "compute_graph", {
					{ "nodes", nlohmann::ordered_json::array({
						nlohmann::ordered_json::array({
							0u,
							{
								{ "type", "compute_node_output" },
								{ "name", "" },
								{ "inputs", nlohmann::ordered_json::array({
									{
										{ "id", 4294967295u },
										{ "node", 4294967295u },
										{ "slot", 4294967295u }
									} })
								},
								{ "parameter_values", nlohmann::ordered_json::array() },
								{ "position", {
									{ "x", 0.0 },
									{ "y", 0.0 }
								} }
							}
						}) })
					}
				} },
				{ "compute_operation", "set" },
			});
		};
		const auto jsonCurveToAnimatedProperty = [](const nlohmann::ordered_json& jCurve) {
			if(!jCurve.is_object()) {
				return nlohmann::ordered_json(nullptr);
			}

			return nlohmann::ordered_json({
				{ "curve", jCurve },
				{ "compute_graph", {
					{ "nodes", nlohmann::ordered_json::array({
						nlohmann::ordered_json::array({
							0u,
							{
								{ "type", "compute_node_output" },
								{ "name", "" },
								{ "inputs", nlohmann::ordered_json::array({
									{
										{ "id", 4294967295u },
										{ "node", 4294967295u },
										{ "slot", 4294967295u }
									} })
								},
								{ "parameter_values", nlohmann::ordered_json::array() },
								{ "position", {
									{ "x", 0.0 },
									{ "y", 0.0 }
								} }
							}
						}) })
					}
				} },
				{ "compute_operation", "set" },
				{ "compute_target", {
					{ "type", "result" },
					{ "index", 0u }
				} }
			});
		};
		const auto jsonAnimatedPropertyFromValue = [](float_t t, const nlohmann::ordered_json& v) {
			return nlohmann::ordered_json({
				{ "curve", {
					{ "interpolation", "linear" },
					{ "points", nlohmann::ordered_json::array({ { t, v } })}
				} },
				{ "compute_graph", {
					{ "nodes", nlohmann::ordered_json::array({
						nlohmann::ordered_json::array({
							0u,
							{
								{ "type", "compute_node_output" },
								{ "name", "" },
								{ "inputs", nlohmann::ordered_json::array({
									{
										{ "id", 4294967295u },
										{ "node", 4294967295u },
										{ "slot", 4294967295u }
									} })
								},
								{ "parameter_values", nlohmann::ordered_json::array() },
								{ "position", {
									{ "x", 0.0 },
									{ "y", 0.0 }
								} }
							}
						}) })
					}
				} },
				{ "compute_operation", "set" },
				{ "compute_target", {
					{ "type", "result" },
					{ "index", 0u }
				}}
			});
		};

		jsonData["image_effects"]["effects"] = jsonData["project"]["postprocessing_pipeline"]["effects"];
		for(nlohmann::ordered_json& jImageEffect : jsonData["image_effects"]["effects"]) {
			jImageEffect["visible"] = true;
		}

		jsonData["camera"] = nlohmann::ordered_json({
			{ "position", jsonData["project"]["camera_position"] },
			{ "zoom", jsonData["project"]["camera_zoom"] },
			{ "fov", jsonData["project"]["camera_fov"] },
			{ "pitch", jsonData["project"]["camera_pitch"] },
			{ "yaw", jsonData["project"]["camera_yaw"] },
			{ "roll", jsonData["project"]["camera_roll"] },
		});

		float4_t backgroundColor = jsonData["project"]["background_color"].get<float4_t>();

		jsonData["render"] = jsonData["project"]["render_settings"];
		jsonData["render"]["ambient_lighting"] = float3_t(0.1);
		jsonData["render"]["hdr_enabled"] = false;
		jsonData["render"]["hdr_bloom_intensity"] = 0.1;
		jsonData["render"]["hdr_exposure"] = 1.0;

		jsonData["preview"] = jsonData["project"]["preview_settings"];
		jsonData["preview"]["ambient_lighting"] = float3_t(0.1);
		jsonData["preview"]["hdr_enabled"] = false;
		jsonData["preview"]["hdr_bloom_intensity"] = 0.1;
		jsonData["preview"]["hdr_exposure"] = 1.0;

		if(backgroundColor.w < 0.5) {
			jsonData["render"]["background_enabled"] = false;
			jsonData["preview"]["background_enabled"] = false;
		}

		backgroundColor.w = 1.0;
		jsonData["render"]["background_color"] = backgroundColor;
		jsonData["preview"]["background_color"] = backgroundColor;

		jsonData["effect"] = nlohmann::ordered_json::object();
		jsonData["effect"]["3d"] = jsonData["project"]["effect"]["3d"];

		jsonData["effect"]["resources"]["images"] = jsonData["resources"]["images"];
		for(nlohmann::ordered_json& jImage : jsonData["effect"]["resources"]["images"]) {
			jImage["name"] = "unknown";
		}

		jsonData["effect"]["resources"]["meshes"] = nlohmann::ordered_json::object();
		jsonData["effect"]["resources"]["materials"] = nlohmann::ordered_json::object();
		jsonData["effect"]["resources"]["vector_fields"] = nlohmann::ordered_json::object();

		jsonData["effect"]["particle_emitters"] = jsonData["project"]["effect"]["particle_emitters"];
		jsonData["effect"]["particle_types"] = jsonData["project"]["effect"]["particle_types"];
		jsonData["effect"]["force_fields"] = jsonData["project"]["effect"]["force_fields"];
		jsonData["effect"]["colliders"] = jsonData["project"]["effect"]["colliders"];
		jsonData["effect"]["light_sources"] = nlohmann::ordered_json::array();
		jsonData["effect"]["inputs"] = nlohmann::ordered_json::array();

		for(nlohmann::ordered_json& jParticleEmitter : jsonData["effect"]["particle_emitters"]) {
			jParticleEmitter["position"] = jsonCurveToAnimatedProperty(jParticleEmitter["position"]);
			jParticleEmitter["size"] = jsonCurveToAnimatedProperty(jParticleEmitter["size"]);
			jParticleEmitter["orientation"] = jsonCurveToAnimatedProperty(jParticleEmitter["orientation"]);
			jParticleEmitter["direction"] = jsonCurveToAnimatedProperty(jParticleEmitter["direction"]);
			jParticleEmitter["spread"] = jsonCurveToAnimatedProperty(jParticleEmitter["spread"]);
			jParticleEmitter["grid_order"] = "x_y_z";
			jParticleEmitter["grid_size_x"] = 5u;
			jParticleEmitter["grid_size_y"] = 5u;
			jParticleEmitter["grid_size_z"] = 5u;
		}

		for(nlohmann::ordered_json& jParticleType : jsonData["effect"]["particle_types"]) {
			jParticleType["num_particles"] = jsonCurveToAnimatedProperty(jParticleType["num_particles"]);
			jParticleType["lifespan"] = jsonCurveToAnimatedProperty(jParticleType["lifespan"]);
			jParticleType["lifespan_variance"] = jsonValueToStaticProperty(jParticleType["lifespan_variance"]);
			jParticleType["motion_path_force"] = jsonValueToStaticProperty(jParticleType["motion_path_force"]);
			jParticleType["initial_velocity"] = jsonCurveToAnimatedProperty(jParticleType["initial_velocity"]);
			jParticleType["inherited_velocity"] = jsonAnimatedPropertyFromValue(0.5, nlohmann::ordered_json(0.0));
			jParticleType["velocity_variance"] = jsonValueToStaticProperty(jParticleType["velocity_variance"]);
			jParticleType["acceleration"] = jsonCurveToAnimatedProperty(jParticleType["acceleration"]);
			jParticleType["radial_acceleration"] = jsonCurveToAnimatedProperty(jParticleType["radial_acceleration"]);
			jParticleType.erase("damping");
			jParticleType["initial_rotation"] = jsonCurveToAnimatedProperty(jParticleType["initial_rotation"]);
			jParticleType["rotation"] = jsonCurveToAnimatedProperty(jParticleType["rotation"]);
			jParticleType["rotation_by_speed"] = jsonAnimatedPropertyFromValue(0.5, nlohmann::ordered_json(float3_t(0.0)));
			jParticleType["rotation_variance"] = jsonValueToStaticProperty(jParticleType["rotation_variance"]);
			jParticleType["angular_velocity_variance"] = jsonValueToStaticProperty(jParticleType["angular_velocity_variance"]);
			jParticleType["pivot"] = jsonValueToStaticProperty(jParticleType["pivot"]);
			jParticleType["physical_size"] = jsonAnimatedPropertyFromValue(0.5, nlohmann::ordered_json(1.0));
			jParticleType["weight"] = jsonCurveToAnimatedProperty(jParticleType["weight"]);
			jParticleType["bounce"] = jsonCurveToAnimatedProperty(jParticleType["bounce"]);
			jParticleType["friction"] = jsonCurveToAnimatedProperty(jParticleType["friction"]);
			jParticleType["initial_size"] = jsonCurveToAnimatedProperty(jParticleType["initial_size"]);
			jParticleType["size"] = jsonCurveToAnimatedProperty(jParticleType["size"]);
			jParticleType["size_variance"] = jsonValueToStaticProperty(jParticleType["size_variance"]);
			jParticleType["stretch"] = jsonAnimatedPropertyFromValue(0.5, nlohmann::ordered_json(float3_t(0.0)));
			jParticleType["color"] = jsonCurveToAnimatedProperty(jParticleType["color"]);
			jParticleType["color_variance"] = jsonValueToStaticProperty(jParticleType["color_variance"]);
			jParticleType["initial_opacity"] = jsonCurveToAnimatedProperty(jParticleType["initial_opacity"]);
			jParticleType["opacity"] = jsonCurveToAnimatedProperty(jParticleType["opacity"]);
			jParticleType["opacity_variance"] = jsonValueToStaticProperty(jParticleType["opacity_variance"]);

			jParticleType["sprite_renderer_settings"].erase("sort_passes");

			std::uint32_t textureRotationIndex = jParticleType["trail_renderer_settings"]["texture_rotation"];
			switch(textureRotationIndex) {
				case 1:
					jParticleType["trail_renderer_settings"]["texture_rotation"] = "left";
					break;
				case 2:
					jParticleType["trail_renderer_settings"]["texture_rotation"] = "down";
					break;
				case 3:
					jParticleType["trail_renderer_settings"]["texture_rotation"] = "right";
					break;
				default:
					jParticleType["trail_renderer_settings"]["texture_rotation"] = "up";
					break;
			}

			jParticleType["trail_renderer_settings"]["smoothing_method"] = jParticleType["trail_renderer_settings"]["num_trail_segments"].get<std::uint32_t>() > 0
				? "spline"
				: "none";
			jParticleType["trail_renderer_settings"]["smoothing_segments"] = jParticleType["trail_renderer_settings"]["num_trail_segments"];
			jParticleType["trail_renderer_settings"].erase("num_trail_segments");

			jParticleType["mesh_renderer_settings"]["mesh_resource"] = "";
			jParticleType["mesh_renderer_settings"]["sort_criterion"] = "none";

			std::string renderer = jParticleType["renderer"];
			std::string blendMode = jParticleType["blend_mode"];

			std::string materialId;
			if(renderer == "trail") {
				materialId = blendMode == "additive" ? "TrailUnlitAdditive" : "TrailUnlitAlpha";
			}
			else {
				materialId = blendMode == "additive" ? "SpriteUnlitAdditive" : "SpriteUnlitAlpha";
			}

			std::string mainTextureImageId;
			for(const auto& jShaderNode : jParticleType["shader"]["nodes"]) {
				std::string shaderNodeType = jShaderNode[1]["type"];
				if(shaderNodeType == "shader_node_texture_image") {
					std::string imageParameterValue = jShaderNode[1]["parameters"][0]["value"];

					std::size_t pathDelimPos = imageParameterValue.find('@');
					mainTextureImageId = pathDelimPos != std::string::npos
						? imageParameterValue.substr(0, pathDelimPos)
						: imageParameterValue;

					break;
				}
			}

			nlohmann::ordered_json jMaterialParameters;
			if(renderer == "trail") {
				jMaterialParameters = nlohmann::ordered_json::array({
					nlohmann::ordered_json::array({ 0u,
						{
							{ "type", "resource_image" },
							{ "value", mainTextureImageId }
						}
					}),
					nlohmann::ordered_json::array({ 10u,
						{
							{ "type", "enum" },
							{ "value", 0 }
						}
					}),
					nlohmann::ordered_json::array({ 20u,
						{
							{ "type", "float3" },
							{ "value", {
								{ "x", 0.0 },
								{ "y", 0.0 },
								{ "z", 0.0 }
							} }
						}
					}),
					nlohmann::ordered_json::array({ 40u,
						{
							{ "type", "bool" },
							{ "value", 0 }
						}
					}),
					nlohmann::ordered_json::array({ 41u,
						{
							{ "type", "float" },
							{ "value", 0.1 }
						}
					}),
					nlohmann::ordered_json::array({ 42u,
						{
							{ "type", "bool" },
							{ "value", 0 }
						}
					}),
					nlohmann::ordered_json::array({ 43u,
						{
							{ "type", "float" },
							{ "value", 0.1 }
						}
					})
				});
			}
			else {
				jMaterialParameters = nlohmann::ordered_json::array({
					nlohmann::ordered_json::array({ 0u,
						{
							{ "type", "resource_image" },
							{ "value", mainTextureImageId }
						}
					}),
					nlohmann::ordered_json::array({ 10u,
						{
							{ "type", "enum" },
							{ "value", 0 }
						}
					}),
					nlohmann::ordered_json::array({ 20u,
						{
							{ "type", "float3" },
							{ "value", {
								{ "x", 0.0 },
								{ "y", 0.0 },
								{ "z", 0.0 }
							} }
						}
					}),
					nlohmann::ordered_json::array({ 30u,
						{
							{ "type", "int" },
							{ "value", 1 }
						}
					}),
					nlohmann::ordered_json::array({ 31u,
						{
							{ "type", "int" },
							{ "value", 1 }
						}
					}),
					nlohmann::ordered_json::array({ 32u,
						{
							{ "type", "enum" },
							{ "value", 0 }
						}
					}),
					nlohmann::ordered_json::array({ 33u,
						{
							{ "type", "int" },
							{ "value", 1 }
						}
					}),
					nlohmann::ordered_json::array({ 34u,
						{
							{ "type", "int" },
							{ "value", 0 }
						}
					}),
					nlohmann::ordered_json::array({ 35u,
						{
							{ "type", "float" },
							{ "value", 1.0 }
						}
					}),
					nlohmann::ordered_json::array({ 36u,
						{
							{ "type", "bool" },
							{ "value", 0 }
						}
					}),
					nlohmann::ordered_json::array({ 40u,
						{
							{ "type", "bool" },
							{ "value", 0 }
						}
					}),
					nlohmann::ordered_json::array({ 41u,
						{
							{ "type", "float" },
							{ "value", 0.1 }
						}
					}),
					nlohmann::ordered_json::array({ 42u,
						{
							{ "type", "bool" },
							{ "value", 0 }
						}
					}),
					nlohmann::ordered_json::array({ 43u,
						{
							{ "type", "float" },
							{ "value", 0.1 }
						}
					})
				});
			}

			jParticleType["material_instance"] = nlohmann::ordered_json{
				{ "builtin_material", true },
				{ "material_id", materialId },
				{ "material_parameters", jMaterialParameters }
			};

			jParticleType.erase("shader");
			jParticleType.erase("blend_mode");
		}

		for(nlohmann::ordered_json& jForceField : jsonData["effect"]["force_fields"]) {
			jForceField["position"] = jsonCurveToAnimatedProperty(jForceField["position"]);

			std::string forceFieldType = jForceField["type"];
			if(forceFieldType == "area") {
				jForceField["type"] = "acceleration_field";
			}
			else {
				jForceField["type"] = "attraction_field";
			}

			jForceField["size"] = jsonCurveToAnimatedProperty(jForceField["size"]);
			jForceField["orientation"] = jsonCurveToAnimatedProperty(jForceField["orientation"]);
			jForceField["strength"] = jsonCurveToAnimatedProperty(jForceField["strength"]);

			jForceField["attraction_field"] = nlohmann::ordered_json(nullptr);
			jForceField["acceleration_field"]["direction"] = jsonCurveToAnimatedProperty(jForceField["direction"]);
			jForceField["acceleration_field"]["direction_variance"] = jsonAnimatedPropertyFromValue(0.5, jForceField["direction_variance"]);
			jForceField["acceleration_field"]["strength_variance"] = jsonAnimatedPropertyFromValue(0.5, jForceField["strength_variance"]);
			jForceField["acceleration_field"]["grid_size_x"] = jForceField["grid_size_x"];
			jForceField["acceleration_field"]["grid_size_y"] = jForceField["grid_size_y"];
			jForceField["acceleration_field"]["grid_size_z"] = jForceField["grid_size_z"];
			jForceField["acceleration_field"]["grid_direction"] = jForceField["grid_direction"];
			jForceField["acceleration_field"]["grid_strength"] = jForceField["grid_strength"];
			jForceField["vector_field"] = nlohmann::ordered_json::object();
			jForceField["noise_field"] = nlohmann::ordered_json::object();
			jForceField["drag_field"] = nlohmann::ordered_json::object();

			jForceField.erase("direction");
			jForceField.erase("direction_variance");
			jForceField.erase("strength_variance");
			jForceField.erase("grid_size_x");
			jForceField.erase("grid_size_y");
			jForceField.erase("grid_size_z");
			jForceField.erase("grid_direction");
			jForceField.erase("grid_strength");
		}

		for(nlohmann::ordered_json& jCollider : jsonData["effect"]["colliders"]) {
			jCollider["position"] = jsonCurveToAnimatedProperty(jCollider["position"]);
			jCollider["width"] = jsonValueToStaticProperty(nlohmann::ordered_json(1.0));
			jCollider["orientation"] = jsonValueToStaticProperty(nlohmann::ordered_json(0.0));
			jCollider["kill_on_contact"] = jsonValueToStaticProperty(nlohmann::ordered_json(false));
			jCollider["bounce"] = jsonCurveToAnimatedProperty(jCollider["bounce"]);
			jCollider["friction"] = jsonCurveToAnimatedProperty(jCollider["friction"]);
		}

		jsonData.erase("project");
		jsonData.erase("resources");

		version = 8;
		jsonData["version"] = 8;
	}

	// Migration 1.7.x -> 1.8.x
	if(version == 8) {
		const auto jsonAnimatedPropertyFromValue = [](float_t t, const nlohmann::ordered_json& v) {
			return nlohmann::ordered_json({
				{ "curve", {
					{ "interpolation", "linear" },
					{ "points", nlohmann::ordered_json::array({ { t, v } })}
				} },
				{ "compute_graph", nlohmann::ordered_json({ }) },
				{ "compute_operation", "set" },
				{ "compute_target", {
					{ "type", "result" },
					{ "index", 0u }
				}}
			});
		};
		const auto jsonApplyInverseRotation = [](nlohmann::ordered_json& jVectorProperty, const nlohmann::ordered_json& jRotationProperty) {
			Transform rotationTransform(float3_t(0.0), jRotationProperty["curve"]["points"][0][1].get<float3_t>(), float3_t(1.0));
			matrix4_t inverseRotationMatrix = math::inverse(rotationTransform.matrix());

			for(auto& keyframe : jVectorProperty["curve"]["points"]) {
				Transform transform(float3_t(0.0), keyframe[1].get<float3_t>(), float3_t(1.0));

				float3_t newValue = Transform(inverseRotationMatrix * transform.matrix()).rotation();
				keyframe[1]["x"] = newValue.x;
				keyframe[1]["y"] = newValue.y;
				keyframe[1]["z"] = newValue.z;
			}
		};

		nlohmann::ordered_json jScene = nlohmann::ordered_json::array();
		nlohmann::ordered_json jParticles = nlohmann::ordered_json::array();

		nlohmann::ordered_json jRootNode;
		jRootNode["id"] = 0u;
		jRootNode["parent_id"] = nullptr;
		jRootNode["name"] = "Root";
		jRootNode["lifetime_start"] = 0.0;
		jRootNode["lifetime_duration"] = 1.0;
		jRootNode["repeat"] = true;
		jRootNode["start_trigger"] = nullptr;
		jRootNode["stop_trigger"] = nullptr;
		jRootNode["position"] = jsonAnimatedPropertyFromValue(0.0, nlohmann::ordered_json(float3_t(0.0)));
		jRootNode["rotation"] = jsonAnimatedPropertyFromValue(0.5, nlohmann::ordered_json(float3_t(0.0)));
		jRootNode["scale"] = jsonAnimatedPropertyFromValue(0.5, nlohmann::ordered_json(float3_t(1.0)));
		jRootNode["node_type"] = "group";
		jScene.push_back(jRootNode);

		id_t nextNodeId = 1;
		std::unordered_map<id_t, id_t> newParticleEmitterIds;
		std::unordered_map<id_t, id_t> particleEmitterParentTypeIds;
		std::unordered_map<id_t, id_t> particleTypeParentEmitterIds;

		for(nlohmann::ordered_json& jParticleEmitter : jsonData["effect"]["particle_emitters"]) {
			id_t parentParticleTypeId = jParticleEmitter.value("parent_id", id_t());

			id_t oldId = jParticleEmitter["id"].get<id_t>();
			id_t newId = nextNodeId++;
			newParticleEmitterIds[oldId] = newId;

			nlohmann::ordered_json jNode;
			jNode["id"] = newId;
			jNode["parent_id"] = 0u;
			jNode["name"] = jParticleEmitter["name"];
			jNode["lifetime_start"] = jParticleEmitter["lifetime_start"];
			jNode["lifetime_duration"] = jParticleEmitter["lifetime_duration"];
			jNode["repeat"] = jParticleEmitter["repeat"];
			jNode["start_trigger"] = nullptr;
			jNode["stop_trigger"] = nullptr;
			jNode["position"] = jParticleEmitter["position"];
			jNode["rotation"] = jParticleEmitter["orientation"];
			jNode["scale"] = jParticleEmitter["size"];
			jNode["node_type"] = "particle_emitter";
			jNode["primary"] = !parentParticleTypeId.valid();
			jNode["particle_types"] = nlohmann::ordered_json::array();
			jNode["shape"] = jParticleEmitter["shape"];
			jNode["path"] = jParticleEmitter["path"];
			jNode["distribution"] = jParticleEmitter["distribution"];
			jNode["grid_order"] = jParticleEmitter["grid_order"];
			jNode["grid_size_x"] = jParticleEmitter["grid_size_x"];
			jNode["grid_size_y"] = jParticleEmitter["grid_size_y"];
			jNode["grid_size_z"] = jParticleEmitter["grid_size_z"];
			jNode["emission_mode"] = jParticleEmitter["emission_mode"];
			jNode["direction_mode"] = jParticleEmitter["direction_mode"];
			jNode["direction"] = jParticleEmitter["direction"];
			jNode["spread"] = jParticleEmitter["spread"];

			if(jParticleEmitter["direction_mode"].get<std::string>() == "fixed") {
				jsonApplyInverseRotation(jNode["direction"], jNode["rotation"]);
			}

			if(parentParticleTypeId) {
				particleEmitterParentTypeIds[newId] = parentParticleTypeId;
			}

			jScene.push_back(jNode);
		}

		for(nlohmann::ordered_json& jForceField : jsonData["effect"]["force_fields"]) {
			id_t newId = nextNodeId++;

			nlohmann::ordered_json jNode;
			jNode["id"] = newId;
			jNode["parent_id"] = 0u;
			jNode["name"] = jForceField["name"];
			jNode["lifetime_start"] = jForceField["lifetime_start"];
			jNode["lifetime_duration"] = jForceField["lifetime_duration"];
			jNode["repeat"] = jForceField["repeat"];
			jNode["start_trigger"] = nullptr;
			jNode["stop_trigger"] = nullptr;
			jNode["position"] = jForceField["position"];
			jNode["rotation"] = jForceField["orientation"];
			jNode["scale"] = jForceField["size"];
			jNode["node_type"] = "force_field";
			jNode["exclusion_list"] = jForceField["exclusion_list"];
			jNode["strength"] = jForceField["strength"];

			float3_t sizeValue = jForceField["size"]["curve"]["points"][0][1].get<float3_t>();
			if(sizeValue.x < 0.0 || sizeValue.y < 0.0 || sizeValue.z < 0.0) {
				jNode["infinite"] = true;
				jNode["scale"] = jsonAnimatedPropertyFromValue(0.5, nlohmann::ordered_json(float3_t(1.0)));
			}
			else {
				jNode["infinite"] = false;
			}

			std::string forceFieldType = jForceField.value("type", "attraction_field");

			if(forceFieldType == "acceleration_field") {
				jNode["force_field_type"] = "acceleration";

				jNode["acceleration"]["direction"] = jForceField["acceleration_field"]["direction"];
				jNode["acceleration"]["direction_variance"] = jForceField["acceleration_field"]["direction_variance"];
				jNode["acceleration"]["strength_variance"] = jForceField["acceleration_field"]["strength_variance"];
				jNode["acceleration"]["grid_size_x"] = jForceField["acceleration_field"]["grid_size_x"];
				jNode["acceleration"]["grid_size_y"] = jForceField["acceleration_field"]["grid_size_y"];
				jNode["acceleration"]["grid_size_z"] = jForceField["acceleration_field"]["grid_size_z"];
				jNode["acceleration"]["grid_direction"] = jForceField["acceleration_field"]["grid_direction"];
				jNode["acceleration"]["grid_strength"] = jForceField["acceleration_field"]["grid_strength"];

				jsonApplyInverseRotation(jNode["acceleration"]["direction"], jNode["rotation"]);
			}
			else if(forceFieldType == "vector_field") {
				jNode["force_field_type"] = "vector";

				jNode["vector"]["resource_id"] = jForceField["vector_field"]["resource_id"];
				jNode["vector"]["filter"] = jForceField["vector_field"]["filter"];
				jNode["vector"]["tightness"] = jForceField["vector_field"]["tightness"];
			}
			else if(forceFieldType == "noise_field") {
				jNode["force_field_type"] = "noise";

				jNode["noise"]["octaves"] = jForceField["noise_field"]["octaves"];
				jNode["noise"]["frequency"] = jForceField["noise_field"]["frequency"];
				jNode["noise"]["persistence"] = jForceField["noise_field"]["persistence"];
				jNode["noise"]["lacunarity"] = jForceField["noise_field"]["lacunarity"];
				jNode["noise"]["animated"] = jForceField["noise_field"]["animated"];
				jNode["noise"]["animation_time_scale"] = jForceField["noise_field"]["animation_time_scale"];
				jNode["noise"]["animation_time_base"] = jForceField["noise_field"]["animation_time_base"];
			}
			else if(forceFieldType == "drag_field") {
				jNode["force_field_type"] = "drag";

				jNode["drag"]["velocity_influence"] = jForceField["drag_field"]["velocity_influence"];
				jNode["drag"]["size_influence"] = jForceField["drag_field"]["size_influence"];
			}
			else {
				jNode["force_field_type"] = "attraction";
			}

			jScene.push_back(jNode);
		}

		for(nlohmann::ordered_json& jCollider : jsonData["effect"]["colliders"]) {
			id_t newId = nextNodeId++;

			nlohmann::ordered_json jNode;
			jNode["id"] = newId;
			jNode["parent_id"] = 0u;
			jNode["name"] = jCollider["name"];
			jNode["lifetime_start"] = jCollider["lifetime_start"];
			jNode["lifetime_duration"] = jCollider["lifetime_duration"];
			jNode["repeat"] = jCollider["repeat"];
			jNode["start_trigger"] = nullptr;
			jNode["stop_trigger"] = nullptr;
			jNode["position"] = jCollider["position"];
			jNode["node_type"] = "collider";
			jNode["exclusion_list"] = jCollider["exclusion_list"];
			jNode["kill_on_contact"] = jCollider["kill_on_contact"];
			jNode["bounce"] = jCollider["bounce"];
			jNode["friction"] = jCollider["friction"];

			if(jsonData["effect"]["3d"].get<bool>()) {
				jNode["collider_type"] = "plane";
				jNode["rotation"] = jsonAnimatedPropertyFromValue(0.5, float3_t(jCollider["orientation"]["value"].get<float_t>(), 1.0, 1.0));
				jNode["scale"] = jsonAnimatedPropertyFromValue(0.5, float3_t(jCollider["width"]["value"].get<float_t>(), 1.0, 1.0));
			}
			else {
				jNode["collider_type"] = "line";
				jNode["rotation"] = jsonAnimatedPropertyFromValue(0.5, float3_t(0.0));
				jNode["scale"] = jsonAnimatedPropertyFromValue(0.5, float3_t(1.0));
				jNode["points"] = jCollider["points"];
			}

			jScene.push_back(jNode);
		}

		for(nlohmann::ordered_json& jLightSource : jsonData["effect"]["light_sources"]) {
			id_t newId = nextNodeId++;

			nlohmann::ordered_json jNode;
			jNode["id"] = newId;
			jNode["parent_id"] = 0u;
			jNode["name"] = jLightSource["name"];
			jNode["lifetime_start"] = jLightSource["lifetime_start"];
			jNode["lifetime_duration"] = jLightSource["lifetime_duration"];
			jNode["repeat"] = jLightSource["repeat"];
			jNode["start_trigger"] = nullptr;
			jNode["stop_trigger"] = nullptr;
			jNode["position"] = jLightSource["position"];
			jNode["rotation"] = jLightSource["direction"];
			jNode["scale"] = jsonAnimatedPropertyFromValue(0.5, nlohmann::ordered_json(float3_t(1.0)));
			jNode["node_type"] = "light_source";
			jNode["attenuation"] = jLightSource["attenuation"];
			jNode["color"] = jLightSource["color"];
			jNode["intensity"] = jLightSource["intensity"];

			auto& jRange = jLightSource["range"];
			auto& jScale = jNode["scale"];
			jScale["curve"]["interpolation"] = jRange["curve"]["interpolation"];
			jScale["curve"]["points"] = nlohmann::ordered_json::array();
			for(const auto& jPoint : jRange["curve"]["points"]) {
				jScale["curve"]["points"].push_back(nlohmann::ordered_json::array({
					jPoint[0],
					nlohmann::ordered_json({
						{ "x", jPoint[1].get<float_t>() },
						{ "y", 1.0 },
						{ "z", 1.0 },
					})
				}));
			}

			std::string lightSourceType = jLightSource["type"];
			if(lightSourceType == "point") {
				jNode["light_source_type"] = "point";
			}
			else if(lightSourceType == "spot") {
				jNode["light_source_type"] = "spot";
				jNode["spot_angle"] = jLightSource["spot_angle"];
				jNode["spot_angle_attenuation"] = jLightSource["spot_angle_attenuation"];
			}
			else {
				jNode["light_source_type"] = "directional";
			}

			jScene.push_back(jNode);
		}

		for(nlohmann::ordered_json& jParticleType : jsonData["effect"]["particle_types"]) {
			id_t particleTypeId = jParticleType["id"];

			nlohmann::ordered_json jNewParticleType;
			jNewParticleType["id"] = particleTypeId;
			jNewParticleType["parent_id"] = nullptr;

			id_t oldParentId = jParticleType.value("parent_id", id_t());
			id_t emitterId;
			if(newParticleEmitterIds.count(oldParentId) != 0) {
				emitterId = newParticleEmitterIds.at(oldParentId);
			}

			jNewParticleType["name"] = jParticleType["name"];
			jNewParticleType["position_relative"] = jParticleType["position_relative"];
			jNewParticleType["rotation_mode"] = jParticleType["rotation_mode"];
			jNewParticleType["alignment_mode"] = jParticleType["alignment_mode"];
			jNewParticleType["pivot"] = jParticleType["pivot"];
			jNewParticleType["motion_path_force"] = jParticleType["motion_path_force"];
			jNewParticleType["visible"] = jParticleType["visible"];
			jNewParticleType["layer"] = jParticleType["layer"];
			jNewParticleType["material_instance"] = jParticleType["material_instance"];
			jNewParticleType["renderer"] = jParticleType["renderer"];
			jNewParticleType["sprite_renderer_settings"] = jParticleType["sprite_renderer_settings"];
			jNewParticleType["trail_renderer_settings"] = jParticleType["trail_renderer_settings"];
			jNewParticleType["mesh_renderer_settings"] = jParticleType["mesh_renderer_settings"];
			jNewParticleType["count"] = jParticleType["num_particles"];
			jNewParticleType["lifespan"] = jParticleType["lifespan"];
			jNewParticleType["initial_size"] = jParticleType["initial_size"];
			jNewParticleType["initial_rotation"] = jParticleType["initial_rotation"];
			jNewParticleType["initial_velocity"] = jParticleType["initial_velocity"];
			jNewParticleType["inherited_velocity"] = jParticleType["inherited_velocity"];
			jNewParticleType["initial_opacity"] = jParticleType["initial_opacity"];
			jNewParticleType["position"] = jParticleType["position"];
			jNewParticleType["size"] = jParticleType["size"];
			jNewParticleType["stretch"] = jParticleType["stretch"];
			jNewParticleType["rotation"] = jParticleType["rotation"];
			jNewParticleType["rotation_by_speed"] = jParticleType["rotation_by_speed"];
			jNewParticleType["acceleration"] = jParticleType["acceleration"];
			jNewParticleType["radial_acceleration"] = jParticleType["radial_acceleration"];
			jNewParticleType["physical_size"] = jParticleType.value("physical_size", jsonAnimatedPropertyFromValue(0.5, nlohmann::ordered_json(1.0)));
			jNewParticleType["weight"] = jParticleType["weight"];
			jNewParticleType["bounce"] = jParticleType["bounce"];
			jNewParticleType["friction"] = jParticleType["friction"];
			jNewParticleType["color"] = jParticleType["color"];
			jNewParticleType["opacity"] = jParticleType["opacity"];
			jNewParticleType["lifespan_variance"] = jParticleType["lifespan_variance"];
			jNewParticleType["size_variance"] = jParticleType["size_variance"];
			jNewParticleType["rotation_variance"] = jParticleType["rotation_variance"];
			jNewParticleType["angular_velocity_variance"] = jParticleType["angular_velocity_variance"];
			jNewParticleType["velocity_variance"] = jParticleType["velocity_variance"];
			jNewParticleType["color_variance"] = jParticleType["color_variance"];
			jNewParticleType["opacity_variance"] = jParticleType["opacity_variance"];

			for(auto& jNode : jScene) {
				if(jNode.value("id", id_t()) == emitterId) {
					jNode["particle_types"].push_back(particleTypeId);
				}
			}

			particleTypeParentEmitterIds[particleTypeId] = emitterId;

			jParticles.push_back(jNewParticleType);
		}

		for(nlohmann::ordered_json& jParticleType : jParticles) {
			id_t particleTypeId = jParticleType["id"];
			if(particleTypeParentEmitterIds.count(particleTypeId) == 0) {
				continue;
			}

			id_t parentEmitterId = particleTypeParentEmitterIds.at(particleTypeId);
			if(particleEmitterParentTypeIds.count(parentEmitterId) == 0) {
				continue;
			}

			id_t parentParticleTypeId = particleEmitterParentTypeIds.at(parentEmitterId);
			jParticleType["parent_id"] = parentParticleTypeId;
		}

		jsonData["effect"]["scene"] = jScene;
		jsonData["effect"]["particles"] = jParticles;
		jsonData["effect"]["triggers"] = nlohmann::ordered_json::array();
		jsonData["effect"].erase("particle_emitters");
		jsonData["effect"].erase("particle_types");
		jsonData["effect"].erase("force_fields");
		jsonData["effect"].erase("colliders");
		jsonData["effect"].erase("light_sources");

		for(nlohmann::ordered_json& jImage : jsonData["effect"]["resources"]["images"]) {
			jImage["color_space"] = "srgb";
			jImage["channels"] = jImage.value("bpp", 0u) / 8;
			jImage.erase("bpp");
		}

		version = 9;
		jsonData["version"] = 9;
	}

	// Migration 1.8.x -> 1.9.x
	if(version == 9) {
		nlohmann::ordered_json& jRenderSettings = jsonData["render"];
		jRenderSettings["simulation_rate"] = 60.0;
		jRenderSettings["frame_rate"] = jRenderSettings["capture_rate"];
		jRenderSettings.erase("capture_rate");

		nlohmann::ordered_json& jPreviewSettings = jsonData["preview"];
		jPreviewSettings["simulation_rate"] = 60.0;
		jPreviewSettings["frame_rate"] = 60.0;
		jPreviewSettings.erase("capture_rate");

		jsonData["effect"]["events"] = nlohmann::ordered_json::array();

		nlohmann::ordered_json& jMaterials = jsonData["effect"]["resources"]["materials"];
		for(nlohmann::ordered_json& jMaterial : jMaterials) {
			std::string blendMode = jMaterial["blend_mode"];
			if(blendMode == "normal") {
				jMaterial["blend_mode"] = "alpha";
			}
		}

		version = 10;
		jsonData["version"] = 10;
	}

	// Migration 1.9.x -> 1.10.x
	if(version == 10) {
		const auto jsonApplyTransformationToRotation = [](nlohmann::ordered_json& jRotationProperty, const Transform& transform) {
			for(auto& keyframe : jRotationProperty["curve"]["points"]) {
				Transform keyframeTransform(float3_t(0.0), keyframe[1].get<float3_t>(), float3_t(1.0));

				float3_t newValue = (keyframeTransform * transform).rotation();
				keyframe[1]["x"] = newValue.x;
				keyframe[1]["y"] = newValue.y;
				keyframe[1]["z"] = newValue.z;
			}
		};
		const auto jsonApplyTransformationToPositionFromLeft = [](nlohmann::ordered_json& jPositionProperty, const Transform& transform) {
			for(auto& keyframe : jPositionProperty["curve"]["points"]) {
				Transform keyframeTransform(keyframe[1].get<float3_t>(), float3_t(0.0), float3_t(1.0));

				float3_t newValue = (transform * keyframeTransform).position();
				keyframe[1]["x"] = newValue.x;
				keyframe[1]["y"] = newValue.y;
				keyframe[1]["z"] = newValue.z;
			}
		};
		const auto jsonApplyTransformationToRotationFromLeft = [](nlohmann::ordered_json& jRotationProperty, const Transform& transform) {
			for(auto& keyframe : jRotationProperty["curve"]["points"]) {
				Transform keyframeTransform(float3_t(0.0), keyframe[1].get<float3_t>(), float3_t(1.0));

				float3_t newValue = (transform * keyframeTransform).rotation();
				keyframe[1]["x"] = newValue.x;
				keyframe[1]["y"] = newValue.y;
				keyframe[1]["z"] = newValue.z;
			}
		};
		const auto jsonApplyTransformationToScaleFromLeft = [](nlohmann::ordered_json& jScaleProperty, const Transform& transform) {
			for(auto& keyframe : jScaleProperty["curve"]["points"]) {
				Transform keyframeTransform(float3_t(0.0), float3_t(0.0), keyframe[1].get<float3_t>());

				float3_t newValue = (transform * keyframeTransform).scale();
				keyframe[1]["x"] = newValue.x;
				keyframe[1]["y"] = newValue.y;
				keyframe[1]["z"] = newValue.z;
			}
		};

		std::unordered_set<id_t> lineEmittersIds;
		Transform lineEmitterCorrectionTransform(
			float3_t(0.0),
			float3_t(-90.0, 0.0, 0.0),
			float3_t(1.0));
		Transform inverseLineEmitterCorrectionTransform(
			math::inverse(lineEmitterCorrectionTransform.matrix()));

		for(nlohmann::ordered_json& jNode : jsonData["effect"]["scene"]) {
			std::string nodeType = jNode.value("node_type", "");
			std::string shape = jNode.value("shape", "");

			if(nodeType == "particle_emitter" && shape == "line") {
				lineEmittersIds.insert(jNode["id"].get<id_t>());

				jsonApplyTransformationToRotation(jNode["rotation"], lineEmitterCorrectionTransform);
				jsonApplyTransformationToRotationFromLeft(jNode["direction"], inverseLineEmitterCorrectionTransform);
			}
		}

		for(nlohmann::ordered_json& jNode : jsonData["effect"]["scene"]) {
			id_t parentId = jNode.value("parent_id", id_t());

			if(lineEmittersIds.count(parentId) != 0) {
				jsonApplyTransformationToPositionFromLeft(jNode["position"], inverseLineEmitterCorrectionTransform);
				jsonApplyTransformationToRotationFromLeft(jNode["rotation"], inverseLineEmitterCorrectionTransform);
				jsonApplyTransformationToScaleFromLeft(jNode["scale"], inverseLineEmitterCorrectionTransform);
			}
		}

		version = 11;
		jsonData["version"] = 11;
	}
}
}

const std::uint32_t EffectAsset::version = 11;

Effect& EffectAsset::effect() {
	return assetEffect;
}
const Effect& EffectAsset::effect() const {
	return assetEffect;
}

ImageEffectSettings& EffectAsset::imageEffectSettings() {
	return assetImageEffectSettings;
}
const ImageEffectSettings& EffectAsset::imageEffectSettings() const {
	return assetImageEffectSettings;
}

CameraSettings& EffectAsset::cameraSettings() {
	return assetCameraSettings;
}
const CameraSettings& EffectAsset::cameraSettings() const {
	return assetCameraSettings;
}

RenderSettings& EffectAsset::renderSettings() {
	return assetRenderSettings;
}
const RenderSettings& EffectAsset::renderSettings() const {
	return assetRenderSettings;
}

RenderSettings& EffectAsset::previewSettings() {
	return assetPreviewSettings;
}
const RenderSettings& EffectAsset::previewSettings() const {
	return assetPreviewSettings;
}

bool EffectAsset::usesName(const std::string& name) const {
	for(const std::unique_ptr<Node>& node : assetEffect.sceneGraph()) {
		if(node->name() == name) {
			return true;
		}
	}

	for(const ParticleType& particleType : assetEffect.particleTypes()) {
		if(particleType.name() == name) {
			return true;
		}
	}

	return false;
}
bool EffectAsset::usesResource(const std::string& resourceId) const {
	for(const std::unique_ptr<Node>& node : assetEffect.sceneGraph()) {
		if(node->usesResource(resourceId)) {
			return true;
		}
	}

	for(const ParticleType& particleType : assetEffect.particleTypes()) {
		if(particleType.usesResource(resourceId)) {
			return true;
		}
	}

	for(const auto& materialResourceEntry : assetEffect.resources().materials()) {
		const MaterialResource& material = materialResourceEntry.second;

		for(const auto& nodeEntry : material.shaderGraph().nodes()) {
			for(const auto& nodeParameter : nodeEntry.second.parameters()) {
				if(nodeParameter.type() == VariantParameter::Value::type_resource_image &&
					resourceId == nodeParameter.valueResourceId()) {
					return true;
				}
			}
		}
	}

	return false;
}

std::string serializeEffectAsset(const EffectAsset& asset, std::int32_t indent) {
	try {
		nlohmann::ordered_json jsonData = asset;

		return jsonData.dump(indent);
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw SerializationException(e.what());
	}
}

EffectAsset deserializeEffectAsset(std::istream& stream) {
	try {
		nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(stream);
		migrateEffectAssetJson(jsonData);

		return jsonData.get<EffectAsset>();
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw DeserializationException(e.what());
	}
}
EffectAsset deserializeEffectAsset(const std::string& data) {
	try {
		nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data);
		migrateEffectAssetJson(jsonData);

		return jsonData.get<EffectAsset>();
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw DeserializationException(e.what());
	}
}
EffectAsset deserializeEffectAsset(const char* data, std::size_t size) {
	try {
		nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data, data + size);
		migrateEffectAssetJson(jsonData);

		return jsonData.get<EffectAsset>();
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw DeserializationException(e.what());
	}
}

void to_json(nlohmann::ordered_json& j, const EffectAsset& asset) {
	j = nlohmann::ordered_json{
		{ "version", EffectAsset::version },
		{ "effect", asset.effect() },
		{ "image_effects", asset.imageEffectSettings() },
		{ "camera", asset.cameraSettings() },
		{ "render", asset.renderSettings() },
		{ "preview", asset.previewSettings() },
	};
}
void from_json(const nlohmann::ordered_json& j, EffectAsset& asset) {
	std::uint32_t version = j.at("version");
	if(version != EffectAsset::version) {
		throw DeserializationException("Unsupported effect asset version " + std::to_string(version));
	}

	asset.effect() = j.value("effect", Effect());
	asset.imageEffectSettings() = j.value("image_effects", ImageEffectSettings());
	asset.cameraSettings() = j.value("camera", CameraSettings());
	asset.renderSettings() = j.value("render", RenderSettings());
	asset.previewSettings() = j.value("preview", RenderSettings());
}
}
