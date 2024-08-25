#pragma once

#include "Node.h"

namespace pixelpart {
Node::Node(id_t ownId, id_t parentId) : nodeId(ownId), nodeParentId(parentId) {

}

id_t Node::id() const {
	return nodeId;
}

void Node::parent(const Node& parentNode) {
	nodeParentId = parentNode.id();
}
void Node::unparent() {
	nodeParentId = nullId;
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

AnimatedProperty<vec3_t>& Node::position() {
	return nodePosition;
}
const AnimatedProperty<vec3_t>& Node::position() const {
	return nodePosition;
}
}