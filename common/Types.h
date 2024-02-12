#pragma once

#include <cstdint>
#include <string>
#include <ostream>
#include <istream>
#include "../glm/glm/glm.hpp"
#include "../glm/glm/gtc/type_ptr.hpp"
#include "../glm/glm/gtc/matrix_transform.hpp"
#include "../glm/glm/gtx/norm.hpp"
#include "../glm/glm/gtx/rotate_vector.hpp"
#include "../glm/glm/gtx/vector_angle.hpp"
#include "../glm/glm/gtx/euler_angles.hpp"
#include "../json/single_include/nlohmann/json.hpp"

namespace pixelpart {
using id_t = uint32_t;
using int_t = int64_t;
using float_t = double;
using vec2_t = glm::vec<2, float_t, glm::highp>;
using vec3_t = glm::vec<3, float_t, glm::highp>;
using vec4_t = glm::vec<4, float_t, glm::highp>;
using mat2_t = glm::mat<2, 2, float_t, glm::highp>;
using mat3_t = glm::mat<3, 3, float_t, glm::highp>;
using mat4_t = glm::mat<4, 4, float_t, glm::highp>;

extern const id_t nullId;
extern const vec2_t worldUpVector2;
extern const vec3_t worldUpVector3;
extern const vec4_t worldUpVector4;

std::ostream& operator<<(std::ostream& os, const vec2_t& v);
std::ostream& operator<<(std::ostream& os, const vec3_t& v);
std::ostream& operator<<(std::ostream& os, const vec4_t& v);
std::istream& operator>>(std::istream& is, vec2_t& v);
std::istream& operator>>(std::istream& is, vec3_t& v);
std::istream& operator>>(std::istream& is, vec4_t& v);

void to_json(nlohmann::ordered_json& j, const vec2_t& v);
void to_json(nlohmann::ordered_json& j, const vec3_t& v);
void to_json(nlohmann::ordered_json& j, const vec4_t& v);
void from_json(const nlohmann::ordered_json& j, vec2_t& v);
void from_json(const nlohmann::ordered_json& j, vec3_t& v);
void from_json(const nlohmann::ordered_json& j, vec4_t& v);
}

namespace nlohmann {
template <>
struct adl_serializer<pixelpart::vec2_t> {
	static void to_json(ordered_json& j, const pixelpart::vec2_t& v) {
		pixelpart::to_json(j, v);
	}
	static void from_json(const ordered_json& j, pixelpart::vec2_t& v) {
		pixelpart::from_json(j, v);
	}
};

template <>
struct adl_serializer<pixelpart::vec3_t> {
	static void to_json(ordered_json& j, const pixelpart::vec3_t& v) {
		pixelpart::to_json(j, v);
	}
	static void from_json(const ordered_json& j, pixelpart::vec3_t& v) {
		pixelpart::from_json(j, v);
	}
};

template <>
struct adl_serializer<pixelpart::vec4_t> {
	static void to_json(ordered_json& j, const pixelpart::vec4_t& v) {
		pixelpart::to_json(j, v);
	}
	static void from_json(const ordered_json& j, pixelpart::vec4_t& v) {
		pixelpart::from_json(j, v);
	}
};
}