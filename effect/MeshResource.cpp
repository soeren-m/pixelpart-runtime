#include "MeshResource.h"
#include "../common/JsonUtil.h"
#include "../common/Base64.h"
#include "../zlib/zlib.h"
#include <utility>

namespace pixelpart {
std::string compressString(const std::string& stringData) {
	uLong compressedSize = compressBound(static_cast<uLong>(stringData.size()));
	std::vector<Byte> compressedData(compressedSize);

	int result = compress(
		reinterpret_cast<Bytef*>(compressedData.data()),
		reinterpret_cast<uLongf*>(&compressedSize),
		reinterpret_cast<const Bytef*>(stringData.data()),
		static_cast<uLong>(stringData.size()));

	if(result != Z_OK) {
		throw std::runtime_error("Compression error, zlib returned " + std::to_string(result));
	}

	return encodeBase64(compressedData.data(), compressedData.size());
}
std::string decompressString(const std::string& compressedData, std::size_t uncompressedSize) {
	std::string data;
	data.resize(uncompressedSize);

	uLong expectedUncompressedSize = uncompressedSize;

	int result = uncompress(
		reinterpret_cast<Bytef*>(data.data()),
		reinterpret_cast<uLongf*>(&expectedUncompressedSize),
		reinterpret_cast<const Bytef*>(compressedData.data()),
		static_cast<uLong>(compressedData.size()));

	if(result != Z_OK) {
		throw std::runtime_error("Decompression error, zlib returned " + std::to_string(result));
	}

	return data;
}

std::pair<std::string, std::size_t> serializeIntegerData(const std::vector<uint32_t>& data) {
	std::string stringData;
	for(std::size_t i = 0u; i < data.size(); i++) {
		stringData += std::to_string(data[i]);

		if(i + 1u < data.size()) {
			stringData += ' ';
		}
	}

	return std::make_pair(
		compressString(stringData),
		stringData.size());
}
std::pair<std::string, std::size_t> serializeVectorData(const std::vector<glm::vec2>& data) {
	std::string stringData;
	for(std::size_t i = 0u; i < data.size(); i++) {
		stringData += std::to_string(data[i].x);
		stringData += ' ';

		stringData += std::to_string(data[i].y);
		if(i + 1u < data.size()) {
			stringData += ' ';
		}
	}

	return std::make_pair(
		compressString(stringData),
		stringData.size());
}
std::pair<std::string, std::size_t> serializeVectorData(const std::vector<glm::vec3>& data) {
	std::string stringData;
	for(std::size_t i = 0u; i < data.size(); i++) {
		stringData += std::to_string(data[i].x);
		stringData += ' ';

		stringData += std::to_string(data[i].y);
		stringData += ' ';

		stringData += std::to_string(data[i].z);
		if(i + 1u < data.size()) {
			stringData += ' ';
		}
	}

	return std::make_pair(
		compressString(stringData),
		stringData.size());
}

void deserializeIntegerData(std::vector<uint32_t>& integerData, const std::string& stringData) {
	std::istringstream dataStream(stringData);

	while(!dataStream.eof()) {
		std::string token;
		std::getline(dataStream, token, ' ');

		integerData.push_back(static_cast<uint32_t>(std::stoul(token)));
	}
}
void deserializeVectorData(std::vector<glm::vec2>& vectorData, const std::string& stringData) {
	std::istringstream dataStream(stringData);

	while(!dataStream.eof()) {
		glm::vec2 vector = glm::vec2(0.0);
		std::string token;

		std::getline(dataStream, token, ' ');
		vector.x = std::stof(token);

		std::getline(dataStream, token, ' ');
		vector.y = std::stof(token);

		vectorData.push_back(vector);
	}
}
void deserializeVectorData(std::vector<glm::vec3>& vectorData, const std::string& stringData) {
	std::istringstream dataStream(stringData);

	while(!dataStream.eof()) {
		glm::vec3 vector = glm::vec3(0.0);
		std::string token;

		std::getline(dataStream, token, ' ');
		vector.x = std::stof(token);

		std::getline(dataStream, token, ' ');
		vector.y = std::stof(token);

		std::getline(dataStream, token, ' ');
		vector.z = std::stof(token);

		vectorData.push_back(vector);
	}
}

void to_json(nlohmann::ordered_json& j, const MeshResource& resource) {
	std::pair<std::string, std::size_t> serializedFaces = serializeIntegerData(resource.faces);
	std::pair<std::string, std::size_t> serializedPositions = serializeVectorData(resource.positions);
	std::pair<std::string, std::size_t> serializedNormals = serializeVectorData(resource.normals);
	std::pair<std::string, std::size_t> serializedTextureCoords = serializeVectorData(resource.textureCoords);

	j = nlohmann::ordered_json{
		{ "name", resource.name },
		{ "faces", serializedFaces.first },
		{ "faces_uncompressed_size", serializedFaces.second },
		{ "positions", serializedPositions.first },
		{ "positions_uncompressed_size", serializedPositions.second },
		{ "normals", serializedNormals.first },
		{ "normals_uncompressed_size", serializedNormals.second },
		{ "texture_coords", serializedTextureCoords.first },
		{ "texture_coords_uncompressed_size", serializedTextureCoords.second },
		{ "compression", "zlib" }
	};
}
void from_json(const nlohmann::ordered_json& j, MeshResource& resource) {
	resource = MeshResource();

	fromJson(resource.name, j, "name");

	std::string compression;
	fromJson(compression, j, "compression");

	std::string compressedFaceData = decodeBase64(j.at("faces").get<std::string>());
	std::string compressedPositionData = decodeBase64(j.at("positions").get<std::string>());
	std::string compressedNormalData = decodeBase64(j.at("normals").get<std::string>());
	std::string compressedTextureCoordData = decodeBase64(j.at("texture_coords").get<std::string>());

	std::string uncompressedFaceData;
	std::string uncompressedPositionData;
	std::string uncompressedNormalData;
	std::string uncompressedTextureCoordData;
	if(compression == "zlib") {
		uncompressedFaceData = decompressString(compressedFaceData, j.at("faces_uncompressed_size").get<std::size_t>());
		uncompressedPositionData = decompressString(compressedPositionData, j.at("positions_uncompressed_size").get<std::size_t>());
		uncompressedNormalData = decompressString(compressedNormalData, j.at("normals_uncompressed_size").get<std::size_t>());
		uncompressedTextureCoordData = decompressString(compressedTextureCoordData, j.at("texture_coords_uncompressed_size").get<std::size_t>());
	}
	else if(compression.empty()) {
		uncompressedFaceData = compressedFaceData;
		uncompressedPositionData = compressedPositionData;
		uncompressedNormalData = compressedNormalData;
		uncompressedTextureCoordData = compressedTextureCoordData;
	}
	else {
		throw std::runtime_error("Decompression error, unknown compression method");
	}

	deserializeIntegerData(resource.faces, uncompressedFaceData);
	deserializeVectorData(resource.positions, uncompressedPositionData);
	deserializeVectorData(resource.normals, uncompressedNormalData);
	deserializeVectorData(resource.textureCoords, uncompressedTextureCoordData);
}
}