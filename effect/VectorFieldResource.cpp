#include "VectorFieldResource.h"
#include "../common/Compression.h"
#include "../common/Json.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const VectorFieldResource& resource) {
	std::string dataString;
	for(std::size_t z = 0u; z < resource.field.getDepth(); z++) {
		for(std::size_t y = 0u; y < resource.field.getHeight(); y++) {
			for(std::size_t x = 0u; x < resource.field.getWidth(); x++) {
				const vec3_t& vector = resource.field(x, y, z);

				dataString += std::to_string(vector.x);
				dataString += ' ';
				dataString += std::to_string(vector.y);
				dataString += ' ';
				dataString += std::to_string(vector.z);
				dataString += ' ';
			}
		}
	}

	std::string compressedData = compressAndEncodeString(dataString, CompressionMethod::zlib);

	j = nlohmann::ordered_json{
		{ "name", resource.name },
		{ "width", resource.field.getWidth() },
		{ "height", resource.field.getHeight() },
		{ "depth", resource.field.getDepth() },
		{ "compression", CompressionMethod::zlib },
		{ "uncompressed_size", dataString.size() },
		{ "field", compressedData }
	};
}
void from_json(const nlohmann::ordered_json& j, VectorFieldResource& resource) {
	resource = VectorFieldResource();

	fromJson(resource.name, j, "name");

	std::size_t width = 0u;
	std::size_t height = 0u;
	std::size_t depth = 0u;
	fromJson(width, j, "width");
	fromJson(height, j, "height");
	fromJson(depth, j, "depth");

	resource.field = Grid3d<vec3_t>(width, height, depth, vec3_t(0.0));

	CompressionMethod compressionMethod = CompressionMethod::none;
	fromJson(compressionMethod, j, "compression");

	std::size_t uncompressedSize = 0u;
	fromJson(uncompressedSize, j, "uncompressed_size");

	std::istringstream dataStream(
		decodeAndDecompressToString(j.at("field").get<std::string>(), uncompressedSize, compressionMethod));

	bool finished = false;
	for(std::size_t z = 0u; z < resource.field.getDepth() && !finished; z++) {
		for(std::size_t y = 0u; y < resource.field.getHeight() && !finished; y++) {
			for(std::size_t x = 0u; x < resource.field.getWidth() && !finished; x++) {
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

				resource.field(x, y, z) = vector;
			}
		}
	}
}
}