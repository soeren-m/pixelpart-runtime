#include "Types.h"

namespace pixelpart {
const id_t nullId = 0xFFFFFFFF;
const vec2_t worldUpVector2 = vec2_t(0.0, 1.0);
const vec3_t worldUpVector3 = vec3_t(0.0, 1.0, 0.0);
const vec4_t worldUpVector4 = vec4_t(0.0, 1.0, 0.0, 0.0);

std::ostream& operator<<(std::ostream& os, const vec2_t& v) {
	os << v.x << " "
		<< v.y;

	return os;
}
std::ostream& operator<<(std::ostream& os, const vec3_t& v) {
	os << v.x << " "
		<< v.y << " "
		<< v.z;

	return os;
}
std::ostream& operator<<(std::ostream& os, const vec4_t& v) {
	os << v.x << " "
		<< v.y << " "
		<< v.z << " "
		<< v.w;

	return os;
}
std::istream& operator>>(std::istream& is, vec2_t& v) {
	is >> v.x;
	is >> v.y;

	return is;
}
std::istream& operator>>(std::istream& is, vec3_t& v) {
	is >> v.x;
	is >> v.y;
	is >> v.z;

	return is;
}
std::istream& operator>>(std::istream& is, vec4_t& v) {
	is >> v.x;
	is >> v.y;
	is >> v.z;
	is >> v.w;

	return is;
}

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