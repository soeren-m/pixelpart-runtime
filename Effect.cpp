#include "Effect.h"
#include "JSONUtil.h"
#include <algorithm>

namespace pixelpart {
Effect::Effect() {

}

void Effect::setParticleEmitters(const std::vector<ParticleEmitter>& emitters) {
	particleEmitters = emitters;
	refreshParticleEmitterIndexMap();
}
uint32_t Effect::addParticleEmitter(const ParticleEmitter& emitter, uint32_t idOffset) {
	uint32_t emitterId = idOffset + ((emitter.id != NullId) ? emitter.id : 0);
	while(hasParticleEmitter(emitterId)) {
		emitterId++;
		idOffset++;
	}

	particleEmitters.push_back(emitter);

	ParticleEmitter& insertedEmitter = particleEmitters.back();
	insertedEmitter.id = emitterId;
	if(insertedEmitter.parentId != NullId) {
		insertedEmitter.parentId += idOffset;
	}

	refreshParticleEmitterIndexMap();

	return emitterId;
}
uint32_t Effect::copyParticleEmitter(uint32_t id) {
	if(!hasParticleEmitter(id)) {
		return NullId;
	}

	ParticleEmitter emitterCopy = getParticleEmitter(id);
	emitterCopy.id = NullId;
	emitterCopy.parentId = NullId;

	do {
		emitterCopy.name += "(copy)";
	} while(isNameUsed(emitterCopy.name));

	return addParticleEmitter(emitterCopy);
}
void Effect::removeParticleEmitter(uint32_t id) {
	removeParticleEmitterAtIndex(findParticleEmitterById(id));
}
void Effect::removeParticleEmitterAtIndex(uint32_t index) {
	if(index >= particleEmitters.size()) {
		return;
	}

	uint32_t emitterId = particleEmitters[index].id;
	for(ParticleEmitter& emitter : particleEmitters) {
		if(emitter.id == emitterId) {
			continue;
		}
		
		if(emitter.parentId == emitterId) {
			emitter.parentId = particleEmitters[index].parentId;
		}
	}

	for(ForceField& forceField : forceFields) {
		forceField.emitterMask[emitterId] = true;
	}

	for(Collider& collider : colliders) {
		collider.emitterMask[emitterId] = true;
	}
 
	particleEmitters.erase(particleEmitters.begin() + index);
	refreshParticleEmitterIndexMap();
}
void Effect::setParticleEmitterParent(uint32_t id, uint32_t parentId) {
	if(!hasParticleEmitter(id)) {
		return;
	}

	ParticleEmitter& emitter = getParticleEmitter(id);

	if(emitter.parentId != parentId) {
		if(isParticleEmitterDescendent(parentId, id)) {
			for(ParticleEmitter& otherEmitter : particleEmitters) {
				if(otherEmitter.parentId == id) {
					otherEmitter.parentId = emitter.parentId;
				}
			}
		}

		emitter.parentId = parentId;
	}
}
void Effect::refreshParticleEmitterIndexMap() {
	particleEmitterIndexMap.clear();
	particleEmitterIndexMap.reserve(particleEmitters.size());

	for(uint32_t i = 0; i < particleEmitters.size(); i++) {
		uint32_t emitterId = particleEmitters[i].id;
		if(emitterId >= particleEmitterIndexMap.size()) {
			particleEmitterIndexMap.resize(emitterId + 1, NullId);
		}

		particleEmitterIndexMap[emitterId] = i;
	}
}
bool Effect::hasParticleEmitter(uint32_t id) const {
	return findParticleEmitterById(id) != NullId;
}
bool Effect::hasParticleEmitterWithParent(uint32_t parentId) const {
	return findParticleEmitterByParent(parentId) != NullId;
}
bool Effect::hasParticleEmitterWithName(const std::string& name) const {
	return findParticleEmitterByName(name) != NullId;
}
bool Effect::isParticleEmitterDescendent(uint32_t id, uint32_t ancestorId) const {
	uint32_t currentId = id;
	while(hasParticleEmitter(currentId)) {
		const ParticleEmitter& emitter = getParticleEmitter(currentId);
		if(emitter.parentId == ancestorId) {
			return true;
		}

		currentId = emitter.parentId;
	}

	return false;
}
uint32_t Effect::getMaxParticleEmitterId() const {
	uint32_t maxEmitterId = 0;
	for(const ParticleEmitter& emitter : particleEmitters) {
		maxEmitterId = std::max(maxEmitterId, emitter.id);
	}

	return maxEmitterId;
}
uint32_t Effect::getNumParticleEmitters() const {
	return particleEmitters.size();
}
ParticleEmitter& Effect::getParticleEmitter(uint32_t id) {
	return particleEmitters.at(findParticleEmitterById(id));
}
ParticleEmitter& Effect::getParticleEmitterByIndex(uint32_t index) {
	return particleEmitters.at(index);
}
ParticleEmitter& Effect::getParticleEmitterByLayer(uint32_t layer) {
	return particleEmitters.at(findParticleEmitterByLayer(layer));
}
ParticleEmitter& Effect::getParticleEmitterByName(const std::string& name) {
	return particleEmitters.at(findParticleEmitterByName(name));
}
const ParticleEmitter& Effect::getParticleEmitter(uint32_t id) const {
	return particleEmitters.at(findParticleEmitterById(id));
}
const ParticleEmitter& Effect::getParticleEmitterByIndex(uint32_t index) const {
	return particleEmitters.at(index);
}
const ParticleEmitter& Effect::getParticleEmitterByLayer(uint32_t layer) const {
	return particleEmitters.at(findParticleEmitterByLayer(layer));
}
const ParticleEmitter& Effect::getParticleEmitterByName(const std::string& name) const {
	return particleEmitters.at(findParticleEmitterByName(name));
}
const std::vector<ParticleEmitter>& Effect::getParticleEmitters() const {
	return particleEmitters;
}
std::vector<uint32_t> Effect::getParticleEmittersSortedById(bool ascending) const {
	std::vector<uint32_t> sortedIndices(particleEmitters.size());
	for(uint32_t i = 0; i < particleEmitters.size(); i++) {
		sortedIndices[i] = i;
	}

	std::sort(sortedIndices.begin(), sortedIndices.end(), [this, ascending](uint32_t i1, uint32_t i2) {
		return ascending ? (particleEmitters[i1].id < particleEmitters[i2].id) : (particleEmitters[i1].id > particleEmitters[i2].id);
	});

	return sortedIndices;
}
std::vector<uint32_t> Effect::getParticleEmittersSortedByLayer(bool ascending) const {
	std::vector<uint32_t> sortedIndices(particleEmitters.size());
	for(uint32_t i = 0; i < particleEmitters.size(); i++) {
		sortedIndices[i] = i;
	}

	std::sort(sortedIndices.begin(), sortedIndices.end(), [this, ascending](uint32_t i1, uint32_t i2) {
		return ascending ? (particleEmitters[i1].layer < particleEmitters[i2].layer) : (particleEmitters[i1].layer > particleEmitters[i2].layer);
	});

	return sortedIndices;
}
std::vector<ParticleEmitter> Effect::getParticleEmittersWithParent(uint32_t parentId) const {
	std::vector<ParticleEmitter> particleEmittersWithParent;
	std::copy_if(particleEmitters.begin(), particleEmitters.end(), std::back_inserter(particleEmittersWithParent), [parentId](const ParticleEmitter& e) {
		return e.parentId == parentId;
	});

	return particleEmittersWithParent;
}
uint32_t Effect::findParticleEmitterById(uint32_t id) const {
	return (id < particleEmitterIndexMap.size()) 
		? particleEmitterIndexMap[id]
		: NullId;
}
uint32_t Effect::findParticleEmitterByParent(uint32_t parentId) const {
	for(uint32_t i = 0; i < particleEmitters.size(); i++) {
		if(particleEmitters[i].parentId == parentId) {
			return i;
		}
	}

	return NullId;
}
uint32_t Effect::findParticleEmitterByLayer(uint32_t layer) const {
	for(uint32_t i = 0; i < particleEmitters.size(); i++) {
		if(particleEmitters[i].layer == layer) {
			return i;
		}
	}

	return NullId;
}
uint32_t Effect::findParticleEmitterByName(const std::string& name) const {
	for(uint32_t i = 0; i < particleEmitters.size(); i++) {
		if(particleEmitters[i].name == name) {
			return i;
		}
	}

	return NullId;
}

void Effect::setForceFields(const std::vector<ForceField>& forceFieldList) {
	forceFields = forceFieldList;
}
uint32_t Effect::addForceField(const ForceField& forceField) {
	forceFields.push_back(forceField);

	return forceFields.size() - 1;
}
uint32_t Effect::copyForceField(uint32_t index) {
	if(!hasForceField(index)) {
		return NullId;
	}

	ForceField forceFieldCopy = getForceField(index);

	do {
		forceFieldCopy.name += "(copy)";
	} while(isNameUsed(forceFieldCopy.name));

	return addForceField(forceFieldCopy);
}
void Effect::removeForceField(uint32_t index) {
	if(!hasForceField(index)) {
		return;
	}

	forceFields.erase(forceFields.begin() + index);
}
bool Effect::hasForceField(uint32_t index) const {
	return index < forceFields.size();
}
bool Effect::hasForceFieldWithName(const std::string& name) const {
	return findForceFieldByName(name) != NullId;
}
uint32_t Effect::getNumForceFields() const {
	return forceFields.size();
}
ForceField& Effect::getForceField(uint32_t index) {
	return forceFields.at(index);
}
ForceField& Effect::getForceFieldByName(const std::string& name) {
	return forceFields.at(findForceFieldByName(name));
}
const ForceField& Effect::getForceField(uint32_t index) const {
	return forceFields.at(index);
}
const ForceField& Effect::getForceFieldByName(const std::string& name) const {
	return forceFields.at(findForceFieldByName(name));
}
const std::vector<ForceField>& Effect::getForceFields() const {
	return forceFields;
}
uint32_t Effect::findForceFieldByName(const std::string& name) const {
	for(uint32_t i = 0; i < forceFields.size(); i++) {
		if(forceFields[i].name == name) {
			return i;
		}
	}

	return NullId;
}

void Effect::setColliders(const std::vector<Collider>& colliderList) {
	colliders = colliderList;
}
uint32_t Effect::addCollider(const Collider& collider) {
	colliders.push_back(collider);

	return colliders.size() - 1;
}
uint32_t Effect::copyCollider(uint32_t index) {
	if(!hasCollider(index)) {
		return NullId;
	}

	Collider colliderCopy = getCollider(index);

	do {
		colliderCopy.name += "(copy)";
	} while(isNameUsed(colliderCopy.name));

	return addCollider(colliderCopy);
}
void Effect::removeCollider(uint32_t index) {
	if(!hasCollider(index)) {
		return;
	}

	colliders.erase(colliders.begin() + index);
}
bool Effect::hasCollider(uint32_t index) const {
	return index < colliders.size();
}
bool Effect::hasColliderWithName(const std::string& name) const {
	return findColliderByName(name) != NullId;
}
uint32_t Effect::getNumColliders() const {
	return colliders.size();
}
Collider& Effect::getCollider(uint32_t index) {
	return colliders.at(index);
}
Collider& Effect::getColliderByName(const std::string& name) {
	return colliders.at(findColliderByName(name));
}
const Collider& Effect::getCollider(uint32_t index) const {
	return colliders.at(index);
}
const Collider& Effect::getColliderByName(const std::string& name) const {
	return colliders.at(findColliderByName(name));
}
const std::vector<Collider>& Effect::getColliders() const {
	return colliders;
}
uint32_t Effect::findColliderByName(const std::string& name) const {
	for(uint32_t i = 0; i < colliders.size(); i++) {
		if(colliders[i].name == name) {
			return i;
		}
	}

	return NullId;
}

void Effect::setSprites(const std::vector<Sprite>& nodes) {
	sprites = nodes;
}
uint32_t Effect::addSprite(const Sprite& node) {
	sprites.push_back(node);

	return sprites.size() - 1;
}
uint32_t Effect::copySprite(uint32_t index) {
	if(!hasSprite(index)) {
		return NullId;
	}

	Sprite spriteCopy = getSprite(index);

	do {
		spriteCopy.name += "(copy)";
	} while(isNameUsed(spriteCopy.name));

	return addSprite(spriteCopy);
}
void Effect::removeSprite(uint32_t index) {
	if(!hasSprite(index)) {
		return;
	}

	sprites.erase(sprites.begin() + index);
}
void Effect::removeSprites() {
	sprites.clear();
}
bool Effect::hasSprite(uint32_t index) const {
	return index < sprites.size();
}
bool Effect::hasSpriteWithName(const std::string& name) const {
	return findSpriteByName(name) != NullId;
}
uint32_t Effect::getNumSprites() const {
	return sprites.size();
}
Sprite& Effect::getSprite(uint32_t index) {
	return sprites.at(index);
}
Sprite& Effect::getSpriteByName(const std::string& name) {
	return sprites.at(findSpriteByName(name));
}
const Sprite& Effect::getSprite(uint32_t index) const {
	return sprites.at(index);
}
const Sprite& Effect::getSpriteByName(const std::string& name) const {
	return sprites.at(findSpriteByName(name));
}
const std::vector<Sprite>& Effect::getSprites() const {
	return sprites;
}
std::vector<uint32_t> Effect::getSpritesSortedByLayer(bool ascending) const {
	std::vector<uint32_t> sortedIndices(sprites.size());
	for(uint32_t i = 0; i < sprites.size(); i++) {
		sortedIndices[i] = i;
	}

	std::stable_sort(sortedIndices.begin(), sortedIndices.end(), [this, ascending](uint32_t i1, uint32_t i2) {
		return ascending ? (sprites[i1].layer < sprites[i2].layer) : (sprites[i1].layer > sprites[i2].layer);
	});

	return sortedIndices;
}
uint32_t Effect::findSpriteByName(const std::string& name) const {
	for(uint32_t i = 0; i < sprites.size(); i++) {
		if(sprites[i].name == name) {
			return i;
		}
	}

	return NullId;
}

uint32_t Effect::getMaxLayer() const {
	uint32_t maxLayer = 0;
	for(const ParticleEmitter& emitter : particleEmitters) {
		maxLayer = std::max(maxLayer, emitter.layer);
	}
	for(const Sprite& sprite : sprites) {
		maxLayer = std::max(maxLayer, sprite.layer);
	}

	return maxLayer;
}

bool Effect::isNameUsed(const std::string& name) const {
	for(const ParticleEmitter& emitter : particleEmitters) {
		if(emitter.name == name) {
			return true;
		}
	}
	for(const ForceField& forceField : forceFields) {
		if(forceField.name == name) {
			return true;
		}
	}
	for(const Collider& collider : colliders) {
		if(collider.name == name) {
			return true;
		}
	}
	for(const Sprite& sprite : sprites) {
		if(sprite.name == name) {
			return true;
		}
	}

	return false;
}
bool Effect::isResourceUsed(const std::string& resourceId) const {
	for(const ParticleEmitter& emitter : particleEmitters) {
		if(emitter.particleSprite.id == resourceId) {
			return true;
		}
	}

	for(const Sprite& sprite : sprites) {
		if(sprite.image.id == resourceId) {
			return true;
		}
	}

	return false;
}

void to_json(nlohmann::ordered_json& j, const Effect& effect) {
	j = nlohmann::ordered_json{
		{ "emitters", effect.getParticleEmitters() },
		{ "force_fields", effect.getForceFields() },
		{ "colliders", effect.getColliders() },
		{ "sprites", effect.getSprites() }
	};
}
void from_json(const nlohmann::ordered_json& j, Effect& effect) {
	std::vector<ParticleEmitter> particleEmitters;
	std::vector<ForceField> forceFields;
	std::vector<Collider> colliders;
	std::vector<Sprite> sprites;

	fromJson(particleEmitters, j, "emitters", "");
	fromJson(forceFields, j, "force_fields", "");
	fromJson(colliders, j, "colliders", "");
	fromJson(sprites, j, "sprites", "");

	const nlohmann::ordered_json& jEmitterArray = j.at("emitters");

	for(uint32_t i = 0; i < particleEmitters.size(); i++) {
		const ParticleEmitter& emitter = particleEmitters[i];

		if(jEmitterArray[i].contains("particle_spawn_on_step_emitter")) {
			uint32_t subEmitterId = NullId;
			floatd subEmitterSpawnProb = 1.0;
			fromJson(subEmitterId, jEmitterArray[i], "particle_spawn_on_step_emitter", "");
			fromJson(subEmitterSpawnProb, jEmitterArray[i], "particle_spawn_on_step_prob", "");
	
			for(ParticleEmitter& subEmitter : particleEmitters) {
				if(subEmitter.id == subEmitterId) {
					subEmitter.parentId = emitter.id;
					subEmitter.instantiationMode = ParticleEmitter::InstantiationMode::continuous;
					subEmitter.numParticles.moveByFactor(subEmitterSpawnProb);
				}
			}
		}

		if(jEmitterArray[i].contains("particle_spawn_on_death_emitter")) {
			uint32_t subEmitterId = NullId;
			floatd subEmitterSpawnProb = 1.0;
			fromJson(subEmitterId, jEmitterArray[i], "particle_spawn_on_death_emitter", "");
			fromJson(subEmitterSpawnProb, jEmitterArray[i], "particle_spawn_on_death_prob", "");
	
			for(ParticleEmitter& subEmitter : particleEmitters) {
				if(subEmitter.id == subEmitterId) {
					subEmitter.parentId = emitter.id;
					subEmitter.instantiationMode = ParticleEmitter::InstantiationMode::burst_death;
					if(jEmitterArray[i].contains("particle_spawn_on_death_number")) {
						subEmitter.numParticles = Curve<floatd>(static_cast<floatd>(jEmitterArray[i].at("particle_spawn_on_death_number").get<uint32_t>()));
						subEmitter.numParticles.moveByFactor(subEmitterSpawnProb);
					}
				}
			}
		}

		if(jEmitterArray[i].contains("force_fields")) {
			std::vector<ForceField> emitterForceFields = jEmitterArray[i].at("force_fields").get<std::vector<ForceField>>();
			for(ForceField& forceField : emitterForceFields) {
				forceField.emitterMask.reset();
				forceField.emitterMask[emitter.id] = true;
				forceField.lifetimeStart = 0.0;
				forceField.lifetimeDuration = 1.0;
				forceField.repeat = true;
				forceFields.push_back(forceField);
			}
		}

		if(jEmitterArray[i].contains("colliders")) {
			std::vector<Collider> emitterColliders = jEmitterArray[i].at("colliders").get<std::vector<Collider>>();
			for(Collider& collider : emitterColliders) {
				collider.emitterMask.reset();
				collider.emitterMask[emitter.id] = true;
				collider.lifetimeStart = 0.0;
				collider.lifetimeDuration = 1.0;
				collider.repeat = true;
				colliders.push_back(collider);
			}
		}
	}

	effect.setParticleEmitters(particleEmitters);
	effect.setForceFields(forceFields);
	effect.setColliders(colliders);
	effect.setSprites(sprites);
}
}