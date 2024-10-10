#include "VectorFieldResource.h"
#include "../common/Compression.h"
#include <cstddef>
#include <string>
#include <sstream>
#include <vector>

namespace pixelpart {
VectorFieldResource::VectorFieldResource(const std::string& name, const Grid3d<float3_t>& field) : Resource(name),
	fieldData(field) {

}

Grid3d<float3_t>& VectorFieldResource::field() {
	return fieldData;
}
const Grid3d<float3_t>& VectorFieldResource::field() const {
	return fieldData;
}

void to_json(nlohmann::ordered_json& j, const VectorFieldResource& resource) {
	std::string dataString;
	for(std::size_t z = 0u; z < resource.field().depth(); z++) {
		for(std::size_t y = 0u; y < resource.field().height(); y++) {
			for(std::size_t x = 0u; x < resource.field().width(); x++) {
				const float3_t& vector = resource.field()(x, y, z);

				dataString += std::to_string(vector.x);
				dataString += ' ';
				dataString += std::to_string(vector.y);
				dataString += ' ';
				dataString += std::to_string(vector.z);
				dataString += ' ';
			}
		}
	}

	std::string compressedData = compressAndEncode(reinterpret_cast<const uint8_t*>(dataString.data()), dataString.size(), CompressionMethod::zlib);

	j = nlohmann::ordered_json{
		{ "name", resource.name() },
		{ "width", resource.field().width() },
		{ "height", resource.field().height() },
		{ "depth", resource.field().depth() },
		{ "compression", CompressionMethod::zlib },
		{ "uncompressed_size", dataString.size() },
		{ "field", compressedData }
	};
}
void from_json(const nlohmann::ordered_json& j, VectorFieldResource& resource) {
	std::string name = j.value("name", "unknown");
	std::size_t width = j.value("width", 0u);
	std::size_t height = j.value("height", 0u);
	std::size_t depth = j.value("depth", 0u);

	CompressionMethod compressionMethod = j.value("compression", CompressionMethod::none);
	std::size_t uncompressedSize = j.value("uncompressed_size", 0u);

	std::vector<uint8_t> uncompressedData = decodeAndDecompress(j.at("field").get<std::string>(), uncompressedSize, compressionMethod);
	std::istringstream dataStream(
		std::string(reinterpret_cast<const char*>(uncompressedData.data()), uncompressedData.size()));

	Grid3d<float3_t> field = Grid3d<float3_t>(width, height, depth, float3_t(0.0));

	bool finished = false;
	for(std::size_t z = 0u; z < field.depth() && !finished; z++) {
		for(std::size_t y = 0u; y < field.height() && !finished; y++) {
			for(std::size_t x = 0u; x < field.width() && !finished; x++) {
				if(dataStream.eof()) {
					finished = true;
					break;
				}

				float3_t vector = float3_t(0.0);
				std::string token;

				std::getline(dataStream, token, ' ');
				vector.x = std::stod(token);

				std::getline(dataStream, token, ' ');
				vector.y = std::stod(token);

				std::getline(dataStream, token, ' ');
				vector.z = std::stod(token);

				field(x, y, z) = vector;
			}
		}
	}

	resource = VectorFieldResource(name, field);
}
}