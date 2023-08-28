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
using floatd = double;
using vec2d = glm::vec<2, floatd, glm::highp>;
using vec3d = glm::vec<3, floatd, glm::highp>;
using vec4d = glm::vec<4, floatd, glm::highp>;
using mat2d = glm::mat<2, 2, floatd, glm::highp>;
using mat3d = glm::mat<3, 3, floatd, glm::highp>;
using mat4d = glm::mat<4, 4, floatd, glm::highp>;

extern const uint32_t nullId;
extern const vec2d worldUpVector2;
extern const vec3d worldUpVector3;
extern const vec4d worldUpVector4;

std::ostream& operator<<(std::ostream& os, const vec2d& v);
std::ostream& operator<<(std::ostream& os, const vec3d& v);
std::ostream& operator<<(std::ostream& os, const vec4d& v);
std::istream& operator>>(std::istream& is, vec2d& v);
std::istream& operator>>(std::istream& is, vec3d& v);
std::istream& operator>>(std::istream& is, vec4d& v);

void to_json(nlohmann::ordered_json& j, const vec2d& v);
void to_json(nlohmann::ordered_json& j, const vec3d& v);
void to_json(nlohmann::ordered_json& j, const vec4d& v);
void from_json(const nlohmann::ordered_json& j, vec2d& v);
void from_json(const nlohmann::ordered_json& j, vec3d& v);
void from_json(const nlohmann::ordered_json& j, vec4d& v);
}

namespace nlohmann {
template <>
struct adl_serializer<pixelpart::vec2d> {
	static void to_json(ordered_json& j, const pixelpart::vec2d& v) {
		pixelpart::to_json(j, v);
	}
	static void from_json(const ordered_json& j, pixelpart::vec2d& v) {
		pixelpart::from_json(j, v);
	}
};

template <>
struct adl_serializer<pixelpart::vec3d> {
	static void to_json(ordered_json& j, const pixelpart::vec3d& v) {
		pixelpart::to_json(j, v);
	}
	static void from_json(const ordered_json& j, pixelpart::vec3d& v) {
		pixelpart::from_json(j, v);
	}
};

template <>
struct adl_serializer<pixelpart::vec4d> {
	static void to_json(ordered_json& j, const pixelpart::vec4d& v) {
		pixelpart::to_json(j, v);
	}
	static void from_json(const ordered_json& j, pixelpart::vec4d& v) {
		pixelpart::from_json(j, v);
	}
};
}