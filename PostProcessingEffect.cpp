#include "PostProcessingEffect.h"

namespace pixelpart {
PostProcessingEffect::Parameter::Parameter() : type(0xFFFFFFFF) {

}
PostProcessingEffect::Parameter::Parameter(const std::string& n, int32_t v) : name(n), type(0) {
	value.integer = v;
}
PostProcessingEffect::Parameter::Parameter(const std::string& n, floatd v) : name(n), type(1) {
	value.number = v;
}
PostProcessingEffect::Parameter::Parameter(const std::string& n, const vec4d& v) : name(n), type(2) {
	value.vector = v;
}

void PostProcessingEffect::addParameter(const std::string& name, int32_t value) {
	parameters.emplace_back(name, value);
}
void PostProcessingEffect::addParameter(const std::string& name, floatd value) {
	parameters.emplace_back(name, value);
}
void PostProcessingEffect::addParameter(const std::string& name, vec4d value) {
	parameters.emplace_back(name, value);
}

void PostProcessingEffect::removeParameter(const std::string& name, int32_t count) {
	int32_t removed = 0;

	for(auto it = parameters.begin(); it < parameters.end(); ) {
		if(it->name == name) {
			if(removed < count || count < 0) {
				it = parameters.erase(it);
				removed++;
			}
			else {
				break;
			}
		}
		else {
			it++;
		}
	}
}
void PostProcessingEffect::removeParameter(std::size_t index) {
	if(index < parameters.size()) {
		parameters.erase(parameters.begin() + index);
	}
}

bool PostProcessingEffect::hasParameter(const std::string& name, std::size_t index) const {
	return index < parameters.size() && parameters[index].name == name;
}
bool PostProcessingEffect::hasParameter(const std::string& name) const {
	for(const Parameter& parameter : parameters) {
		if(parameter.name == name) {
			return true;
		}
	}

	return false;
}

int32_t& PostProcessingEffect::getParameterInteger(const std::string& name, std::size_t index) {
	if(parameters.size() <= index) {
		parameters.resize(index + 1);
	}

	if(parameters[index].name != name) {
		parameters[index].name = name;
	}
	if(parameters[index].type != 0) {
		parameters[index].type = 0;
		parameters[index].value.integer = 0;
	}

	return parameters[index].value.integer;
}
floatd& PostProcessingEffect::getParameterNumber(const std::string& name, std::size_t index) {
	if(parameters.size() <= index) {
		parameters.resize(index + 1);
	}

	if(parameters[index].name != name) {
		parameters[index].name = name;
	}
	if(parameters[index].type != 1) {
		parameters[index].type = 1;
		parameters[index].value.number = 0.0;
	}

	return parameters[index].value.number;
}
vec4d& PostProcessingEffect::getParameterVector(const std::string& name, std::size_t index) {
	if(parameters.size() <= index) {
		parameters.resize(index + 1);
	}

	if(parameters[index].name != name) {
		parameters[index].name = name;
	}
	if(parameters[index].type != 2) {
		parameters[index].type = 2;
		parameters[index].value.vector = vec4d(0.0);
	}

	return parameters[index].value.vector;
}
int32_t& PostProcessingEffect::getParameterInteger(const std::string& name) {
	for(Parameter& parameter : parameters) {
		if(parameter.name == name) {
			if(parameter.type == 0) {
				return parameter.value.integer;
			}
			else {
				throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect has unexpected type");
			}
		}
	}

	throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect not found");
}
floatd& PostProcessingEffect::getParameterNumber(const std::string& name) {
	for(Parameter& parameter : parameters) {
		if(parameter.name == name) {
			if(parameter.type == 1) {
				return parameter.value.number;
			}
			else {
				throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect has unexpected type");
			}
		}
	}

	throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect not found");
}
vec4d& PostProcessingEffect::getParameterVector(const std::string& name) {
	for(Parameter& parameter : parameters) {
		if(parameter.name == name) {
			if(parameter.type == 2) {
				return parameter.value.vector;
			}
			else {
				throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect has unexpected type");
			}
		}
	}

	throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect not found");
}
const int32_t& PostProcessingEffect::getParameterInteger(const std::string& name, std::size_t index) const {
	if(parameters.size() <= index || parameters[index].name != name) {
		throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect not found");
	}
	if(parameters[index].type != 0) {
		throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect has unexpected type");
	}

	return parameters[index].value.integer;
}
const floatd& PostProcessingEffect::getParameterNumber(const std::string& name, std::size_t index) const {
	if(parameters.size() <= index || parameters[index].name != name) {
		throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect not found");
	}
	if(parameters[index].type != 1) {
		throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect has unexpected type");
	}

	return parameters[index].value.number;
}
const vec4d& PostProcessingEffect::getParameterVector(const std::string& name, std::size_t index) const {
	if(parameters.size() <= index || parameters[index].name != name) {
		throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect not found");
	}
	if(parameters[index].type != 2) {
		throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect has unexpected type");
	}

	return parameters[index].value.vector;
}
const int32_t& PostProcessingEffect::getParameterInteger(const std::string& name) const {
	for(const Parameter& parameter : parameters) {
		if(parameter.name == name) {
			if(parameter.type == 0) {
				return parameter.value.integer;
			}
			else {
				throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect has unexpected type");
			}
		}
	}

	throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect not found");
}
const floatd& PostProcessingEffect::getParameterNumber(const std::string& name) const {
	for(const Parameter& parameter : parameters) {
		if(parameter.name == name) {
			if(parameter.type == 1) {
				return parameter.value.number;
			}
			else {
				throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect has unexpected type");
			}
		}
	}

	throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect not found");
}
const vec4d& PostProcessingEffect::getParameterVector(const std::string& name) const {
	for(const Parameter& parameter : parameters) {
		if(parameter.name == name) {
			if(parameter.type == 2) {
				return parameter.value.vector;
			}
			else {
				throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect has unexpected type");
			}
		}
	}

	throw std::runtime_error("Parameter \"" + name + "\" of post-processing effect not found");
}

void to_json(nlohmann::ordered_json& j, const PostProcessingEffect& effect) {
	nlohmann::ordered_json jParamArray = nlohmann::ordered_json::array();
	
	for(const PostProcessingEffect::Parameter& parameter : effect.parameters) {
		nlohmann::ordered_json jValue;
		switch(parameter.type) {
			case 0:
				jValue = parameter.value.integer;
				break;
			case 1:
				jValue = parameter.value.number;
				break;
			case 2:
				jValue = parameter.value.vector;
				break;
			default:
				break;
		}
			
		jParamArray.push_back(nlohmann::ordered_json{
			{ "name", parameter.name },
			{ "type", parameter.type },
			{ "value", jValue }
		});
	}

	j = nlohmann::ordered_json{
		{ "id", effect.id },
		{ "parameters", jParamArray }
	};
}
void from_json(const nlohmann::ordered_json& j, PostProcessingEffect& effect) {
	effect = PostProcessingEffect();
	effect.id = j.at("id");

	nlohmann::ordered_json jParameterArray = j.at("parameters");

	for(const nlohmann::ordered_json& jParameter : jParameterArray) {
		const nlohmann::ordered_json& jValue = jParameter.at("value");

		std::string name = jParameter.contains("name") ? jParameter.at("name").get<std::string>() : "";
		uint32_t type = jParameter.at("type").get<uint32_t>();

		switch(type) {
			case 0:
				effect.parameters.emplace_back(name, jValue.get<int32_t>());
				break;
			case 1:
				effect.parameters.emplace_back(name, jValue.get<floatd>());
				break;
			case 2:
				effect.parameters.emplace_back(name, jValue.get<vec4d>());
				break;
			default:
				break;
		}
	}
}
}