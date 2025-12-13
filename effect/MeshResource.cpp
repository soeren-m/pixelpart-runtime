#include "MeshResource.h"
#include "../common/Serialization.h"
#include "../common/Compression.h"
#include <cstddef>
#include <string>
#include <sstream>
#include <utility>

namespace pixelpart {
MeshResource::MeshResource(const std::string& name) : Resource(name) {

}

std::vector<std::uint32_t>& MeshResource::faces() {
	return meshFaces;
}
const std::vector<std::uint32_t>& MeshResource::faces() const {
	return meshFaces;
}

std::vector<glm::vec3>& MeshResource::positions() {
	return meshPositions;
}
const std::vector<glm::vec3>& MeshResource::positions() const {
	return meshPositions;
}

std::vector<glm::vec3>& MeshResource::normals() {
	return meshNormals;
}
const std::vector<glm::vec3>& MeshResource::normals() const {
	return meshNormals;
}

std::vector<glm::vec2>& MeshResource::textureCoords() {
	return meshTextureCoords;
}
const std::vector<glm::vec2>& MeshResource::textureCoords() const {
	return meshTextureCoords;
}

std::pair<std::string, std::size_t> serializeIntegerData(const std::vector<std::uint32_t>& data) {
	std::string stringData;
	for(std::size_t dataIndex = 0; dataIndex < data.size(); dataIndex++) {
		stringData += serializeInt(data[dataIndex]);

		if(dataIndex + 1 < data.size()) {
			stringData += ' ';
		}
	}

	return std::make_pair(
		compressBase64(reinterpret_cast<const std::uint8_t*>(stringData.data()), stringData.size(), CompressionMethod::zlib),
		stringData.size());
}
std::pair<std::string, std::size_t> serializeVectorData(const std::vector<glm::vec2>& data) {
	std::string stringData;
	for(std::size_t dataIndex = 0; dataIndex < data.size(); dataIndex++) {
		stringData += serializeFloat(data[dataIndex].x, 6);
		stringData += ' ';

		stringData += serializeFloat(data[dataIndex].y, 6);
		if(dataIndex + 1 < data.size()) {
			stringData += ' ';
		}
	}

	return std::make_pair(
		compressBase64(reinterpret_cast<const std::uint8_t*>(stringData.data()), stringData.size(), CompressionMethod::zlib),
		stringData.size());
}
std::pair<std::string, std::size_t> serializeVectorData(const std::vector<glm::vec3>& data) {
	std::string stringData;
	for(std::size_t dataIndex = 0; dataIndex < data.size(); dataIndex++) {
		stringData += serializeFloat(data[dataIndex].x, 6);
		stringData += ' ';

		stringData += serializeFloat(data[dataIndex].y, 6);
		stringData += ' ';

		stringData += serializeFloat(data[dataIndex].z, 6);
		if(dataIndex + 1 < data.size()) {
			stringData += ' ';
		}
	}

	return std::make_pair(
		compressBase64(reinterpret_cast<const std::uint8_t*>(stringData.data()), stringData.size(), CompressionMethod::zlib),
		stringData.size());
}

void deserializeIntegerData(std::vector<std::uint32_t>& integerData, const std::string& compressedData, std::size_t uncompressedSize, CompressionMethod compressionMethod) {
	std::vector<std::uint8_t> uncompressedData = decompressBase64(compressedData, uncompressedSize, compressionMethod);
	std::istringstream dataStream(std::string(reinterpret_cast<const char*>(uncompressedData.data()), uncompressedData.size()));

	while(!dataStream.eof()) {
		std::string token;
		std::getline(dataStream, token, ' ');

		integerData.push_back(deserializeInt<std::uint32_t>(token));
	}
}
void deserializeVectorData(std::vector<glm::vec2>& vectorData, const std::string& compressedData, std::size_t uncompressedSize, CompressionMethod compressionMethod) {
	std::vector<std::uint8_t> uncompressedData = decompressBase64(compressedData, uncompressedSize, compressionMethod);
	std::istringstream dataStream(std::string(reinterpret_cast<const char*>(uncompressedData.data()), uncompressedData.size()));

	while(!dataStream.eof()) {
		glm::vec2 vector = glm::vec2(0.0);
		std::string token;

		std::getline(dataStream, token, ' ');
		vector.x = deserializeFloat<float>(token);

		std::getline(dataStream, token, ' ');
		vector.y = deserializeFloat<float>(token);

		vectorData.push_back(vector);
	}
}
void deserializeVectorData(std::vector<glm::vec3>& vectorData, const std::string& compressedData, std::size_t uncompressedSize, CompressionMethod compressionMethod) {
	std::vector<std::uint8_t> uncompressedData = decompressBase64(compressedData, uncompressedSize, compressionMethod);
	std::istringstream dataStream(std::string(reinterpret_cast<const char*>(uncompressedData.data()), uncompressedData.size()));

	while(!dataStream.eof()) {
		glm::vec3 vector = glm::vec3(0.0);
		std::string token;

		std::getline(dataStream, token, ' ');
		vector.x = deserializeFloat<float>(token);

		std::getline(dataStream, token, ' ');
		vector.y = deserializeFloat<float>(token);

		std::getline(dataStream, token, ' ');
		vector.z = deserializeFloat<float>(token);

		vectorData.push_back(vector);
	}
}

void to_json(nlohmann::ordered_json& j, const MeshResource& resource) {
	std::pair<std::string, std::size_t> serializedFaces = serializeIntegerData(resource.faces());
	std::pair<std::string, std::size_t> serializedPositions = serializeVectorData(resource.positions());
	std::pair<std::string, std::size_t> serializedNormals = serializeVectorData(resource.normals());
	std::pair<std::string, std::size_t> serializedTextureCoords = serializeVectorData(resource.textureCoords());

	j = nlohmann::ordered_json{
		{ "name", resource.name() },
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
	resource = MeshResource(
		j.value("name", "unknown"));

	CompressionMethod compressionMethod = j.value("compression", CompressionMethod::none);

	deserializeIntegerData(resource.faces(), j.at("faces"), j.at("faces_uncompressed_size"), compressionMethod);
	deserializeVectorData(resource.positions(), j.at("positions"), j.at("positions_uncompressed_size"), compressionMethod);
	deserializeVectorData(resource.normals(), j.at("normals"), j.at("normals_uncompressed_size"), compressionMethod);
	deserializeVectorData(resource.textureCoords(), j.at("texture_coords"), j.at("texture_coords_uncompressed_size"), compressionMethod);
}
}
