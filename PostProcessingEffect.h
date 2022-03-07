#pragma once

#include "Types.h"
#include <variant>
#include <vector>

namespace pixelpart {
struct PostProcessingEffect {
	struct Parameter {
		using Value = std::variant<int32_t, floatd, vec4d>;

		std::string name;
		Value value;
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

	template <typename T>
	T& getParameter(const std::string& name, std::size_t index) {
		if(parameters.size() <= index) {
			parameters.resize(index + 1);
		}
		if(parameters[index].name != name) {
			parameters[index].name = name;
		}

		if(!std::holds_alternative<T>(parameters[index].value)) {
			parameters[index].value = T(0);
		}
		
		return std::get<T>(parameters[index].value);
	}
	template <typename T>
	const T& getParameter(const std::string& name, std::size_t index) const {
		if(parameters.size() <= index || parameters[index].name != name) {
			throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect not found");
		}
		else if(!std::holds_alternative<T>(parameters[index].value)) {
			throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect has unexpected type");
		}
		
		return std::get<T>(parameters[index].value);
	}

	template <typename T>
	T& getParameter(const std::string& name) {
		for(Parameter& parameter : parameters) {
			if(parameter.name == name) {
				return std::get<T>(parameter);
			}
		}

		throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect not found");
	}
	template <typename T>
	const T& getParameter(const std::string& name) const {
		for(const Parameter& parameter : parameters) {
			if(parameter.name == name) {
				return std::get<T>(parameter);
			}
		}

		throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect not found");
	}
};

using PostProcessingPipeline = std::vector<PostProcessingEffect>;

void to_json(nlohmann::ordered_json& j, const PostProcessingEffect& effect);
void from_json(const nlohmann::ordered_json& j, PostProcessingEffect& effect);
}