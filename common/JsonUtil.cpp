#include "JsonUtil.h"

namespace pixelpart {
template <>
nlohmann::ordered_json toJson<id_t>(const id_t& value) {
	if(value == nullId) {
		return nlohmann::ordered_json(nullptr);
	}

	return nlohmann::ordered_json(value);
}

template <>
bool fromJson(id_t& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey) {
	if(j.contains(key)) {
		const nlohmann::ordered_json& jValue = j.at(key);
		value = jValue.is_number_unsigned()
			? jValue.get<id_t>()
			: nullId;

		return true;
	}
	else if(!alternativeKey.empty()) {
		return fromJson<id_t>(value, j, alternativeKey, "");
	}

	return false;
}

template <>
bool fromJson<Curve<float_t>>(Curve<float_t>& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey) {
	if(j.contains(key)) {
		const nlohmann::ordered_json& jValue = j.at(key);

		if(jValue.is_number()) {
			value.setPoints({ Curve<float_t>::Point{ 0.5, jValue.get<float_t>() } });

			return true;
		}
		else if(jValue.is_object()) {
			value = jValue.get<Curve<float_t>>();

			return true;
		}
	}
	else if(!alternativeKey.empty()) {
		return fromJson<Curve<float_t>>(value, j, alternativeKey, "");
	}

	return false;
}

template <>
bool fromJson<Curve<vec2_t>>(Curve<vec2_t>& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey) {
	if(j.contains(key)) {
		const nlohmann::ordered_json& jValue = j.at(key);

		if(jValue.contains("x") &&
		jValue.contains("y")) {
			value = Curve<vec2_t>(jValue.get<vec2_t>());

			return true;
		}
		else if(jValue.is_object()) {
			value = jValue.get<Curve<vec2_t>>();

			return true;
		}
	}
	else if(!alternativeKey.empty()) {
		return fromJson<Curve<vec2_t>>(value, j, alternativeKey, "");
	}

	return false;
}

template <>
bool fromJson<Curve<vec3_t>>(Curve<vec3_t>& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey) {
	if(j.contains(key)) {
		const nlohmann::ordered_json& jValue = j.at(key);

		if(jValue.contains("x") &&
		jValue.contains("y") &&
		jValue.contains("z")) {
			value = Curve<vec3_t>(jValue.get<vec3_t>());

			return true;
		}
		else if(jValue.is_object()) {
			value = jValue.get<Curve<vec3_t>>();

			return true;
		}
	}
	else if(!alternativeKey.empty()) {
		return fromJson<Curve<vec3_t>>(value, j, alternativeKey, "");
	}

	return false;
}

template <>
bool fromJson<Curve<vec4_t>>(Curve<vec4_t>& value, const nlohmann::ordered_json& j, const std::string& key, const std::string& alternativeKey) {
	if(j.contains(key)) {
		const nlohmann::ordered_json& jValue = j.at(key);

		if(jValue.contains("x") &&
		jValue.contains("y") &&
		jValue.contains("z") &&
		jValue.contains("w")) {
			value = Curve<vec4_t>(jValue.get<vec4_t>());

			return true;
		}
		else if(jValue.is_object()) {
			value = jValue.get<Curve<vec4_t>>();

			return true;
		}
	}
	else if(!alternativeKey.empty()) {
		return fromJson<Curve<vec4_t>>(value, j, alternativeKey, "");
	}

	return false;
}
}