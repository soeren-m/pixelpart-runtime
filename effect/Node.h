#pragma once

#include "../common/Types.h"
#include "../common/Math.h"
#include "../common/Constants.h"
#include "AnimatedProperty.h"
#include <string>

namespace pixelpart {
class Node {
public:
	Node() = default;
	Node(id_t ownId, id_t parentId = nullId);

	id_t id() const;

	void parent(const Node& parentNode);
	void unparent();
	id_t parentId() const;

	void name(const std::string& name);
	const std::string& name() const;

	void start(float_t time);
	float_t start() const;

	void duration(float_t time);
	float_t duration() const;

	void repeat(bool repeat);
	bool repeat() const;

	float_t life(float_t time) const;
	bool active(float_t time) const;

	AnimatedProperty<vec3_t>& position();
	const AnimatedProperty<vec3_t>& position() const;

private:
	id_t nodeId = nullId;
	id_t nodeParentId = nullId;
	std::string nodeName;

	float_t nodeLifetimeStart = 0.0;
	float_t nodeLifetimeDuration = 1.0;
	bool nodeRepeat = true;

	AnimatedProperty<vec3_t> nodePosition = AnimatedProperty<vec3_t>(0.0, vec3_t(0.0));
};
}