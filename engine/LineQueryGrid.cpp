#include "LineQueryGrid.h"
#include <algorithm>

namespace pixelpart {
LineQueryGrid::QueryResult LineQueryGrid::queryPoint(const float2_t& position, const float2_t& size) const {
	GridIndex<int32_t> index = gridIndex(position);

	QueryResult result;
	findMatches(result, index.x, index.y, size);

	return result;
}
LineQueryGrid::QueryResult LineQueryGrid::queryLine(const float2_t& start, const float2_t& end, const float2_t& size) const {
	GridIndex<int32_t> startIndex = gridIndex(start);
	GridIndex<int32_t> endIndex = gridIndex(end);

	if(startIndex.x > endIndex.x) {
		std::swap(startIndex.x, endIndex.x);
	}
	if(startIndex.y > endIndex.y) {
		std::swap(startIndex.y, endIndex.y);
	}

	QueryResult result;
	for(int32_t cy = startIndex.y; cy <= endIndex.y; cy++) {
		for(int32_t cx = startIndex.x; cx <= endIndex.x; cx++) {
			findMatches(result, cx, cy, size);
		}
	}

	return result;
}

void LineQueryGrid::build(const std::vector<Line>& lines, uint32_t cellCountFactor, float_t padding) {
	gridBottom = float2_t(+FLT_MAX, +FLT_MAX);
	gridTop = float2_t(-FLT_MAX, -FLT_MAX);
	for(const Line& line : lines) {
		gridBottom = glm::min(gridBottom, glm::min(line.first, line.second));
		gridTop = glm::max(gridTop, glm::max(line.first, line.second));
	}

	gridBottom -= float2_t(padding);
	gridTop += float2_t(padding);

	uint32_t gridNumColumns = std::max(cellCountFactor * static_cast<uint32_t>(
		(gridTop.x - gridBottom.x) * glm::sqrt(static_cast<float_t>(lines.size()) / ((gridTop.x - gridBottom.x) * (gridTop.y - gridBottom.y)))),
			1u);
	uint32_t gridNumRows = std::max(cellCountFactor * static_cast<uint32_t>(
		(gridTop.y - gridBottom.y) * glm::sqrt(static_cast<float_t>(lines.size()) / ((gridTop.x - gridBottom.x) * (gridTop.y - gridBottom.y)))),
			1u);
	grid.resize(gridNumColumns, gridNumRows);
	grid.clear();

	gridCellDimension = (gridTop - gridBottom) / float2_t(gridNumColumns, gridNumRows);

	for(uint32_t lineIndex = 0u; lineIndex < lines.size(); lineIndex++) {
		const Line& line = lines[lineIndex];
		float2_t direction = glm::normalize(line.second - line.first);
		float2_t p1Grid = line.first - gridBottom;
		float2_t p2Grid = line.second - gridBottom;
		float2_t delta = float2_t(0.0);
		float_t tX = 0.0;
		float_t tY = 0.0;

		if(direction.x < 0.0) {
			delta.x = -gridCellDimension.x / direction.x;
			tX = (glm::floor(p1Grid.x / gridCellDimension.x) * gridCellDimension.x - p1Grid.x) / direction.x;
		}
		else {
			delta.x = +gridCellDimension.x / direction.x;
			tX = ((glm::floor(p1Grid.x / gridCellDimension.x) + 1) * gridCellDimension.x - p1Grid.x) / direction.x;
		}
		if(direction.y < 0.0) {
			delta.y = -gridCellDimension.y / direction.y;
			tY = (glm::floor(p1Grid.y / gridCellDimension.y) * gridCellDimension.y - p1Grid.y) / direction.y;
		}
		else {
			delta.y = +gridCellDimension.y / direction.y;
			tY = ((glm::floor(p1Grid.y / gridCellDimension.y) + 1) * gridCellDimension.y - p1Grid.y) / direction.y;
		}

		int32_t cX = std::min(std::max(static_cast<int32_t>(glm::floor(p1Grid.x / gridCellDimension.x)), 0), static_cast<int32_t>(gridNumColumns) - 1);
		int32_t cY = std::min(std::max(static_cast<int32_t>(glm::floor(p1Grid.y / gridCellDimension.y)), 0), static_cast<int32_t>(gridNumRows) - 1);
		int32_t exitCX = std::min(std::max(static_cast<int32_t>(glm::floor(p2Grid.x / gridCellDimension.x)), 0), static_cast<int32_t>(gridNumColumns) - 1);
		int32_t exitCY = std::min(std::max(static_cast<int32_t>(glm::floor(p2Grid.y / gridCellDimension.y)), 0), static_cast<int32_t>(gridNumRows) - 1);

		while(grid.contains(cX, cY)) {
			grid(cX, cY).push_back(lineIndex);

			if(tX < tY) {
				tX += delta.x;
				cX += (direction.x < 0.0) ? -1 : +1;
			}
			else {
				tY += delta.y;
				cY += (direction.y < 0.0) ? -1 : +1;
			}

			if(cX == exitCX && cY == exitCY) {
				break;
			}
		}
	}
}
void LineQueryGrid::clear() {
	grid = Grid<std::vector<uint32_t>>(1u, 1u);
	gridBottom = float2_t(0.0);
	gridTop = float2_t(0.0);
	gridCellDimension = float2_t(1.0);
}

void LineQueryGrid::findMatches(QueryResult& results, int32_t cx, int32_t cy, const float2_t& size) const {
	int32_t radiusX = 1 + static_cast<int32_t>(size.x / gridCellDimension.x);
	int32_t radiusY = 1 + static_cast<int32_t>(size.y / gridCellDimension.y);

	for(int32_t y = cy - radiusY; y <= cy + radiusY && y < static_cast<int32_t>(grid.height()); y++) {
		for(int32_t x = cx - radiusX; x <= cx + radiusX && x < static_cast<int32_t>(grid.width()); x++) {
			if(x < 0 || y < 0) {
				continue;
			}

			for(uint32_t lineIndex : grid(x, y)) {
				results.insert(lineIndex);
			}
		}
	}
}

GridIndex<int32_t> LineQueryGrid::gridIndex(const float2_t& position) const {
	int32_t x = std::min(std::max(static_cast<int32_t>(glm::floor((position.x - gridBottom.x) / gridCellDimension.x)), 0), static_cast<int32_t>(grid.width()) - 1);
	int32_t y = std::min(std::max(static_cast<int32_t>(glm::floor((position.y - gridBottom.y) / gridCellDimension.y)), 0), static_cast<int32_t>(grid.height()) - 1);

	return GridIndex<int32_t>{ x, y };
}
}