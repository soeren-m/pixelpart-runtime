#pragma once

#include "../common/Types.h"
#include "../common/Math.h"
#include "../common/Id.h"
#include "AnimatedProperty.h"
#include <string>

namespace pixelpart {
class Node {
public:
	friend class SceneGraph;

	id_t id() const;

	/*void parent(const Node& parentNode);
	void parent(id_t parentNodeId);
	void unparent();*/
	id_t parentId() const;

	void name(const std::string& name);
	const std::string& name() const;

	void start(float_t time);
	float_t start() const;

	void duration(float_t time);
	float_t duration() const;

	void repeat(bool repeat);
	bool repeat() const;

	float_t life(float_t time, bool forceRepeat = false) const;
	bool active(float_t time) const;

	AnimatedProperty<float3_t>& position();
	const AnimatedProperty<float3_t>& position() const;

	AnimatedProperty<float3_t>& orientation();
	const AnimatedProperty<float3_t>& orientation() const;

	AnimatedProperty<float3_t>& size();
	const AnimatedProperty<float3_t>& size() const;

protected:
	Node(id_t ownId, id_t parentId = id_t());

private:
	id_t nodeId = id_t();
	id_t nodeParentId = id_t();
	std::string nodeName;

	float_t nodeLifetimeStart = 0.0;
	float_t nodeLifetimeDuration = 1.0;
	bool nodeRepeat = true;

	AnimatedProperty<float3_t> nodePosition = AnimatedProperty<float3_t>(0.0, float3_t(0.0));
	AnimatedProperty<float3_t> nodeOrientation = AnimatedProperty<float3_t>(float3_t(0.0));
	AnimatedProperty<float3_t> nodeSize = AnimatedProperty<float3_t>(float3_t(1.0));

	bool nodeInheritPosition = true;
	bool nodeInheritOrientation = true;
	bool nodeInheritSize = true;
};
}