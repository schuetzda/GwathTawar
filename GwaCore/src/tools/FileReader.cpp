#include "FileReader.h"

#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
namespace gwa {
	std::vector<char> readBinaryFile(const std::string& filename)
	{
		std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);

		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open a file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> fileBuffer(fileSize);

		file.seekg(0);

		file.read(fileBuffer.data(), fileSize);

		// Close stream
		file.close();

		return fileBuffer;
	}
}