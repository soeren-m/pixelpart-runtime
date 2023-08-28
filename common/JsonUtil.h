#pragma once

#include "Curve.h"

namespace pixelpart {
template <typename T>
nlohmann::ordered_json toJson(const T& value) {
	return nlohmann::ordered_json(value);
}

template <>
nlohmann::ordered_json toJson<uint32_t>(const uint32_t& value);

template <typename T>
bool fromJson(T& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey = "") {
	if(j.contains(key)) {
		value = j.at(key).get<T>();

		return true;
	}
	else if(!alternativeKey.empty()) {
		return fromJson<T>(value, j, alternativeKey, "");
	}

	return false;
}

template <>
bool fromJson<uint32_t>(uint32_t& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey);
template <>
bool fromJson<Curve<floatd>>(Curve<floatd>& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey);
template <>
bool fromJson<Curve<vec2d>>(Curve<vec2d>& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey);
template <>
bool fromJson<Curve<vec3d>>(Curve<vec3d>& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey);
template <>
bool fromJson<Curve<vec4d>>(Curve<vec4d>& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey);
}