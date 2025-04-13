#include "Node.h"
#include <cmath>

namespace pixelpart {
Node::Node(id_t ownId, id_t parentId) : nodeId(ownId), nodeParentId(parentId) {

}

std::unique_ptr<Node> Node::clone() const {
	return std::unique_ptr<Node>(cloneImpl());
}

void Node::applyInputs(const ComputeGraph::InputSet& inputs) {
	nodePosition.input(inputs);
	nodeOrientation.input(inputs);
	nodeSize.input(inputs);
}
bool Node::usesResource(const std::string& resourceId) const {
	return false;
}

id_t Node::id() const {
	return nodeId;
}

void Node::parent(const Node& parentNode) {
	nodeParentId = parentNode.id();
}
void Node::parent(id_t parentNodeId) {
	nodeParentId = parentNodeId;
}
id_t Node::parentId() const {
	return nodeParentId;
}

void Node::name(const std::string& name) {
	nodeName = name;
}
const std::string& Node::name() const {
	return nodeName;
}

void Node::start(float_t time) {
	nodeLifetimeStart = time;
}
float_t Node::start() const {
	return nodeLifetimeStart;
}

void Node::duration(float_t time) {
	nodeLifetimeDuration = time;
}
float_t Node::duration() const {
	return nodeLifetimeDuration;
}

void Node::repeat(bool repeat) {
	nodeRepeat = repeat;
}
bool Node::repeat() const {
	return nodeRepeat;
}

void Node::startTrigger(id_t triggerId) {
	nodeStartTriggerId = triggerId;
}
id_t Node::startTrigger() const {
	return nodeStartTriggerId;
}

void Node::stopTrigger(id_t triggerId) {
	nodeStopTriggerId = triggerId;
}
id_t Node::stopTrigger() const {
	return nodeStopTriggerId;
}

float_t Node::timeSinceStart(const RuntimeContext& runtimeContext, bool useTriggers) const {
	if(useTriggers && nodeStartTriggerId) {
		if(!runtimeContext.triggerActivated(nodeStartTriggerId)) {
			return 0.0;
		}

		return runtimeContext.currentTime() - (runtimeContext.triggerActivationTime(nodeStartTriggerId) + nodeLifetimeStart);
	}

	return runtimeContext.currentTime() - nodeLifetimeStart;
}
float_t Node::life(const RuntimeContext& runtimeContext, bool useTriggers) const {
	float_t startTime = nodeLifetimeStart;

	if(useTriggers) {
		if(nodeStartTriggerId) {
			if(!runtimeContext.triggerActivated(nodeStartTriggerId)) {
				return 0.0;
			}

			startTime = runtimeContext.triggerActivationTime(nodeStartTriggerId) + nodeLifetimeStart;
		}

		if(nodeStopTriggerId) {
			if(runtimeContext.triggerActivated(nodeStopTriggerId)) {
				return 1.0;
			}
		}
	}

	return nodeRepeat
		? std::fmod(runtimeContext.currentTime() - startTime, nodeLifetimeDuration) / nodeLifetimeDuration
		: (runtimeContext.currentTime() - startTime) / nodeLifetimeDuration;
}
bool Node::active(const RuntimeContext& runtimeContext, bool useTriggers) const {
	float_t startTime = nodeLifetimeStart;

	if(useTriggers) {
		if(nodeStartTriggerId) {
			if(!runtimeContext.triggerActivated(nodeStartTriggerId)) {
				return false;
			}

			startTime = runtimeContext.triggerActivationTime(nodeStartTriggerId) + nodeLifetimeStart;
		}

		if(nodeStopTriggerId) {
			if(runtimeContext.triggerActivated(nodeStopTriggerId)) {
				return false;
			}
		}
	}

	return
		(runtimeContext.currentTime() >= startTime) &&
		(runtimeContext.currentTime() <= startTime + nodeLifetimeDuration || nodeRepeat);
}
bool Node::activatedByTrigger(const RuntimeContext& runtimeContext) const {
	return
		(!nodeStartTriggerId || runtimeContext.triggerActivated(nodeStartTriggerId)) &&
		(!nodeStopTriggerId || !runtimeContext.triggerActivated(nodeStopTriggerId));
}

AnimatedProperty<float3_t>& Node::position() {
	return nodePosition;
}
const AnimatedProperty<float3_t>& Node::position() const {
	return nodePosition;
}

AnimatedProperty<float3_t>& Node::orientation() {
	return nodeOrientation;
}
const AnimatedProperty<float3_t>& Node::orientation() const {
	return nodeOrientation;
}

AnimatedProperty<float3_t>& Node::size() {
	return nodeSize;
}
const AnimatedProperty<float3_t>& Node::size() const {
	return nodeSize;
}

NodeTransform Node::transform(const RuntimeContext& runtimeContext, bool useTriggers) const {
	float_t fraction = life(runtimeContext, useTriggers);

	return NodeTransform(
		nodePosition.at(fraction),
		nodeOrientation.at(fraction),
		nodeSize.at(fraction));
}
NodeTransform Node::baseTransform(const RuntimeContext& runtimeContext) const {
	float_t fraction = life(runtimeContext, false);

	return NodeTransform(
		nodePosition.curve().at(fraction),
		nodeOrientation.curve().at(fraction),
		nodeSize.curve().at(fraction));
}

Node* Node::cloneImpl() const {
	return new Node(*this);
}

void to_json(nlohmann::ordered_json& j, const Node& node) {
	j = nlohmann::ordered_json{
		{ "id", node.id() },
		{ "parent_id", node.parentId() },
		{ "name", node.name() },
		{ "lifetime_start", node.start() },
		{ "lifetime_duration", node.duration() },
		{ "repeat", node.repeat() },
		{ "start_trigger", node.startTrigger() },
		{ "stop_trigger", node.stopTrigger() },
		{ "position", node.position() },
		{ "orientation", node.orientation() },
		{ "size", node.size() }
	};
}
void from_json(const nlohmann::ordered_json& j, Node& node) {
	node.parent(j.value("parent_id", id_t()));
	node.name(j.value("name", ""));
	node.start(j.value("lifetime_start", 0.0));
	node.duration(j.value("lifetime_duration", 1.0));
	node.repeat(j.value("repeat", true));
	node.startTrigger(j.value("start_trigger", id_t()));
	node.stopTrigger(j.value("stop_trigger", id_t()));
	node.position() = j.value("position", AnimatedProperty<float3_t>(0.0, float3_t(0.0)));
	node.orientation() = j.value("orientation", AnimatedProperty<float3_t>(float3_t(0.0)));
	node.size() = j.value("size", AnimatedProperty<float3_t>(float3_t(1.0)));
}
}