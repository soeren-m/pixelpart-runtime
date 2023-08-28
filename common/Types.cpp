#include "Types.h"

namespace pixelpart {
const uint32_t nullId = 0xFFFFFFFF;
const vec2d worldUpVector2 = vec2d(0.0, 1.0);
const vec3d worldUpVector3 = vec3d(0.0, 1.0, 0.0);
const vec4d worldUpVector4 = vec4d(0.0, 1.0, 0.0, 0.0);

std::ostream& operator<<(std::ostream& os, const vec2d& v) {
	os << v.x << " "
		<< v.y;

	return os;
}
std::ostream& operator<<(std::ostream& os, const vec3d& v) {
	os << v.x << " "
		<< v.y << " "
		<< v.z;

	return os;
}
std::ostream& operator<<(std::ostream& os, const vec4d& v) {
	os << v.x << " "
		<< v.y << " "
		<< v.z << " "
		<< v.w;

	return os;
}
std::istream& operator>>(std::istream& is, vec2d& v) {
	is >> v.x;
	is >> v.y;

	return is;
}
std::istream& operator>>(std::istream& is, vec3d& v) {
	is >> v.x;
	is >> v.y;
	is >> v.z;

	return is;
}
std::istream& operator>>(std::istream& is, vec4d& v) {
	is >> v.x;
	is >> v.y;
	is >> v.z;
	is >> v.w;

	return is;
}

void to_json(nlohmann::ordered_json& j, const vec2d& v) {
	j = nlohmann::ordered_json{
		{ "x", v.x },
		{ "y", v.y },
	};
}
void to_json(nlohmann::ordered_json& j, const vec3d& v) {
	j = nlohmann::ordered_json{
		{ "x", v.x },
		{ "y", v.y },
		{ "z", v.z },
	};
}
void to_json(nlohmann::ordered_json& j, const vec4d& v) {
	j = nlohmann::ordered_json{
		{ "x", v.x },
		{ "y", v.y },
		{ "z", v.z },
		{ "w", v.w },
	};
}
void from_json(const nlohmann::ordered_json& j, vec2d& v) {
	if(j.is_number()) {
		v.x = j.get<floatd>();
		v.y = 0.0;
	}
	else {
		v.x = j.contains("x") ? j.at("x").get<floatd>() : 0.0;
		v.y = j.contains("y") ? j.at("y").get<floatd>() : 0.0;
	}
}
void from_json(const nlohmann::ordered_json& j, vec3d& v) {
	if(j.is_number()) {
		v.x = j.get<floatd>();
		v.y = 0.0;
		v.z = 0.0;
	}
	else {
		v.x = j.contains("x") ? j.at("x").get<floatd>() : 0.0;
		v.y = j.contains("y") ? j.at("y").get<floatd>() : 0.0;
		v.z = j.contains("z") ? j.at("z").get<floatd>() : 0.0;
	}
}
void from_json(const nlohmann::ordered_json& j, vec4d& v) {
	if(j.is_number()) {
		v.x = j.get<floatd>();
		v.y = 0.0;
		v.z = 0.0;
		v.w = 0.0;
	}
	else {
		v.x = j.contains("x") ? j.at("x").get<floatd>() : 0.0;
		v.y = j.contains("y") ? j.at("y").get<floatd>() : 0.0;
		v.z = j.contains("z") ? j.at("z").get<floatd>() : 0.0;
		v.w = j.contains("w") ? j.at("w").get<floatd>() : 0.0;
	}
}
}