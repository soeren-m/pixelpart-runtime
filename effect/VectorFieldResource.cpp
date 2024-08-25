#include "VectorFieldResource.h"
#include "../common/Compression.h"
#include "../common/Json.h"
#include <cstddef>
#include <string>
#include <sstream>
#include <vector>

namespace pixelpart {
VectorFieldResource::VectorFieldResource(const std::string& resName, const Grid3d<vec3_t>& data) : Resource(resName),
	fieldData(data) {

}

const Grid3d<vec3_t>& VectorFieldResource::field() const {
	return fieldData;
}

void to_json(nlohmann::ordered_json& j, const VectorFieldResource& resource) {
	std::string dataString;
	for(std::size_t z = 0u; z < resource.field().depth(); z++) {
		for(std::size_t y = 0u; y < resource.field().height(); y++) {
			for(std::size_t x = 0u; x < resource.field().width(); x++) {
				const vec3_t& vector = resource.field()(x, y, z);

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
	std::string name;
	fromJson(name, j, "name");

	std::size_t width = 0u;
	std::size_t height = 0u;
	std::size_t depth = 0u;
	fromJson(width, j, "width");
	fromJson(height, j, "height");
	fromJson(depth, j, "depth");

	Grid3d<vec3_t> field = Grid3d<vec3_t>(width, height, depth, vec3_t(0.0));

	CompressionMethod compressionMethod = CompressionMethod::none;
	fromJson(compressionMethod, j, "compression");

	std::size_t uncompressedSize = 0u;
	fromJson(uncompressedSize, j, "uncompressed_size");

	std::vector<uint8_t> uncompressedData = decodeAndDecompress(j.at("field").get<std::string>(), uncompressedSize, compressionMethod);
	std::istringstream dataStream(
		std::string(reinterpret_cast<const char*>(uncompressedData.data()), uncompressedData.size()));

	bool finished = false;
	for(std::size_t z = 0u; z < field.depth() && !finished; z++) {
		for(std::size_t y = 0u; y < field.height() && !finished; y++) {
			for(std::size_t x = 0u; x < field.width() && !finished; x++) {
				if(dataStream.eof()) {
					finished = true;
					break;
				}

				vec3_t vector = vec3_t(0.0);
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