#pragma once

#include <cstddef>
#include <vector>
#include <algorithm>

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
	Grid(std::size_t w, std::size_t h) : sizeX(w), sizeY(h) {
		data.resize(sizeX * sizeY);
	}
	Grid(std::size_t w, std::size_t h, const T& value) : sizeX(w), sizeY(h) {
		data.resize(sizeX * sizeY, value);
	}

	T& operator()(std::size_t i) {
		return data[i];
	}
	template <typename IntT>
	T& operator()(IntT x, IntT y) {
		return data[
			static_cast<std::size_t>(y) * sizeX +
			static_cast<std::size_t>(x)];
	}
	template <typename IntT>
	T& operator()(const GridIndex<IntT>& index) {
		return data[
			static_cast<std::size_t>(index.y) * sizeX +
			static_cast<std::size_t>(index.x)];
	}

	const T& operator()(std::size_t i) const {
		return data[i];
	}
	template <typename IntT>
	const T& operator()(IntT x, IntT y) const {
		return data[
			static_cast<std::size_t>(y) * sizeX +
			static_cast<std::size_t>(x)];
	}
	template <typename IntT>
	const T& operator()(const GridIndex<IntT>& index) const {
		return data[
			static_cast<std::size_t>(index.y) * sizeX +
			static_cast<std::size_t>(index.x)];
	}

	template <typename IntT>
	T& value(IntT x, IntT y, const T& defaultValue) {
		if(contains(x, y)) {
			return operator()<IntT>(x, y);
		}

		return defaultValue;
	}
	template <typename IntT>
	T& value(const GridIndex<IntT>& index, const T& defaultValue) {
		if(contains(index.x, index.y)) {
			return operator()<IntT>(index);
		}

		return defaultValue;
	}
	template <typename IntT>
	const T& value(IntT x, IntT y, const T& defaultValue) const {
		if(contains(x, y)) {
			return operator()<IntT>(x, y);
		}

		return defaultValue;
	}
	template <typename IntT>
	const T& value(const GridIndex<IntT>& index, const T& defaultValue) const {
		if(contains(index.x, index.y)) {
			return operator()<IntT>(index);
		}

		return defaultValue;
	}

	void assign(const T& value) {
		data.assign(data.size(), value);
	}
	void resize(std::size_t w, std::size_t h) {
		sizeX = w;
		sizeY = h;
		data.resize(sizeX * sizeY);
	}
	void clear() {
		for(T& cell : data) {
			cell = T();
		}
	}

	template <typename IntT>
	bool contains(IntT x, IntT y) const {
		return
			x >= 0 && static_cast<std::size_t>(x) < sizeX &&
			y >= 0 && static_cast<std::size_t>(y) < sizeY;
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
	std::size_t size() const {
		return sizeX * sizeY;
	}

	template <typename Fn>
	void forEach1d(Fn&& func) const {
		for(std::size_t i = 0; i < sizeX * sizeY; i++) {
			func(i);
		}
	}
	template <typename Fn>
	void forEach1d(Fn&& func, std::size_t from, std::size_t to) const {
		for(std::size_t i = from; i < std::min(to, sizeX * sizeY); i++) {
			func(i);
		}
	}

	template <typename Fn>
	void forEach2d(Fn&& func) const {
		for(std::size_t y = 0; y < sizeY; y++) {
			for(std::size_t x = 0; x < sizeX; x++) {
				func(x, y);
			}
		}
	}
	template <typename Fn>
	void forEach2d(Fn&& func, std::size_t fromX, std::size_t toX, std::size_t fromY, std::size_t toY) const {
		for(std::size_t y = fromY; y < std::min(toY, sizeY); y++) {
			for(std::size_t x = fromX; x < std::min(toX, sizeX); x++) {
				func(x, y);
			}
		}
	}

private:
	std::size_t sizeX = 0;
	std::size_t sizeY = 0;

	std::vector<T> data;
};
}
