#pragma once
#include "resources/ResourceManager.h"
#include <filesystem>
#include "ecs/Registry.h"
#include "resources/ResourceManager.h"
namespace gwa
{
	class gltfImporter
	{
	public:
       static bool loadResource(gwa::ntity::Registry& registry, const std::filesystem::path& assetDirectory,
            const std::string& gltfFileName);
	private:
        template <typename InType, typename OutType>
        static void convertToVector(const std::vector<std::byte>& originalData, std::vector<OutType>& resultData, size_t numElements, size_t offset = 0) {
            assert(offset + numElements * sizeof(InType) <= originalData.size());

            const InType* ptr = reinterpret_cast<const InType*>(originalData.data()+ offset);
            if constexpr (std::is_same<InType, OutType>::is_same())
            {
                resultData = std::vector<OutType>(ptr, ptr + numElements);
            }
            else
            {
                resultData.resize(numElements);
                for (size_t i = 0; i < numElements; ++i) {
                    resultData[i] = static_cast<OutType>(ptr[i]);
                }
            }
        }
    };
}