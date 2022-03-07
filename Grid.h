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
	Grid() : width(0), height(0) {

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
	T& operator()(std::size_t x, std::size_t y) {
		return cells[y * width + x];
	}

	template <typename IntT>
	T& operator()(const GridIndex<IntT>& index) {
		return cells[static_cast<std::size_t>(index.y) * width + static_cast<std::size_t>(index.x)];
	}

	const T& operator()(std::size_t i) const {
		return cells[i];
	}
	const T& operator()(std::size_t x, std::size_t y) const {
		return cells[y * width + x];
	}

	template <typename IntT>
	const T& operator()(const GridIndex<IntT>& index) const {
		return cells[static_cast<std::size_t>(index.y) * width + static_cast<std::size_t>(index.x)];
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
		return x >= 0 &&
			y >= 0 &&
			static_cast<std::size_t>(x) < width &&
			static_cast<std::size_t>(y) < height;
	}
	
	std::vector<T>& getCells() {
		return cells;
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
		for(std::size_t i = 0; i < width * height; i++) {
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
		for(std::size_t y = 0; y < height; y++) {
			for(std::size_t x = 0; x < width; x++) {
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
	std::vector<T> cells;
	std::size_t width;
	std::size_t height;
};
}