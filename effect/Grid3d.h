#pragma once

#include <cstddef>
#include <vector>
#include <algorithm>

namespace pixelpart {
struct GridIndex3d {
	std::size_t x;
	std::size_t y;
	std::size_t z;
};

inline bool operator==(const GridIndex3d& i1, const GridIndex3d& i2) {
	return i1.x == i2.x &&
		i1.y == i2.y &&
		i1.z == i2.z;
}

template <typename T>
class Grid3d {
public:
	Grid3d() {

	}
	Grid3d(std::size_t w, std::size_t h, std::size_t d) :
		gridSizeX(w), gridSizeY(h), gridSizeZ(d) {
		gridData.resize(w * h * d);
	}
	Grid3d(std::size_t w, std::size_t h, std::size_t d, const T& value) :
		gridSizeX(w), gridSizeY(h), gridSizeZ(d) {
		gridData.resize(w * h * d, value);
	}

	T& operator()(std::size_t i) {
		return gridData[i];
	}
	T& operator()(std::size_t x, std::size_t y, std::size_t z) {
		return gridData[
			z * gridSizeY * gridSizeX +
			y * gridSizeX +
			x];
	}
	T& operator()(const GridIndex3d& index) {
		return gridData[
			index.z * gridSizeY * gridSizeX +
			index.y * gridSizeX +
			index.x];
	}

	const T& operator()(std::size_t i) const {
		return gridData[i];
	}
	const T& operator()(std::size_t x, std::size_t y, std::size_t z) const {
		return gridData[
			z * gridSizeY * gridSizeX +
			y * gridSizeX +
			x];
	}
	const T& operator()(const GridIndex3d& index) const {
		return gridData[
			index.z * gridSizeY * gridSizeX +
			index.y * gridSizeX +
			index.x];
	}

	T& value(std::size_t x, std::size_t y, std::size_t z, const T& defaultValue) {
		if(contains(x, y, z)) {
			return operator()(x, y, z);
		}

		return defaultValue;
	}
	T& value(const GridIndex3d& index, const T& defaultValue) {
		if(contains(index.x, index.y, index.z)) {
			return operator()(index);
		}

		return defaultValue;
	}
	const T& value(std::size_t x, std::size_t y, std::size_t z, const T& defaultValue) const {
		if(contains(x, y, z)) {
			return operator()(x, y, z);
		}

		return defaultValue;
	}
	const T& value(const GridIndex3d& index, const T& defaultValue) const {
		if(contains(index.x, index.y, index.z)) {
			return operator()(index);
		}

		return defaultValue;
	}

	void assign(const T& value) {
		gridData.assign(gridData.size(), value);
	}
	void assign(std::size_t w, std::size_t h, std::size_t d, const T& value) {
		gridSizeX = w;
		gridSizeY = h;
		gridSizeZ = d;
		gridData.assign(gridSizeX * gridSizeY * gridSizeZ, value);
	}
	void resize(std::size_t w, std::size_t h, std::size_t d) {
		gridSizeX = w;
		gridSizeY = h;
		gridSizeZ = d;
		gridData.resize(gridSizeX * gridSizeY * gridSizeZ);
	}

	bool contains(std::size_t x, std::size_t y, std::size_t z) const {
		return
			x >= 0 && x < gridSizeX &&
			y >= 0 && y < gridSizeY &&
			z >= 0 && z < gridSizeZ;
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
	std::size_t depth() const {
		return gridSizeZ;
	}
	std::size_t size() const {
		return gridSizeX * gridSizeY * gridSizeZ;
	}

	template <typename Fn>
	void forEach1d(Fn&& func) const {
		for(std::size_t i = 0; i < gridSizeX * gridSizeY * gridSizeZ; i++) {
			func(i);
		}
	}
	template <typename Fn>
	void forEach1d(Fn&& func, std::size_t from, std::size_t to) const {
		for(std::size_t i = from; i < std::min(to, gridSizeX * gridSizeY * gridSizeZ); i++) {
			func(i);
		}
	}

	template <typename Fn>
	void forEach3d(Fn&& func) const {
		for(std::size_t z = 0; z < gridSizeZ; z++) {
			for(std::size_t y = 0; y < gridSizeY; y++) {
				for(std::size_t x = 0; x < gridSizeX; x++) {
					func(x, y, z);
				}
			}
		}
	}
	template <typename Fn>
	void forEach3d(Fn&& func, std::size_t fromX, std::size_t toX, std::size_t fromY, std::size_t toY, std::size_t fromZ, std::size_t toZ) const {
		for(std::size_t z = fromZ; z < std::min(toZ, gridSizeZ); z++) {
			for(std::size_t y = fromY; y < std::min(toY, gridSizeY); y++) {
				for(std::size_t x = fromX; x < std::min(toX, gridSizeX); x++) {
					func(x, y, z);
				}
			}
		}
	}

private:
	std::size_t gridSizeX = 0;
	std::size_t gridSizeY = 0;
	std::size_t gridSizeZ = 0;

	std::vector<T> gridData;
};
}
