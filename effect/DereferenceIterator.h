#pragma once

#include <cstddef>
#include <iterator>

namespace pixelpart {
template <typename T>
class DereferenceIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = typename T::value_type::element_type;
	using pointer = value_type*;
	using reference = value_type&;

	explicit DereferenceIterator(T baseIt) : it(baseIt) {

	}

	reference operator*() const {
		return **it;
	}
	pointer operator->() const {
		return it->get();
	}

	DereferenceIterator& operator++() {
		++it;

		return *this;
	}
	DereferenceIterator operator++(int) {
		auto tmp = *this;
		++(*this);

		return tmp;
	}

	bool operator==(const DereferenceIterator& other) const {
		return it == other.it;
	}
	bool operator!=(const DereferenceIterator& other) const {
		return !(*this == other);
	}

private:
	T it;
};
}
