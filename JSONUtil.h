#pragma once

#include "Curve.h"
#include <bitset>

namespace pixelpart {
template <typename T>
nlohmann::ordered_json toJson(const T& value) {
	return nlohmann::ordered_json(value);
}

template <>
nlohmann::ordered_json toJson<uint32_t>(const uint32_t& value);

template <typename T>
bool fromJson(T& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey) {
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

namespace std {
template <std::size_t N>
void to_json(nlohmann::ordered_json& j, const bitset<N>& bs) {
	j = nlohmann::ordered_json(bs.to_string('0', '1'));
}
template <std::size_t N>
void from_json(const nlohmann::ordered_json& j, bitset<N>& bs) {
	bs = bitset<N>(j.get<std::string>(), 0, N, '0', '1');
}
}