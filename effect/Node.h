#pragma once

#include "AnimatedProperty.h"
#include "RuntimeContext.h"
#include "ComputeGraph.h"
#include "../common/Types.h"
#include "../common/Math.h"
#include "../common/Transform.h"
#include "../common/Id.h"
#include <memory>
#include <string>

namespace pixelpart {
class Node {
public:
	friend class SceneGraph;

	Node() = default;
	Node(id_t ownId, id_t parentId = id_t());

	std::unique_ptr<Node> clone() const;

	virtual void applyInputs(const ComputeGraph::InputSet& inputs);
	virtual bool usesResource(const std::string& resourceId) const;

	id_t id() const;

	void parent(const Node& parentNode);
	void parent(id_t parentNodeId);
	id_t parentId() const;

	void name(const std::string& name);
	const std::string& name() const;

	void start(float_t time);
	float_t start() const;

	void duration(float_t time);
	float_t duration() const;

	void repeat(bool repeat);
	bool repeat() const;

	void startTrigger(id_t triggerId);
	id_t startTrigger() const;

	void stopTrigger(id_t triggerId);
	id_t stopTrigger() const;

	float_t timeSinceStart(const RuntimeContext& runtimeContext, bool useTriggers = true) const;
	float_t life(const RuntimeContext& runtimeContext, bool useTriggers = true) const;
	bool active(const RuntimeContext& runtimeContext, bool useTriggers = true) const;
	bool activatedByTrigger(const RuntimeContext& runtimeContext) const;

	AnimatedProperty<float3_t>& position();
	const AnimatedProperty<float3_t>& position() const;

	AnimatedProperty<float3_t>& rotation();
	const AnimatedProperty<float3_t>& rotation() const;

	AnimatedProperty<float3_t>& scale();
	const AnimatedProperty<float3_t>& scale() const;

	Transform transform(const RuntimeContext& runtimeContext, bool useTriggers = true) const;
	Transform baseTransform(const RuntimeContext& runtimeContext) const;

protected:
	virtual Node* cloneImpl() const;

private:
	id_t nodeId;
	id_t nodeParentId;
	std::string nodeName;

	float_t nodeLifetimeStart = 0.0;
	float_t nodeLifetimeDuration = 1.0;
	bool nodeRepeat = true;

	id_t nodeStartTriggerId;
	id_t nodeStopTriggerId;

	AnimatedProperty<float3_t> nodePosition = AnimatedProperty<float3_t>(0.0, float3_t(0.0));
	AnimatedProperty<float3_t> nodeRotation = AnimatedProperty<float3_t>(float3_t(0.0));
	AnimatedProperty<float3_t> nodeScale = AnimatedProperty<float3_t>(float3_t(1.0));
};

void to_json(nlohmann::ordered_json& j, const Node& node);
void from_json(const nlohmann::ordered_json& j, Node& node);
}
