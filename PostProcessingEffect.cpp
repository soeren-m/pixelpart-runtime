#include "PostProcessingEffect.h"

namespace pixelpart {
void PostProcessingEffect::addParameter(const std::string& name, int32_t value) {
	parameters.emplace_back(Parameter{ 
		name,
		value
	});
}
void PostProcessingEffect::addParameter(const std::string& name, floatd value) {
	parameters.emplace_back(Parameter{ 
		name,
		value
	});
}
void PostProcessingEffect::addParameter(const std::string& name, vec4d value) {
	parameters.emplace_back(Parameter{ 
		name,
		value
	});
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

void to_json(nlohmann::ordered_json& j, const PostProcessingEffect& effect) {
	nlohmann::ordered_json jParamArray = nlohmann::ordered_json::array();
	
	for(const PostProcessingEffect::Parameter& parameter : effect.parameters) {
		nlohmann::ordered_json jValue;
		switch(parameter.value.index()) {
			case 0:
				jValue = std::get<int32_t>(parameter.value);
				break;
			case 1:
				jValue = std::get<floatd>(parameter.value);
				break;
			case 2:
				jValue = std::get<vec4d>(parameter.value);
				break;
			default:
				break;
		}
			
		jParamArray.push_back(nlohmann::ordered_json{
			{ "name", parameter.name },
			{ "type", parameter.value.index() },
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

		PostProcessingEffect::Parameter parameter;
		if(jParameter.contains("name")) {
			parameter.name = jParameter.at("name");
		}
				
		switch(jParameter.at("type").get<std::size_t>()) {
			case 0:
				parameter.value = jValue.get<int32_t>();
				break;
			case 1:
				parameter.value = jValue.get<floatd>();
				break;
			case 2:
				parameter.value = jValue.get<vec4d>();
				break;
			default:
				break;
		}

		effect.parameters.push_back(parameter);
	}
}
}