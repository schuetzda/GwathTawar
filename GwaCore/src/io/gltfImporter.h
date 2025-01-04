#pragma once
#include "resources/ResourceManager.h"
#include <filesystem>
namespace gwa
{
	class gltfImporter
	{
	public:
       static bool loadResource(const ResourceManager& resourceManager, const std::filesystem::path& assetDirectory,
            const std::string& gltfFileName, std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices);
	private:
        template <typename InType, typename OutType>
        static void convertToVector(const std::vector<std::byte>& originalData, std::vector<OutType>& resultData, size_t numElements, size_t offset = 0) {
            assert(offset + numElements * sizeof(InType) <= originalData.size());
            resultData.reserve(numElements);

            const InType* ptr = reinterpret_cast<const InType*>(originalData.data()+ offset);

            for (size_t i = 0; i < numElements; ++i) {
                resultData.push_back(static_cast<OutType>(ptr[i]));
            }
        }
    };
}