#pragma once

#include <cstddef>
#include <vector>
#include <algorithm>

namespace pixelpart {
struct GridIndex {
	std::size_t x;
	std::size_t y;
};

inline bool operator==(const GridIndex& i1, const GridIndex& i2) {
	return i1.x == i2.x && i1.y == i2.y;
}

template <typename T>
class Grid {
public:
	Grid() {

	}
	Grid(std::size_t w, std::size_t h) :
		gridSizeX(w), gridSizeY(h) {
		gridData.resize(gridSizeX * gridSizeY);
	}
	Grid(std::size_t w, std::size_t h, const T& value) :
		gridSizeX(w), gridSizeY(h) {
		gridData.resize(gridSizeX * gridSizeY, value);
	}

	T& operator()(std::size_t i) {
		return gridData[i];
	}
	T& operator()(std::size_t x, std::size_t y) {
		return gridData[y * gridSizeX + x];
	}
	T& operator()(const GridIndex& index) {
		return gridData[index.y * gridSizeX + index.x];
	}

	const T& operator()(std::size_t i) const {
		return gridData[i];
	}
	const T& operator()(std::size_t x, std::size_t y) const {
		return gridData[y * gridSizeX + x];
	}
	const T& operator()(const GridIndex& index) const {
		return gridData[index.y * gridSizeX + index.x];
	}

	T& value(std::size_t x, std::size_t y, const T& defaultValue) {
		if(contains(x, y)) {
			return operator()(x, y);
		}

		return defaultValue;
	}
	T& value(const GridIndex& index, const T& defaultValue) {
		if(contains(index.x, index.y)) {
			return operator()(index);
		}

		return defaultValue;
	}
	const T& value(std::size_t x, std::size_t y, const T& defaultValue) const {
		if(contains(x, y)) {
			return operator()(x, y);
		}

		return defaultValue;
	}
	const T& value(const GridIndex& index, const T& defaultValue) const {
		if(contains(index.x, index.y)) {
			return operator()(index);
		}

		return defaultValue;
	}

	void assign(const T& value) {
		gridData.assign(gridData.size(), value);
	}
	void assign(std::size_t w, std::size_t h, const T& value) {
		gridSizeX = w;
		gridSizeY = h;
		gridData.assign(gridSizeX * gridSizeY, value);
	}
	void resize(std::size_t w, std::size_t h) {
		gridSizeX = w;
		gridSizeY = h;
		gridData.resize(gridSizeX * gridSizeY);
	}

	bool contains(std::size_t x, std::size_t y) const {
		return
			x >= 0 && x < gridSizeX &&
			y >= 0 && y < gridSizeY;
	}

	const std::vector<T>& cells() const {
		return gridData;
	}

	std::size_t width() const {
		return gridSizeX;
	}
	std::size_t height() const {
		return gridSizeY;
	}
	std::size_t size() const {
		return gridSizeX * gridSizeY;
	}

	template <typename Fn>
	void forEach1d(Fn&& func) const {
		for(std::size_t i = 0; i < gridSizeX * gridSizeY; i++) {
			func(i);
		}
	}
	template <typename Fn>
	void forEach1d(Fn&& func, std::size_t from, std::size_t to) const {
		for(std::size_t i = from; i < std::min(to, gridSizeX * gridSizeY); i++) {
			func(i);
		}
	}

	template <typename Fn>
	void forEach2d(Fn&& func) const {
		for(std::size_t y = 0; y < gridSizeY; y++) {
			for(std::size_t x = 0; x < gridSizeX; x++) {
				func(x, y);
			}
		}
	}
	template <typename Fn>
	void forEach2d(Fn&& func, std::size_t fromX, std::size_t toX, std::size_t fromY, std::size_t toY) const {
		for(std::size_t y = fromY; y < std::min(toY, gridSizeY); y++) {
			for(std::size_t x = fromX; x < std::min(toX, gridSizeX); x++) {
				func(x, y);
			}
		}
	}

private:
	std::size_t gridSizeX = 0;
	std::size_t gridSizeY = 0;

	std::vector<T> gridData;
};
}
