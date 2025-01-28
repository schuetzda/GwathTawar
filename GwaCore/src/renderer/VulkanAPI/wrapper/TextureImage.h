#pragma once
#include <filesystem>
namespace gwa
{
	class TextureImage
	{
	public:
		TextureImage() = default;
		TextureImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice);
	private:
		void createTextureImage(std::filesystem::path path);
		
		VkDevice logicalDevice_{0};
	};
}
