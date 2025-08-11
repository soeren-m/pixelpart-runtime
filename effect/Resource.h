#pragma once

#include <string>

namespace pixelpart {
class Resource {
public:
	Resource() = default;
	Resource(const std::string& name);

	void name(const std::string& name);
	const std::string& name() const;

private:
	std::string resourceName = "unknown";
};
}
