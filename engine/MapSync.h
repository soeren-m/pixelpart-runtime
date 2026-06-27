#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>

namespace pixelpart {
template<typename TKey, typename TValue, typename... TArgs>
void syncMapToKeys(std::unordered_map<TKey, TValue>& map, const std::vector<TKey>& keys, TArgs&&... defaultValueArgs) {
	std::unordered_set<TKey> keySet(keys.begin(), keys.end());

	for(typename std::unordered_map<TKey, TValue>::iterator it = map.begin(); it != map.end(); ) {
		if(keySet.find(it->first) == keySet.end()) {
			it = map.erase(it);
		}
		else {
			it++;
		}
	}

	for(const TKey& key : keys) {
		if(map.find(key) == map.end()) {
			map.emplace(std::piecewise_construct,
				std::forward_as_tuple(key),
				std::forward_as_tuple(std::forward<TArgs>(defaultValueArgs)...));
		}
	}
}
}
