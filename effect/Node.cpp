#include "Node.h"
#include <cmath>

namespace pixelpart {
Node::Node(id_t ownId, id_t parentId) : nodeId(ownId), nodeParentId(parentId) {

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
void Node::unparent() {
	nodeParentId = id_t();
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

float_t Node::life(float_t time, bool forceRepeat) const {
	return nodeRepeat || forceRepeat
		? std::fmod(time - nodeLifetimeStart, nodeLifetimeDuration) / nodeLifetimeDuration
		: (time - nodeLifetimeStart) / nodeLifetimeDuration;
}
bool Node::active(float_t time) const {
	return
		(time >= nodeLifetimeStart) &&
		(time <= nodeLifetimeStart + nodeLifetimeDuration || nodeRepeat);
}

AnimatedProperty<float3_t>& Node::position() {
	return nodePosition;
}
const AnimatedProperty<float3_t>& Node::position() const {
	return nodePosition;
}
}