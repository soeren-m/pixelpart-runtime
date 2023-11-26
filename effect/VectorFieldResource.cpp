#include "VectorFieldResource.h"
#include "../common/JsonUtil.h"
#include "../common/Base64.h"
#include "../zlib/zlib.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const VectorFieldResource& resource) {
	std::string dataString;
	for(std::size_t z = 0u; z < resource.field.getDepth(); z++) {
		for(std::size_t y = 0u; y < resource.field.getHeight(); y++) {
			for(std::size_t x = 0u; x < resource.field.getWidth(); x++) {
				const vec3d& vector = resource.field(x, y, z);

				dataString += std::to_string(vector.x);
				dataString += ' ';
				dataString += std::to_string(vector.y);
				dataString += ' ';
				dataString += std::to_string(vector.z);
				dataString += ' ';
			}
		}
	}

	uLong compressedSize = compressBound(static_cast<uLong>(dataString.size()));
	std::vector<Byte> compressedData(compressedSize);

	int result = compress(
		compressedData.data(), &compressedSize,
		reinterpret_cast<const Bytef*>(&dataString[0]), static_cast<uLong>(dataString.size()));

	if(result != Z_OK) {
		throw std::runtime_error("Compression error, zlib returned " + std::to_string(result));
	}

	j = nlohmann::ordered_json{
		{ "name", resource.name },
		{ "width", resource.field.getWidth() },
		{ "height", resource.field.getHeight() },
		{ "depth", resource.field.getDepth() },
		{ "compression", "zlib" },
		{ "uncompressed_size", dataString.size() },
		{ "field", encodeBase64(static_cast<const unsigned char*>(compressedData.data()), static_cast<std::size_t>(compressedSize)) }
	};
}
void from_json(const nlohmann::ordered_json& j, VectorFieldResource& resource) {
	fromJson(resource.name, j, "name");

	std::size_t width = j.at("width");
	std::size_t height = j.at("height");
	std::size_t depth = j.at("depth");
	resource.field = Grid3d<vec3d>(width, height, depth, vec3d(0.0));

	std::string compression;
	fromJson(compression, j, "compression");

	std::string compressedData = decodeBase64(j.at("field").get<std::string>());
	if(compressedData.empty()) {
		throw std::runtime_error("Decompression error, compressed data is empty");
	}

	std::string stringData;

	if(compression == "zlib") {
		uLong uncompressedSize = j.at("uncompressed_size");
		stringData.resize(uncompressedSize);

		int result = uncompress(
			reinterpret_cast<Bytef*>(stringData.data()), &uncompressedSize,
			reinterpret_cast<const Bytef*>(compressedData.data()), static_cast<uLong>(compressedData.size()));

		if(result != Z_OK) {
			throw std::runtime_error("Decompression error, zlib returned " + std::to_string(result));
		}
	}
	else if(compression.empty()) {
		stringData = compressedData;
	}
	else {
		throw std::runtime_error("Decompression error, unknown compression method");
	}

	std::istringstream dataStream(stringData);

	bool finished = false;
	for(std::size_t z = 0u; z < resource.field.getDepth() && !finished; z++) {
		for(std::size_t y = 0u; y < resource.field.getHeight() && !finished; y++) {
			for(std::size_t x = 0u; x < resource.field.getWidth() && !finished; x++) {
				if(dataStream.eof()) {
					finished = true;
					break;
				}

				vec3d vector = vec3d(0.0);
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