#include "ParticleSorting.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "../glm/gtx/norm.hpp"
#include <numeric>
#include <functional>
#include <utility>
#include <vector>
#include <algorithm>

namespace pixelpart {
template <typename It, typename Comp = std::less<typename std::iterator_traits<It>::value_type>>
void insertionSort(It begin, It end, Comp comp = Comp()) {
	std::iter_swap(begin, std::min_element(begin, end, comp));

	for(It i = begin; ++i < end; begin = i) {
		for(It j = i; comp(*j, *begin); --j, --begin) {
			std::iter_swap(begin, j);
		}
	}
}

#if defined(PIXELPART_RUNTIME_MULTITHREADING) || defined(__INTELLISENSE__)
template<typename It, typename Comp>
std::vector<typename It::value_type> merge(It begin, It mid, It end, Comp comp) {
	std::vector<typename It::value_type> result;
	result.reserve(std::distance(begin, end));

	It left = begin;
	It right = mid;

	while(left != mid && right != end) {
		result.push_back(comp(*left, *right)
			? *left++
			: *right++);
	}

	result.insert(result.end(), left, mid);
	result.insert(result.end(), right, end);

	return result;
}

template <typename It, typename Comp = std::less<typename std::iterator_traits<It>::value_type>>
void parallelInsertionSort(ThreadPool& threadPool, std::size_t minWorkSize, It begin, It end, Comp comp = Comp()) {
	std::size_t length = std::distance(begin, end);
	if(length < 2) {
		return;
	}

	std::size_t threadCount = std::min(length / std::max(minWorkSize, static_cast<std::size_t>(1)), threadPool.threadCount());
	if(threadCount <= 1) {
		insertionSort(begin, end, comp);
		return;
	}

	std::vector<std::pair<std::size_t, std::size_t>> workgroups;
	std::size_t threadCountWithLargerLoad = length % threadCount;
	std::size_t workSize = length / threadCount;
	std::size_t workgroupStart = 0;

	for(std::size_t t = 0; t < threadCount; t++) {
		std::size_t workgroupSize = (t < threadCount - threadCountWithLargerLoad) ? workSize : workSize + 1;
		std::size_t workgroupEnd = workgroupStart + workgroupSize;

		threadPool.enqueue(static_cast<std::uint32_t>(t),
			insertionSort<It, Comp>,
			std::next(begin, workgroupStart),
			std::next(begin, workgroupEnd),
			comp);

		workgroups.emplace_back(std::make_pair(workgroupStart, workgroupEnd));
		workgroupStart += workgroupSize;
	}

	for(std::size_t t = 0; t < threadCount; t++) {
		threadPool.wait(static_cast<std::uint32_t>(t));
	}

	for(std::size_t t = 1; t < threadCount; t++) {
		auto&& result = merge(
			begin,
			std::next(begin, workgroups[t].first),
			std::next(begin, workgroups[t].second),
			comp);

		std::move(result.cbegin(), result.cend(), begin);
	}
}
#endif

void applySortKeys(ParticleCollection& resultCollection,
	const ParticleCollection::ReadPtr& particles, std::uint32_t particleCount,
	const std::vector<std::uint32_t>& sortKeys) {
	ParticleCollection::WritePtr resultParticleData = resultCollection.writePtr();

	for(std::uint32_t index = 0; index < particleCount; index++) {
		std::uint32_t otherIndex = sortKeys[index];

		resultParticleData.id[index] = particles.id[otherIndex];
		resultParticleData.parentId[index] = particles.parentId[otherIndex];
		resultParticleData.life[index] = particles.life[otherIndex];
		resultParticleData.globalPosition[index] = particles.globalPosition[otherIndex];
		resultParticleData.velocity[index] = particles.velocity[otherIndex];
		resultParticleData.force[index] = particles.force[otherIndex];
		resultParticleData.rotation[index] = particles.rotation[otherIndex];
		resultParticleData.size[index] = particles.size[otherIndex];
		resultParticleData.color[index] = particles.color[otherIndex];
	}
}

void sortParticles(ParticleCollection& resultCollection,
	const ParticleCollection::ReadPtr& particles, std::uint32_t particleCount,
	const SceneContext& sceneContext,
	ParticleSortCriterion sortCriterion) {
	if(particleCount == 0) {
		return;
	}

	std::vector<std::uint32_t> sortKeys(particleCount);
	std::iota(sortKeys.begin(), sortKeys.begin() + particleCount, 0);

	if(particleCount > resultCollection.capacity()) {
		resultCollection = ParticleCollection(particleCount);
	}

	switch(sortCriterion) {
		case ParticleSortCriterion::age:
			insertionSort(sortKeys.begin(), sortKeys.begin() + particleCount,
				[particles](std::uint32_t i, std::uint32_t j) {
					return particles.id[i] < particles.id[j];
				});
			break;
		case ParticleSortCriterion::distance:
			insertionSort(sortKeys.begin(), sortKeys.begin() + particleCount,
				[particles, sceneContext](std::uint32_t i, std::uint32_t j) {
					return glm::distance2(particles.globalPosition[i], sceneContext.cameraPosition) >
						glm::distance2(particles.globalPosition[j], sceneContext.cameraPosition);
				});
			break;
		default:
			break;
	}

	applySortKeys(resultCollection, particles, particleCount, sortKeys);
}

#if defined(PIXELPART_RUNTIME_MULTITHREADING) || defined(__INTELLISENSE__)
void sortParticles(ParticleCollection& resultCollection,
	const ParticleCollection::ReadPtr& particles, std::uint32_t particleCount,
	const SceneContext& sceneContext,
	ParticleSortCriterion sortCriterion, ThreadPool& threadPool) {
	std::vector<std::uint32_t> sortKeys(particleCount);
	std::iota(sortKeys.begin(), sortKeys.begin() + particleCount, 0);

	if(particleCount > resultCollection.capacity()) {
		resultCollection = ParticleCollection(particleCount);
	}

	switch(sortCriterion) {
		case ParticleSortCriterion::age:
			parallelInsertionSort(threadPool, 64, sortKeys.begin(), sortKeys.begin() + particleCount,
				[particles](std::uint32_t i, std::uint32_t j) {
					return particles.id[i] < particles.id[j];
				});
			break;
		case ParticleSortCriterion::distance:
			parallelInsertionSort(threadPool, 64, sortKeys.begin(), sortKeys.begin() + particleCount,
				[particles, sceneContext](std::uint32_t i, std::uint32_t j) {
					return glm::distance2(particles.globalPosition[i], sceneContext.cameraPosition) >
						glm::distance2(particles.globalPosition[j], sceneContext.cameraPosition);
				});
			break;
		default:
			break;
	}

	applySortKeys(resultCollection, particles, particleCount, sortKeys);
}
#endif
}
