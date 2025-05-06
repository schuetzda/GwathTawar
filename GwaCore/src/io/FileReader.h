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

            assert(file.is_open());

            size_t fileSize = (size_t)file.tellg();

            size_t startOffset = offset.value_or(0);
            size_t readSize = size.value_or(fileSize - startOffset);

            if (startOffset + readSize > fileSize)
            {
                std::cerr << "File size: " << fileSize << " is smaller than expected size: " << readSize << std::endl;
                assert(false);
            }

            std::vector<T> fileBuffer(readSize / sizeof(T));

            file.seekg(startOffset);

            file.read(std::bit_cast<char*>(fileBuffer.data()), readSize);
            file.close();

            return fileBuffer;
        }
	};
}
