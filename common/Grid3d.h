#pragma once

#include <cstddef>
#include <vector>
#include <algorithm>

namespace pixelpart {
template <typename IntT = std::size_t>
struct GridIndex3d {
	IntT x;
	IntT y;
	IntT z;
};

template <typename IntT>
bool operator==(const GridIndex3d<IntT>& i1, const GridIndex3d<IntT>& i2) {
	return i1.x == i2.x &&
		i1.y == i2.y &&
		i1.z == i2.z;
}

template <typename T>
class Grid3d {
public:
	Grid3d() {

	}
	Grid3d(std::size_t w, std::size_t h, std::size_t d) : sizeX(w), sizeY(h), sizeZ(d) {
		data.resize(w * h * d);
	}
	Grid3d(std::size_t w, std::size_t h, std::size_t d, const T& value) : sizeX(w), sizeY(h), sizeZ(d) {
		data.resize(w * h * d, value);
	}

	T& operator()(std::size_t i) {
		return data[i];
	}
	template <typename IntT>
	T& operator()(IntT x, IntT y, IntT z) {
		return data[
			static_cast<std::size_t>(z) * sizeY * sizeX +
			static_cast<std::size_t>(y) * sizeX +
			static_cast<std::size_t>(x)];
	}
	template <typename IntT>
	T& operator()(const GridIndex3d<IntT>& index) {
		return data[
			static_cast<std::size_t>(index.z) * sizeY * sizeX +
			static_cast<std::size_t>(index.y) * sizeX +
			static_cast<std::size_t>(index.x)];
	}

	const T& operator()(std::size_t i) const {
		return data[i];
	}
	template <typename IntT>
	const T& operator()(IntT x, IntT y, IntT z) const {
		return data[
			static_cast<std::size_t>(z) * sizeY * sizeX +
			static_cast<std::size_t>(y) * sizeX +
			static_cast<std::size_t>(x)];
	}
	template <typename IntT>
	const T& operator()(const GridIndex3d<IntT>& index) const {
		return data[
			static_cast<std::size_t>(index.z) * sizeY * sizeX +
			static_cast<std::size_t>(index.y) * sizeX +
			static_cast<std::size_t>(index.x)];
	}

	template <typename IntT>
	T& value(IntT x, IntT y, IntT z, const T& defaultValue) {
		if(contains(x, y, z)) {
			return operator()<IntT>(x, y, z);
		}

		return defaultValue;
	}
	template <typename IntT>
	T& value(const GridIndex3d<IntT>& index, const T& defaultValue) {
		if(contains(index.x, index.y, index.z)) {
			return operator()<IntT>(index);
		}

		return defaultValue;
	}
	template <typename IntT>
	const T& value(IntT x, IntT y, IntT z, const T& defaultValue) const {
		if(contains(x, y, z)) {
			return operator()<IntT>(x, y, z);
		}

		return defaultValue;
	}
	template <typename IntT>
	const T& value(const GridIndex3d<IntT>& index, const T& defaultValue) const {
		if(contains(index.x, index.y, index.z)) {
			return operator()<IntT>(index);
		}

		return defaultValue;
	}

	void assign(const T& value) {
		data.assign(data.size(), value);
	}
	void resize(std::size_t w, std::size_t h, std::size_t d) {
		sizeX = w;
		sizeY = h;
		sizeZ = d;
		data.resize(sizeX * sizeY * sizeZ);
	}
	void clear() {
		for(T& cell : data) {
			cell = T();
		}
	}

	template <typename IntT>
	bool contains(IntT x, IntT y, IntT z) const {
		return
			x >= 0 && static_cast<std::size_t>(x) < sizeX &&
			y >= 0 && static_cast<std::size_t>(y) < sizeY &&
			z >= 0 && static_cast<std::size_t>(z) < sizeZ;
	}

	const std::vector<T>& cells() const {
		return data;
	}
	std::size_t width() const {
		return sizeX;
	}
	std::size_t height() const {
		return sizeY;
	}
	std::size_t depth() const {
		return sizeZ;
	}
	std::size_t size() const {
		return sizeX * sizeY * sizeZ;
	}

	template <typename Fn>
	void forEach1d(Fn&& func) const {
		for(std::size_t i = 0; i < sizeX * sizeY * sizeZ; i++) {
			func(i);
		}
	}
	template <typename Fn>
	void forEach1d(Fn&& func, std::size_t from, std::size_t to) const {
		for(std::size_t i = from; i < std::min(to, sizeX * sizeY * sizeZ); i++) {
			func(i);
		}
	}

	template <typename Fn>
	void forEach3d(Fn&& func) const {
		for(std::size_t z = 0; z < sizeZ; z++) {
			for(std::size_t y = 0; y < sizeY; y++) {
				for(std::size_t x = 0; x < sizeX; x++) {
					func(x, y, z);
				}
			}
		}
	}
	template <typename Fn>
	void forEach3d(Fn&& func, std::size_t fromX, std::size_t toX, std::size_t fromY, std::size_t toY, std::size_t fromZ, std::size_t toZ) const {
		for(std::size_t z = fromZ; z < std::min(toZ, sizeZ); z++) {
			for(std::size_t y = fromY; y < std::min(toY, sizeY); y++) {
				for(std::size_t x = fromX; x < std::min(toX, sizeX); x++) {
					func(x, y, z);
				}
			}
		}
	}

private:
	std::size_t sizeX = 0;
	std::size_t sizeY = 0;
	std::size_t sizeZ = 0;

	std::vector<T> data;
};
}