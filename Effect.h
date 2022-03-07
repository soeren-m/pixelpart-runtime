#pragma once

#include "ParticleEmitter.h"
#include "ForceField.h"
#include "Collider.h"
#include "Sprite.h"

namespace pixelpart {
class Effect {
public:
	Effect();

	void setParticleEmitters(const std::vector<ParticleEmitter>& emitters);
	uint32_t addParticleEmitter(const ParticleEmitter& emitter, uint32_t idOffset = 0);
	uint32_t copyParticleEmitter(uint32_t id);
	void removeParticleEmitter(uint32_t id);
	void removeParticleEmitterAtIndex(uint32_t index);
	void setParticleEmitterParent(uint32_t id, uint32_t parentId);
	void refreshParticleEmitterIndexMap();
	bool hasParticleEmitter(uint32_t id) const;
	bool hasParticleEmitterWithParent(uint32_t parentId) const;
	bool hasParticleEmitterWithName(const std::string& name) const;
	bool isParticleEmitterDescendent(uint32_t id, uint32_t ancestorId) const;
	uint32_t getMaxParticleEmitterId() const;
	uint32_t getNumParticleEmitters() const;
	ParticleEmitter& getParticleEmitter(uint32_t id);
	ParticleEmitter& getParticleEmitterByIndex(uint32_t index);	
	ParticleEmitter& getParticleEmitterByLayer(uint32_t layer);
	ParticleEmitter& getParticleEmitterByName(const std::string& name);
	const ParticleEmitter& getParticleEmitter(uint32_t id) const;
	const ParticleEmitter& getParticleEmitterByIndex(uint32_t index) const;
	const ParticleEmitter& getParticleEmitterByLayer(uint32_t layer) const;
	const ParticleEmitter& getParticleEmitterByName(const std::string& name) const;
	const std::vector<ParticleEmitter>& getParticleEmitters() const;
	std::vector<uint32_t> getParticleEmittersSortedById(bool ascending = true) const;
	std::vector<uint32_t> getParticleEmittersSortedByLayer(bool ascending = true) const;
	std::vector<ParticleEmitter> getParticleEmittersWithParent(uint32_t parentId) const;
	uint32_t findParticleEmitterById(uint32_t id) const;
	uint32_t findParticleEmitterByParent(uint32_t parentId) const;
	uint32_t findParticleEmitterByLayer(uint32_t layer) const;
	uint32_t findParticleEmitterByName(const std::string& name) const;

	void setForceFields(const std::vector<ForceField>& forceFieldList);
	uint32_t addForceField(const ForceField& forceField);
	uint32_t copyForceField(uint32_t index);
	void removeForceField(uint32_t index);
	bool hasForceField(uint32_t index) const;
	bool hasForceFieldWithName(const std::string& name) const;
	uint32_t getNumForceFields() const;
	ForceField& getForceField(uint32_t index);
	ForceField& getForceFieldByName(const std::string& name);
	const ForceField& getForceField(uint32_t index) const;
	const ForceField& getForceFieldByName(const std::string& name) const;
	const std::vector<ForceField>& getForceFields() const;
	uint32_t findForceFieldByName(const std::string& name) const;

	void setColliders(const std::vector<Collider>& colliderList);
	uint32_t addCollider(const Collider& collider);
	uint32_t copyCollider(uint32_t index);
	void removeCollider(uint32_t index);
	bool hasCollider(uint32_t index) const;
	bool hasColliderWithName(const std::string& name) const;
	uint32_t getNumColliders() const;
	Collider& getCollider(uint32_t index);
	Collider& getColliderByName(const std::string& name);
	const Collider& getCollider(uint32_t index) const;
	const Collider& getColliderByName(const std::string& name) const;
	const std::vector<Collider>& getColliders() const;
	uint32_t findColliderByName(const std::string& name) const;

	void setSprites(const std::vector<Sprite>& nodes);
	uint32_t addSprite(const Sprite& node);
	uint32_t copySprite(uint32_t index);
	void removeSprite(uint32_t index);
	void removeSprites();
	bool hasSprite(uint32_t index) const;
	bool hasSpriteWithName(const std::string& name) const;
	uint32_t getNumSprites() const;
	Sprite& getSprite(uint32_t index);
	Sprite& getSpriteByName(const std::string& name);
	const Sprite& getSprite(uint32_t index) const;
	const Sprite& getSpriteByName(const std::string& name) const;
	const std::vector<Sprite>& getSprites() const;
	std::vector<uint32_t> getSpritesSortedByLayer(bool ascending = true) const;
	uint32_t findSpriteByName(const std::string& name) const;

	uint32_t getMaxLayer() const;

	bool isNameUsed(const std::string& name) const;
	bool isResourceUsed(const std::string& resourceId) const;

private:
	std::vector<ParticleEmitter> particleEmitters;
	std::vector<ForceField> forceFields;
	std::vector<Collider> colliders;
	std::vector<Sprite> sprites;

	std::vector<uint32_t> particleEmitterIndexMap;
};

void to_json(nlohmann::ordered_json& j, const Effect& effect);
void from_json(const nlohmann::ordered_json& j, Effect& effect);
}