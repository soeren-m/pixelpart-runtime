#pragma once

#include "../common/Types.h"
#include "../common/Math.h"
#include "../common/Grid.h"
#include <utility>
#include <vector>
#include <unordered_set>

namespace pixelpart {
class LineQueryGrid {
public:
	using Line = std::pair<float2_t, float2_t>;
	using QueryResult = std::unordered_set<std::uint32_t>;

	LineQueryGrid() = default;

	QueryResult queryPoint(const float2_t& position, const float2_t& size) const;
	QueryResult queryLine(const float2_t& start, const float2_t& end, const float2_t& size) const;

	void build(const std::vector<Line>& lines, std::uint32_t cellCountFactor = 3, float_t padding = 0.25);
	void clear();

private:
	void findMatches(QueryResult& results, std::int32_t cx, std::int32_t cy, const float2_t& size) const;

	GridIndex<std::int32_t> gridIndex(const float2_t& position) const;

	Grid<std::vector<std::uint32_t>> grid = Grid<std::vector<std::uint32_t>>(1, 1);
	float2_t gridBottom = float2_t(0.0);
	float2_t gridTop = float2_t(0.0);
	float2_t gridCellDimension = float2_t(1.0);
};
}