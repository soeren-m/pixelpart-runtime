#include "Math.h"

namespace pixelpart {
const vec2_t worldUpVector2 = vec2_t(0.0, 1.0);
const vec3_t worldUpVector3 = vec3_t(0.0, 1.0, 0.0);
const vec4_t worldUpVector4 = vec4_t(0.0, 1.0, 0.0, 0.0);

void to_json(nlohmann::ordered_json& j, const vec2_t& v) {
	j = nlohmann::ordered_json{
		{ "x", v.x },
		{ "y", v.y },
	};
}
void to_json(nlohmann::ordered_json& j, const vec3_t& v) {
	j = nlohmann::ordered_json{
		{ "x", v.x },
		{ "y", v.y },
		{ "z", v.z },
	};
}
void to_json(nlohmann::ordered_json& j, const vec4_t& v) {
	j = nlohmann::ordered_json{
		{ "x", v.x },
		{ "y", v.y },
		{ "z", v.z },
		{ "w", v.w },
	};
}
void from_json(const nlohmann::ordered_json& j, vec2_t& v) {
	if(j.is_number()) {
		v.x = j.get<float_t>();
		v.y = 0.0;
	}
	else {
		v.x = j.contains("x") ? j.at("x").get<float_t>() : 0.0;
		v.y = j.contains("y") ? j.at("y").get<float_t>() : 0.0;
	}
}
void from_json(const nlohmann::ordered_json& j, vec3_t& v) {
	if(j.is_number()) {
		v.x = j.get<float_t>();
		v.y = 0.0;
		v.z = 0.0;
	}
	else {
		v.x = j.contains("x") ? j.at("x").get<float_t>() : 0.0;
		v.y = j.contains("y") ? j.at("y").get<float_t>() : 0.0;
		v.z = j.contains("z") ? j.at("z").get<float_t>() : 0.0;
	}
}
void from_json(const nlohmann::ordered_json& j, vec4_t& v) {
	if(j.is_number()) {
		v.x = j.get<float_t>();
		v.y = 0.0;
		v.z = 0.0;
		v.w = 0.0;
	}
	else {
		v.x = j.contains("x") ? j.at("x").get<float_t>() : 0.0;
		v.y = j.contains("y") ? j.at("y").get<float_t>() : 0.0;
		v.z = j.contains("z") ? j.at("z").get<float_t>() : 0.0;
		v.w = j.contains("w") ? j.at("w").get<float_t>() : 0.0;
	}
}
}