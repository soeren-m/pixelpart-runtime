#include "Types.h"

namespace pixelpart {
const uint32_t NullId = 0xFFFFFFFF;

std::ostream& operator<<(std::ostream& os, const vec2d& v) {
	os << v.x << " " << v.y;

	return os;
}
std::ostream& operator<<(std::ostream& os, const vec3d& v) {
	os << v.x << " " << v.y << " " << v.z;
	
	return os;
}
std::ostream& operator<<(std::ostream& os, const vec4d& v) {
	os << v.x << " " << v.y << " " << v.z << " " << v.w;
	
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

void to_json(nlohmann::ordered_json& j, const vec2d& v){
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
	v.x = j.at("x");
	v.y = j.at("y");
}
void from_json(const nlohmann::ordered_json& j, vec3d& v) {
	v.x = j.at("x");
	v.y = j.at("y");
	v.z = j.at("z");
}
void from_json(const nlohmann::ordered_json& j, vec4d& v) {
	v.x = j.at("x");
	v.y = j.at("y");
	v.z = j.at("z");
	v.w = j.at("w");
}
}