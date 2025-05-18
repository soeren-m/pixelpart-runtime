#include "Id.h"

namespace pixelpart {
const std::uint32_t id_t::nullValue = 0xFFFFFFFF;

id_t::id_t(std::uint32_t id) : idValue(id) {

}

id_t::operator std::uint32_t() const {
	return idValue;
}
id_t::operator bool() const {
	return valid();
}

id_t id_t::operator++(int) {
	id_t tmp = *this;
	if(valid()) {
		idValue++;
	}

	return tmp;
}
id_t id_t::operator--(int) {
	id_t tmp = *this;
	if(valid()) {
		idValue--;
	}

	return tmp;
}

std::uint32_t id_t::value() const {
	return idValue;
}
bool id_t::valid() const {
	return idValue != nullValue;
}

bool operator==(const id_t& lhs, const id_t& rhs) {
	return lhs.value() == rhs.value();
}
bool operator!=(const id_t& lhs, const id_t& rhs) {
	return lhs.value() != rhs.value();
}
bool operator<(const id_t& lhs, const id_t& rhs) {
	return lhs.value() < rhs.value();
}
bool operator>(const id_t& lhs, const id_t& rhs) {
	return lhs.value() > rhs.value();
}
bool operator<=(const id_t& lhs, const id_t& rhs) {
	return lhs.value() <= rhs.value();
}
bool operator>=(const id_t& lhs, const id_t& rhs) {
	return lhs.value() >= rhs.value();
}

void to_json(nlohmann::ordered_json& j, const id_t& id) {
	j = id.valid()
		? nlohmann::ordered_json(static_cast<std::uint32_t>(id))
		: nlohmann::ordered_json(nullptr);
}
void from_json(const nlohmann::ordered_json& j, id_t& id) {
	id = j.is_number_unsigned()
		? id_t(j.get<std::uint32_t>())
		: id_t();
}
}