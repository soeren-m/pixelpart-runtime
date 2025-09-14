#pragma once

#include "Id.h"
#include "../json/json.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace pixelpart {
template <typename T>
nlohmann::ordered_json toSortedJson(const std::unordered_map<id_t, T>& objects) {
	std::vector<id_t> ids;
	ids.reserve(objects.size());
	for(const auto& entry : objects) {
		ids.push_back(entry.first);
	}

	std::sort(ids.begin(), ids.end());

	nlohmann::ordered_json j = nlohmann::ordered_json::array();
	for(id_t id : ids) {
		j.push_back(nlohmann::ordered_json::array({
			nlohmann::ordered_json(id),
			nlohmann::ordered_json(objects.at(id))
		}));
	}

	return j;
}

template <typename T>
nlohmann::ordered_json toSortedJson(const std::unordered_map<std::string, T>& objects) {
	std::vector<std::string> names;
	names.reserve(objects.size());
	for(const auto& entry : objects) {
		names.push_back(entry.first);
	}

	std::sort(names.begin(), names.end());

	nlohmann::ordered_json j = nlohmann::ordered_json::object();
	for(const std::string& name : names) {
		j[name] = objects.at(name);
	}

	return j;
}
}
