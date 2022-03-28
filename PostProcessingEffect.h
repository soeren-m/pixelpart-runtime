#pragma once

#include "Types.h"
#include <vector>
#include <type_traits>

namespace pixelpart {
struct PostProcessingEffect {
	struct Parameter {
		std::string name;
		uint32_t type;

		union Value {
			int32_t integer;
			floatd number;
			vec4d vector;
		} value;

		Parameter();
		Parameter(const std::string& n, int32_t v);
		Parameter(const std::string& n, floatd v);
		Parameter(const std::string& n, const vec4d& v);
	};

	std::string id;
	std::vector<Parameter> parameters;

	void addParameter(const std::string& name, int32_t value);
	void addParameter(const std::string& name, floatd value);
	void addParameter(const std::string& name, vec4d value);

	void removeParameter(const std::string& name, int32_t count = -1);
	void removeParameter(std::size_t index);

	bool hasParameter(const std::string& name, std::size_t index) const;
	bool hasParameter(const std::string& name) const;

	int32_t& getParameterInteger(const std::string& name, std::size_t index);
	floatd& getParameterNumber(const std::string& name, std::size_t index);
	vec4d& getParameterVector(const std::string& name, std::size_t index);
	int32_t& getParameterInteger(const std::string& name);
	floatd& getParameterNumber(const std::string& name);
	vec4d& getParameterVector(const std::string& name);
	const int32_t& getParameterInteger(const std::string& name, std::size_t index) const;
	const floatd& getParameterNumber(const std::string& name, std::size_t index) const;
	const vec4d& getParameterVector(const std::string& name, std::size_t index) const;
	const int32_t& getParameterInteger(const std::string& name) const;
	const floatd& getParameterNumber(const std::string& name) const;
	const vec4d& getParameterVector(const std::string& name) const;
};

using PostProcessingPipeline = std::vector<PostProcessingEffect>;

void to_json(nlohmann::ordered_json& j, const PostProcessingEffect& effect);
void from_json(const nlohmann::ordered_json& j, PostProcessingEffect& effect);
}