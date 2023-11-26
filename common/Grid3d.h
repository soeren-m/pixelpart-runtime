#pragma once

#include <vector>

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
	Grid3d(std::size_t w, std::size_t h, std::size_t d) : width(w), height(h), depth(d) {
		cells.resize(w * h * d);
	}
	Grid3d(std::size_t w, std::size_t h, std::size_t d, const T& value) : width(w), height(h), depth(d) {
		cells.resize(w * h * d, value);
	}

	T& operator()(std::size_t i) {
		return cells[i];
	}
	template <typename IntT>
	T& operator()(IntT x, IntT y, IntT z) {
		return cells[
			static_cast<std::size_t>(z) * height * width +
			static_cast<std::size_t>(y) * width +
			static_cast<std::size_t>(x)];
	}
	template <typename IntT>
	T& operator()(const GridIndex3d<IntT>& index) {
		return cells[
			static_cast<std::size_t>(index.z) * height * width +
			static_cast<std::size_t>(index.y) * width +
			static_cast<std::size_t>(index.x)];
	}

	const T& operator()(std::size_t i) const {
		return cells[i];
	}
	template <typename IntT>
	const T& operator()(IntT x, IntT y, IntT z) const {
		return cells[
			static_cast<std::size_t>(z) * height * width +
			static_cast<std::size_t>(y) * width +
			static_cast<std::size_t>(x)];
	}
	template <typename IntT>
	const T& operator()(const GridIndex3d<IntT>& index) const {
		return cells[
			static_cast<std::size_t>(index.z) * height * width +
			static_cast<std::size_t>(index.y) * width +
			static_cast<std::size_t>(index.x)];
	}

	template <typename IntT>
	T& getOrDefault(IntT x, IntT y, IntT z, const T& defaultValue) {
		if(contains(x, y, z)) {
			return operator()<IntT>(x, y, z);
		}

		return defaultValue;
	}
	template <typename IntT>
	T& getOrDefault(const GridIndex3d<IntT>& index, const T& defaultValue) {
		if(contains(index.x, index.y, index.z)) {
			return operator()<IntT>(index);
		}

		return defaultValue;
	}
	template <typename IntT>
	const T& getOrDefault(IntT x, IntT y, IntT z, const T& defaultValue) const {
		if(contains(x, y, z)) {
			return operator()<IntT>(x, y, z);
		}

		return defaultValue;
	}
	template <typename IntT>
	const T& getOrDefault(const GridIndex3d<IntT>& index, const T& defaultValue) const {
		if(contains(index.x, index.y, index.z)) {
			return operator()<IntT>(index);
		}

		return defaultValue;
	}

	void resize(std::size_t w, std::size_t h, std::size_t d) {
		width = w;
		height = h;
		depth = d;
		cells.resize(width * height * depth);
	}
	void clear() {
		for(T& cell : cells) {
			cell = T();
		}
	}
	void assign(const T& value) {
		cells.assign(cells.size(), value);
	}

	template <typename IntT>
	bool contains(IntT x, IntT y, IntT z) const {
		return
			x >= 0 && static_cast<std::size_t>(x) < width &&
			y >= 0 && static_cast<std::size_t>(y) < height &&
			z >= 0 && static_cast<std::size_t>(z) < depth;
	}

	const std::vector<T>& getCells() const {
		return cells;
	}
	std::size_t getWidth() const {
		return width;
	}
	std::size_t getHeight() const {
		return height;
	}
	std::size_t getDepth() const {
		return depth;
	}
	std::size_t getSize() const {
		return width * height * depth;
	}

	template <typename Fn>
	void loop1d(Fn&& func) const {
		for(std::size_t i = 0u; i < width * height * depth; i++) {
			func(i);
		}
	}
	template <typename Fn>
	void loop1d(Fn&& func, std::size_t from, std::size_t to) const {
		for(std::size_t i = from; i < std::min(to, width * height * depth); i++) {
			func(i);
		}
	}

	template <typename Fn>
	void loop3d(Fn&& func) const {
		for(std::size_t z = 0u; z < depth; z++) {
			for(std::size_t y = 0u; y < height; y++) {
				for(std::size_t x = 0u; x < width; x++) {
					func(x, y, z);
				}
			}
		}
	}
	template <typename Fn>
	void loop3d(Fn&& func, std::size_t fromX, std::size_t toX, std::size_t fromY, std::size_t toY, std::size_t fromZ, std::size_t toZ) const {
		for(std::size_t z = fromZ; z < std::min(toZ, depth); z++) {
			for(std::size_t y = fromY; y < std::min(toY, height); y++) {
				for(std::size_t x = fromX; x < std::min(toX, width); x++) {
					func(x, y, z);
				}
			}
		}
	}

private:
	std::size_t width = 0u;
	std::size_t height = 0u;
	std::size_t depth = 0u;

	std::vector<T> cells;
};
}