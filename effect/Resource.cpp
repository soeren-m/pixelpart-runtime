#include "Resource.h"

namespace pixelpart {
Resource::Resource(const std::string& name) : resourceName(name) {

}

void Resource::name(const std::string& name) {
	resourceName = name;
}
const std::string& Resource::name() const {
	return resourceName;
}
}