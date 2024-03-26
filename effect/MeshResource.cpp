#include "MeshResource.h"
#include "../common/Compression.h"
#include "../common/Json.h"
#include <utility>

namespace pixelpart {
std::pair<std::string, std::size_t> serializeIntegerData(const std::vector<uint32_t>& data) {
	std::string stringData;
	for(std::size_t i = 0u; i < data.size(); i++) {
		stringData += std::to_string(data[i]);

		if(i + 1u < data.size()) {
			stringData += ' ';
		}
	}

	return std::make_pair(
		compressAndEncodeString(stringData, CompressionMethod::zlib),
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
		compressAndEncodeString(stringData, CompressionMethod::zlib),
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
		compressAndEncodeString(stringData, CompressionMethod::zlib),
		stringData.size());
}

void deserializeIntegerData(std::vector<uint32_t>& integerData, const std::string& compressedData, std::size_t uncompressedSize, CompressionMethod compressionMethod) {
	std::string stringData = decodeAndDecompressToString(compressedData, uncompressedSize, compressionMethod);
	std::istringstream dataStream(stringData);

	while(!dataStream.eof()) {
		std::string token;
		std::getline(dataStream, token, ' ');

		integerData.push_back(static_cast<uint32_t>(std::stoul(token)));
	}
}
void deserializeVectorData(std::vector<glm::vec2>& vectorData, const std::string& compressedData, std::size_t uncompressedSize, CompressionMethod compressionMethod) {
	std::string stringData = decodeAndDecompressToString(compressedData, uncompressedSize, compressionMethod);
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
void deserializeVectorData(std::vector<glm::vec3>& vectorData, const std::string& compressedData, std::size_t uncompressedSize, CompressionMethod compressionMethod) {
	std::string stringData = decodeAndDecompressToString(compressedData, uncompressedSize, compressionMethod);
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
		{ "compression", CompressionMethod::zlib }
	};
}
void from_json(const nlohmann::ordered_json& j, MeshResource& resource) {
	resource = MeshResource();

	fromJson(resource.name, j, "name");

	CompressionMethod compressionMethod = CompressionMethod::none;
	fromJson(compressionMethod, j, "compression");

	deserializeIntegerData(resource.faces, j.at("faces"), j.at("faces_uncompressed_size"), compressionMethod);
	deserializeVectorData(resource.positions, j.at("positions"), j.at("positions_uncompressed_size"), compressionMethod);
	deserializeVectorData(resource.normals, j.at("normals"), j.at("normals_uncompressed_size"), compressionMethod);
	deserializeVectorData(resource.textureCoords, j.at("texture_coords"), j.at("texture_coords_uncompressed_size"), compressionMethod);
}
}