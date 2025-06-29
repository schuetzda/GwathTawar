#pragma once
#include <filesystem>
#include "ecs/Registry.h"
#include "ecs/components/RenderObjects.h"

namespace gwa
{
    struct StringViewHash {
        using is_transparent = void;  
        size_t operator()(std::string_view sv) const noexcept {
            return std::hash<std::string_view>{}(sv);
        }
    };
	class gltfImporter
	{
	public:
       static uint32_t loadResource(gwa::ntity::Registry& registry, const std::filesystem::path& assetDirectory,
            const std::string& gltfFileName);
	private:
        template <typename InType, typename OutType>
        static void convertToVector(const std::vector<std::byte>& originalData, std::vector<OutType>& resultData, size_t numElements, size_t offset = 0) {
            assert(offset + numElements * sizeof(InType) <= originalData.size());

            const InType* ptr = reinterpret_cast<const InType*>(originalData.data()+ offset);
            if constexpr (std::is_same_v<InType, OutType>)
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