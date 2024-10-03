#pragma once

#include <cstdint>
#include "../json/json.hpp"

namespace pixelpart {
class id_t {
public:
	static const uint32_t nullValue;

	id_t() = default;
	id_t(uint32_t id);

	operator uint32_t() const;
	operator bool() const;

	id_t operator++(int);
	id_t operator--(int);

	uint32_t value() const;
	bool valid() const;

	friend bool operator==(const id_t& lhs, const id_t& rhs);
	friend bool operator!=(const id_t& lhs, const id_t& rhs);
	friend bool operator<(const id_t& lhs, const id_t& rhs);
	friend bool operator>(const id_t& lhs, const id_t& rhs);
	friend bool operator<=(const id_t& lhs, const id_t& rhs);
	friend bool operator>=(const id_t& lhs, const id_t& rhs);

private:
	uint32_t idValue = 0xFFFFFFFFu;
};

void to_json(nlohmann::ordered_json& j, const id_t& id);
void from_json(const nlohmann::ordered_json& j, id_t& id);
}

template <>
struct std::hash<pixelpart::id_t> {
	std::size_t operator()(const pixelpart::id_t& id) const;
};