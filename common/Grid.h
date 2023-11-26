#pragma once

#include <vector>

namespace pixelpart {
template <typename IntT = std::size_t>
struct GridIndex {
	IntT x;
	IntT y;
};

template <typename IntT>
bool operator==(const GridIndex<IntT>& i1, const GridIndex<IntT>& i2) {
	return i1.x == i2.x && i1.y == i2.y;
}

template <typename T>
class Grid {
public:
	Grid() {

	}
	Grid(std::size_t w, std::size_t h) : width(w), height(h) {
		cells.resize(width * height);
	}
	Grid(std::size_t w, std::size_t h, const T& value) : width(w), height(h) {
		cells.resize(width * height, value);
	}

	T& operator()(std::size_t i) {
		return cells[i];
	}
	template <typename IntT>
	T& operator()(IntT x, IntT y) {
		return cells[
			static_cast<std::size_t>(y) * width +
			static_cast<std::size_t>(x)];
	}
	template <typename IntT>
	T& operator()(const GridIndex<IntT>& index) {
		return cells[
			static_cast<std::size_t>(index.y) * width +
			static_cast<std::size_t>(index.x)];
	}

	const T& operator()(std::size_t i) const {
		return cells[i];
	}
	template <typename IntT>
	const T& operator()(IntT x, IntT y) const {
		return cells[
			static_cast<std::size_t>(y) * width +
			static_cast<std::size_t>(x)];
	}
	template <typename IntT>
	const T& operator()(const GridIndex<IntT>& index) const {
		return cells[
			static_cast<std::size_t>(index.y) * width +
			static_cast<std::size_t>(index.x)];
	}

	template <typename IntT>
	T& getOrDefault(IntT x, IntT y, const T& defaultValue) {
		if(contains(x, y)) {
			return operator()<IntT>(x, y);
		}

		return defaultValue;
	}
	template <typename IntT>
	T& getOrDefault(const GridIndex<IntT>& index, const T& defaultValue) {
		if(contains(index.x, index.y)) {
			return operator()<IntT>(index);
		}

		return defaultValue;
	}
	template <typename IntT>
	const T& getOrDefault(IntT x, IntT y, const T& defaultValue) const {
		if(contains(x, y)) {
			return operator()<IntT>(x, y);
		}

		return defaultValue;
	}
	template <typename IntT>
	const T& getOrDefault(const GridIndex3d<IntT>& index, const T& defaultValue) const {
		if(contains(index.x, index.y)) {
			return operator()<IntT>(index);
		}

		return defaultValue;
	}

	void resize(std::size_t w, std::size_t h) {
		width = w;
		height = h;
		cells.resize(width * height);
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
	bool contains(IntT x, IntT y) const {
		return
			x >= 0 && static_cast<std::size_t>(x) < width &&
			y >= 0 && static_cast<std::size_t>(y) < height;
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
	std::size_t getSize() const {
		return width * height;
	}

	template <typename Fn>
	void loop1d(Fn&& func) const {
		for(std::size_t i = 0u; i < width * height; i++) {
			func(i);
		}
	}
	template <typename Fn>
	void loop1d(Fn&& func, std::size_t from, std::size_t to) const {
		for(std::size_t i = from; i < std::min(to, width * height); i++) {
			func(i);
		}
	}

	template <typename Fn>
	void loop2d(Fn&& func) const {
		for(std::size_t y = 0u; y < height; y++) {
			for(std::size_t x = 0u; x < width; x++) {
				func(x, y);
			}
		}
	}
	template <typename Fn>
	void loop2d(Fn&& func, std::size_t fromX, std::size_t toX, std::size_t fromY, std::size_t toY) const {
		for(std::size_t y = fromY; y < std::min(toY, height); y++) {
			for(std::size_t x = fromX; x < std::min(toX, width); x++) {
				func(x, y);
			}
		}
	}

private:
	std::size_t width = 0u;
	std::size_t height = 0u;

	std::vector<T> cells;
};
}