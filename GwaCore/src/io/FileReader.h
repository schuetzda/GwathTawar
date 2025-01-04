#pragma once
#include <vector>
#include <string>

#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <iostream>
#include <cassert>
namespace gwa 
{
	class FileReader
	{
	public:
        template<typename T>
        static std::vector<T> readBinaryFile(const std::filesystem::path& filename, std::optional<size_t> offset = std::nullopt, std::optional<size_t> size = std::nullopt)
        {
            std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);

            if (!file.is_open())
            {
                throw std::runtime_error("Failed to open a file!");
            }

            size_t fileSize = (size_t)file.tellg();

            // Determine read offset and size
            size_t startOffset = offset.value_or(0);
            size_t readSize = size.value_or(fileSize - startOffset);

            // Check for size constraints
            if (startOffset + readSize > fileSize)
            {
                std::cerr << "File size: " << fileSize << " is smaller than expected size: " << readSize << std::endl;
                assert(false);
            }

            std::vector<T> fileBuffer(readSize / sizeof(T));

            file.seekg(startOffset);

            file.read(std::bit_cast<char*>(fileBuffer.data()), readSize);

            // Close stream
            file.close();

            return fileBuffer;
        }
	};
}
