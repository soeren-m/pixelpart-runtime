#include "LightSource.h"
#include "NodeType.h"

namespace pixelpart {
LightSource::LightSource(id_t ownId, id_t parentId) : Node(ownId, parentId) {

}

void LightSource::applyInputs(const ComputeGraph::InputSet& inputs) {
	Node::applyInputs(inputs);

	lightAttenuation.input(inputs);
	lightColor.input(inputs);
	lightIntensity.input(inputs);
}

AnimatedProperty<float_t>& LightSource::attenuation() {
	return lightAttenuation;
}
const AnimatedProperty<float_t>& LightSource::attenuation() const {
	return lightAttenuation;
}

AnimatedProperty<float4_t>& LightSource::color() {
	return lightColor;
}
const AnimatedProperty<float4_t>& LightSource::color() const {
	return lightColor;
}

AnimatedProperty<float_t>& LightSource::intensity() {
	return lightIntensity;
}
const AnimatedProperty<float_t>& LightSource::intensity() const {
	return lightIntensity;
}

void to_json(nlohmann::ordered_json& j, const LightSource& lightSource) {
	to_json(j, static_cast<const Node&>(lightSource));

	j.update(nlohmann::ordered_json{
		{ "node_type", NodeType::light_source },
		{ "attenuation", lightSource.attenuation() },
		{ "color", lightSource.color() },
		{ "intensity", lightSource.intensity() }
	});
}
void from_json(const nlohmann::ordered_json& j, LightSource& lightSource) {
	from_json(j, static_cast<Node&>(lightSource));

	lightSource.attenuation() = j.value("attenuation", AnimatedProperty<float_t>(1.0));
	lightSource.color() = j.value("color", AnimatedProperty<float4_t>(float4_t(1.0)));
	lightSource.intensity() = j.value("intensity", AnimatedProperty<float_t>(1.0));
}
}