#include "ForceField.h"
#include <algorithm>

namespace pixelpart {
ForceField::ForceField(id_t ownId, id_t parentId) : Node(ownId, parentId) {

}

void ForceField::type(Type type) {
	fieldType = type;
}
ForceField::Type ForceField::type() const {
	return fieldType;
}

NodeExclusionSet& ForceField::exclusionSet() {
	return fieldExclusionSet;
}
const NodeExclusionSet& ForceField::exclusionSet() const {
	return fieldExclusionSet;
}

AnimatedProperty<float3_t>& ForceField::size() {
	return fieldSize;
}
const AnimatedProperty<float3_t>& ForceField::size() const {
	return fieldSize;
}

AnimatedProperty<float3_t>& ForceField::orientation() {
	return fieldOrientation;
}
const AnimatedProperty<float3_t>& ForceField::orientation() const {
	return fieldOrientation;
}

AnimatedProperty<float_t>& ForceField::strength() {
	return fieldStrength;
}
const AnimatedProperty<float_t>& ForceField::strength() const {
	return fieldStrength;
}

AnimatedProperty<float3_t>& ForceField::accelerationDirection() {
	return fieldAccelerationDirection;
}
const AnimatedProperty<float3_t>& ForceField::accelerationDirection() const {
	return fieldAccelerationDirection;
}

AnimatedProperty<float_t>& ForceField::accelerationDirectionVariance() {
	return fieldAccelerationDirectionVariance;
}
const AnimatedProperty<float_t>& ForceField::accelerationDirectionVariance() const {
	return fieldAccelerationDirectionVariance;
}

AnimatedProperty<float_t>& ForceField::accelerationStrengthVariance() {
	return fieldAccelerationStrengthVariance;
}
const AnimatedProperty<float_t>& ForceField::accelerationStrengthVariance() const {
	return fieldAccelerationStrengthVariance;
}

void ForceField::accelerationGrid(int32_t x, int32_t y, int32_t z) {
	fieldAccelerationGridSize[0] = x;
	fieldAccelerationGridSize[1] = y;
	fieldAccelerationGridSize[2] = z;
	fieldAccelerationDirectionGrid.clear();
	fieldAccelerationStrengthGrid.clear();

	std::size_t cellCount = static_cast<std::size_t>(std::max(x * y * z, 0));
	fieldAccelerationDirectionGrid.resize(cellCount, float3_t(0.0));
	fieldAccelerationStrengthGrid.resize(cellCount, 0.0);
}
void ForceField::accelerationGrid(int32_t x, int32_t y, int32_t z,
	const std::vector<float3_t>& directionGrid, const std::vector<float_t>& strengthGrid) {
	fieldAccelerationGridSize[0] = x;
	fieldAccelerationGridSize[1] = y;
	fieldAccelerationGridSize[2] = z;
	fieldAccelerationDirectionGrid = directionGrid;
	fieldAccelerationStrengthGrid = strengthGrid;

	std::size_t cellCount = static_cast<std::size_t>(std::max(x * y * z, 0));
	fieldAccelerationDirectionGrid.resize(cellCount, float3_t(0.0));
	fieldAccelerationStrengthGrid.resize(cellCount, 0.0);
}
const std::vector<float3_t>& ForceField::accelerationDirectionGrid() const {
	return fieldAccelerationDirectionGrid;
}
const std::vector<float_t>& ForceField::accelerationStrengthGrid() const {
	return fieldAccelerationStrengthGrid;
}
int32_t ForceField::accelerationGridSizeX() const {
	return fieldAccelerationGridSize[0];
}
int32_t ForceField::accelerationGridSizeY() const {
	return fieldAccelerationGridSize[1];
}
int32_t ForceField::accelerationGridSizeZ() const {
	return fieldAccelerationGridSize[2];
}

void ForceField::vectorResourceId(const std::string& resourceId) {
	fieldVectorResourceId = resourceId;
}
const std::string& ForceField::vectorResourceId() const {
	return fieldVectorResourceId;
}

void ForceField::vectorFilter(Filter filter) {
	fieldVectorFilter = filter;
}
ForceField::Filter ForceField::vectorFilter() const {
	return fieldVectorFilter;
}

AnimatedProperty<float_t>& ForceField::vectorTightness() {
	return fieldVectorTightness;
}
const AnimatedProperty<float_t>& ForceField::vectorTightness() const {
	return fieldVectorTightness;
}

StaticProperty<int_t>& ForceField::noiseOctaves() {
	return fieldNoiseOctaves;
}
const StaticProperty<int_t>& ForceField::noiseOctaves() const {
	return fieldNoiseOctaves;
}

AnimatedProperty<float_t>& ForceField::noiseFrequency() {
	return fieldNoiseFrequency;
}
const AnimatedProperty<float_t>& ForceField::noiseFrequency() const {
	return fieldNoiseFrequency;
}

AnimatedProperty<float_t>& ForceField::noisePersistence() {
	return fieldNoisePersistence;
}
const AnimatedProperty<float_t>& ForceField::noisePersistence() const {
	return fieldNoisePersistence;
}

AnimatedProperty<float_t>& ForceField::noiseLacunarity() {
	return fieldNoiseLacunarity;
}
const AnimatedProperty<float_t>& ForceField::noiseLacunarity() const {
	return fieldNoiseLacunarity;
}

void ForceField::noiseAnimated(bool enable) {
	fieldNoiseAnimated = enable;
}
bool ForceField::noiseAnimated() const {
	return fieldNoiseAnimated;
}

StaticProperty<float_t>& ForceField::noiseAnimationTimeScale() {
	return fieldNoiseAnimationTimeScale;
}
const StaticProperty<float_t>& ForceField::noiseAnimationTimeScale() const {
	return fieldNoiseAnimationTimeScale;
}

StaticProperty<float_t>& ForceField::noiseAnimationTimeBase() {
	return fieldNoiseAnimationTimeBase;
}
const StaticProperty<float_t>& ForceField::noiseAnimationTimeBase() const {
	return fieldNoiseAnimationTimeBase;
}

StaticProperty<float_t>& ForceField::dragVelocityInfluence() {
	return fieldDragVelocityInfluence;
}
const StaticProperty<float_t>& ForceField::dragVelocityInfluence() const {
	return fieldDragVelocityInfluence;
}

StaticProperty<float_t>& ForceField::dragSizeInfluence() {
	return fieldDragSizeInfluence;
}
const StaticProperty<float_t>& ForceField::dragSizeInfluence() const {
	return fieldDragSizeInfluence;
}

void to_json(nlohmann::ordered_json& j, const ForceField& forceField) {
	j = nlohmann::ordered_json{
		{ "id", forceField.id() },
		{ "parent_id", forceField.parentId() },
		{ "name", forceField.name() },
		{ "lifetime_start", forceField.start() },
		{ "lifetime_duration", forceField.duration() },
		{ "repeat", forceField.repeat() },
		{ "position", forceField.position() },

		{ "type", forceField.type() },
		{ "exclusion_list", forceField.exclusionSet() },
		{ "size", forceField.size() },
		{ "orientation", forceField.orientation() },
		{ "strength", forceField.strength() },

		{ "attraction_field", nlohmann::ordered_json{ } },

		{ "acceleration_field", nlohmann::ordered_json{
			{ "direction", forceField.accelerationDirection() },
			{ "direction_variance", forceField.accelerationDirectionVariance() },
			{ "strength_variance", forceField.accelerationStrengthVariance() },
			{ "grid_size_x", forceField.accelerationGridSizeX() },
			{ "grid_size_y", forceField.accelerationGridSizeY() },
			{ "grid_size_z", forceField.accelerationGridSizeZ() },
			{ "grid_direction", forceField.accelerationDirectionGrid() },
			{ "grid_strength", forceField.accelerationStrengthGrid() }
		} },

		{ "vector_field", nlohmann::ordered_json{
			{ "resource_id", forceField.vectorResourceId() },
			{ "filter", forceField.vectorFilter() },
			{ "tightness", forceField.vectorTightness() }
		} },

		{ "noise_field", nlohmann::ordered_json{
			{ "octaves", forceField.noiseOctaves() },
			{ "frequency", forceField.noiseFrequency() },
			{ "persistence", forceField.noisePersistence() },
			{ "lacunarity", forceField.noiseLacunarity() },
			{ "animated", forceField.noiseAnimated() },
			{ "animation_time_scale", forceField.noiseAnimationTimeScale() },
			{ "animation_time_base", forceField.noiseAnimationTimeBase() }
		} },

		{ "drag_field", nlohmann::ordered_json{
			{ "velocity_influence", forceField.dragVelocityInfluence() },
			{ "size_influence", forceField.dragSizeInfluence() }
		} }
	};
}
void from_json(const nlohmann::ordered_json& j, ForceField& forceField) {
	forceField = ForceField(
		j.at("id"),
		j.value("parent_id", id_t()));

	forceField.name(j.value("name", ""));
	forceField.start(j.value("lifetime_start", 0.0));
	forceField.duration(j.value("lifetime_duration", 1.0));
	forceField.repeat(j.value("repeat", true));
	forceField.position() = j.value("position", AnimatedProperty<float3_t>(0.0, float3_t(0.0)));

	forceField.type(j.value("type", ForceField::Type::attraction_field));
	forceField.exclusionSet() = j.value("exclusion_list", NodeExclusionSet());
	forceField.size() = j.value("size", AnimatedProperty<float3_t>(float3_t(1.0)));
	forceField.orientation() = j.value("orientation", AnimatedProperty<float3_t>(float3_t(0.0)));
	forceField.strength() = j.value("strength", AnimatedProperty<float_t>(1.0));

	nlohmann::ordered_json jAccelerationField = j.value("acceleration_field", nlohmann::ordered_json());
	forceField.accelerationDirection() = jAccelerationField.value("direction", AnimatedProperty<float3_t>(float3_t(0.0)));
	forceField.accelerationDirectionVariance() = jAccelerationField.value("direction_variance", AnimatedProperty<float_t>(0.0));
	forceField.accelerationStrengthVariance() = jAccelerationField.value("strength_variance", AnimatedProperty<float_t>(0.0));
	forceField.accelerationGrid(
		jAccelerationField.value("grid_size_x", 1),
		jAccelerationField.value("grid_size_y", 1),
		jAccelerationField.value("grid_size_z", 1),
		jAccelerationField.value("grid_direction", std::vector<float3_t>{ float3_t(0.0) }),
		jAccelerationField.value("grid_strength", std::vector<float_t>{ 0.0 }));

	nlohmann::ordered_json jVectorField = j.value("vector_field", nlohmann::ordered_json());
	forceField.vectorResourceId(jVectorField.value("resource_id", ""));
	forceField.vectorFilter(jVectorField.value("filter", ForceField::Filter::none));
	forceField.vectorTightness() = jVectorField.value("tightness", AnimatedProperty<float_t>(1.0));

	nlohmann::ordered_json jNoiseField = j.value("noise_field", nlohmann::ordered_json());
	forceField.noiseOctaves() = jNoiseField.value("octaves", StaticProperty<int64_t>(4));
	forceField.noiseFrequency() = jNoiseField.value("frequency", AnimatedProperty<float_t>(1.0));
	forceField.noisePersistence() = jNoiseField.value("persistence", AnimatedProperty<float_t>(0.5));
	forceField.noiseLacunarity() = jNoiseField.value("lacunarity", AnimatedProperty<float_t>(2.0));
	forceField.noiseAnimated(jNoiseField.value("animated", false));
	forceField.noiseAnimationTimeScale() = jNoiseField.value("animation_time_scale", StaticProperty<float_t>(1.0));
	forceField.noiseAnimationTimeBase() = jNoiseField.value("animation_time_base", StaticProperty<float_t>(0.0));

	nlohmann::ordered_json jDragField = j.value("drag_field", nlohmann::ordered_json());
	forceField.dragVelocityInfluence() = jDragField.value("velocity_influence", StaticProperty<float_t>(1.0));
	forceField.dragSizeInfluence() = jDragField.value("size_influence", StaticProperty<float_t>(1.0));
}
}