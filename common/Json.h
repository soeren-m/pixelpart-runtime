#pragma once

#include "Curve.h"

namespace pixelpart {
template <typename T>
nlohmann::ordered_json toJson(const T& value) {
	return nlohmann::ordered_json(value);
}

template <>
nlohmann::ordered_json toJson<id_t>(const id_t& value);

template <typename T>
bool fromJson(T& value, const nlohmann::ordered_json& j, const std::string& key) {
	if(j.contains(key)) {
		value = j.at(key).get<T>();

		return true;
	}

	return false;
}

template <typename T>
bool fromJson(T& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey) {
	if(fromJson<T>(value, j, key)) {
		return true;
	}

	return fromJson<T>(value, j, alternativeKey);
}

template <>
bool fromJson<id_t>(id_t& value, const nlohmann::ordered_json& j, const std::string& key);
template <>
bool fromJson<Curve<float_t>>(Curve<float_t>& value, const nlohmann::ordered_json& j, const std::string& key);
template <>
bool fromJson<Curve<vec2_t>>(Curve<vec2_t>& value, const nlohmann::ordered_json& j, const std::string& key);
template <>
bool fromJson<Curve<vec3_t>>(Curve<vec3_t>& value, const nlohmann::ordered_json& j, const std::string& key);
template <>
bool fromJson<Curve<vec4_t>>(Curve<vec4_t>& value, const nlohmann::ordered_json& j, const std::string& key);
}