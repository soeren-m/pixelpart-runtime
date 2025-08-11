#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class CompressionMethod : std::uint32_t {
	none = 0,
	zlib = 1
};

NLOHMANN_JSON_SERIALIZE_ENUM(CompressionMethod, {
	{ CompressionMethod::none, "none" },
	{ CompressionMethod::zlib, "zlib" }
})
}
