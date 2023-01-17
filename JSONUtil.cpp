#include "JSONUtil.h"

namespace pixelpart {
template <>
nlohmann::ordered_json toJson<uint32_t>(const uint32_t& value) {
	if(value == nullId) {
		return nlohmann::ordered_json(nullptr);
	}

	return nlohmann::ordered_json(value);
}

template <>
bool fromJson(uint32_t& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey) {
	if(j.contains(key)) {
		const nlohmann::ordered_json& jValue = j.at(key);
		value = jValue.is_number_unsigned()
			? jValue.get<uint32_t>()
			: nullId;

		return true;
	}
	else if(!alternativeKey.empty()) {
		return fromJson<uint32_t>(value, j, alternativeKey, "");
	}

	return false;
}

template <>
bool fromJson<Curve<floatd>>(Curve<floatd>& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey) {
	if(j.contains(key)) {
		const nlohmann::ordered_json& jValue = j.at(key);

		if(jValue.is_number()) {
			value.setPoints({ Curve<floatd>::Point{ 0.5, jValue.get<floatd>() } });

			return true;
		}
		else if(jValue.is_object()) {
			value = jValue.get<Curve<floatd>>();

			return true;
		}
	}
	else if(!alternativeKey.empty()) {
		return fromJson<Curve<floatd>>(value, j, alternativeKey, "");
	}

	return false;
}

template <>
bool fromJson<Curve<vec2d>>(Curve<vec2d>& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey) {
	if(j.contains(key)) {
		const nlohmann::ordered_json& jValue = j.at(key);

		if(jValue.contains("x") &&
		jValue.contains("y")) {
			value = Curve<vec2d>(jValue.get<vec2d>());

			return true;
		}
		else if(jValue.is_object()) {
			value = jValue.get<Curve<vec2d>>();

			return true;
		}
	}
	else if(!alternativeKey.empty()) {
		return fromJson<Curve<vec2d>>(value, j, alternativeKey, "");
	}

	return false;
}

template <>
bool fromJson<Curve<vec3d>>(Curve<vec3d>& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey) {
	if(j.contains(key)) {
		const nlohmann::ordered_json& jValue = j.at(key);

		if(jValue.contains("x") &&
		jValue.contains("y") &&
		jValue.contains("z")) {
			value = Curve<vec3d>(jValue.get<vec3d>());

			return true;
		}
		else if(jValue.is_object()) {
			value = jValue.get<Curve<vec3d>>();

			return true;
		}
	}
	else if(!alternativeKey.empty()) {
		return fromJson<Curve<vec3d>>(value, j, alternativeKey, "");
	}

	return false;
}

template <>
bool fromJson<Curve<vec4d>>(Curve<vec4d>& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey) {
	if(j.contains(key)) {
		const nlohmann::ordered_json& jValue = j.at(key);

		if(jValue.contains("x") &&
		jValue.contains("y") &&
		jValue.contains("z") &&
		jValue.contains("w")) {
			value = Curve<vec4d>(jValue.get<vec4d>());

			return true;
		}
		else if(jValue.is_object()) {
			value = jValue.get<Curve<vec4d>>();

			return true;
		}
	}
	else if(!alternativeKey.empty()) {
		return fromJson<Curve<vec4d>>(value, j, alternativeKey, "");
	}

	return false;
}
}