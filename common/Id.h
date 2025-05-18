#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
class id_t {
public:
	static const std::uint32_t nullValue;

	id_t() = default;
	id_t(std::uint32_t id);

	explicit operator std::uint32_t() const;
	explicit operator bool() const;

	id_t operator++(int);
	id_t operator--(int);

	std::uint32_t value() const;
	bool valid() const;

	friend bool operator==(const id_t& lhs, const id_t& rhs);
	friend bool operator!=(const id_t& lhs, const id_t& rhs);
	friend bool operator<(const id_t& lhs, const id_t& rhs);
	friend bool operator>(const id_t& lhs, const id_t& rhs);
	friend bool operator<=(const id_t& lhs, const id_t& rhs);
	friend bool operator>=(const id_t& lhs, const id_t& rhs);

private:
	std::uint32_t idValue = nullValue;
};

void to_json(nlohmann::ordered_json& j, const id_t& id);
void from_json(const nlohmann::ordered_json& j, id_t& id);
}

template <>
struct std::hash<pixelpart::id_t> {
	std::size_t operator()(const pixelpart::id_t& id) const {
		return std::hash<std::uint32_t>()(id.value());
	}
};