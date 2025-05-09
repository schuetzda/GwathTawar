#include "VulkanImguiIntegration.h"
#include <imgui_impl_vulkan.h>
#include <cassert>
#include "QueueFamilyIndices.h"
#include <imgui_impl_glfw.h>

namespace gwa
{
	VulkanImguiIntegration::VulkanImguiIntegration(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkInstance instance, VkRenderPass renderpass, VkQueue graphicsQueue)
	{
		//TODO move
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE },
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 0;
		for (VkDescriptorPoolSize& pool_size : pool_sizes)
			pool_info.maxSets += pool_size.descriptorCount;
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		assert(vkCreateDescriptorPool(logicalDevice, &pool_info, nullptr, &imguiPool) == VK_SUCCESS);

		ImGui_ImplVulkan_InitInfo init_info = {};
		//init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
		init_info.Instance = instance;
		init_info.PhysicalDevice = physicalDevice;
		init_info.Device = logicalDevice;
		init_info.QueueFamily = QueueFamilyIndices::getQueueFamilyIndices(physicalDevice, surface).graphicsFamily;
		init_info.Queue = graphicsQueue;
		init_info.PipelineCache = VK_NULL_HANDLE; //NOTE maybe has to change
		init_info.DescriptorPool = imguiPool;
		init_info.RenderPass = renderpass;
		init_info.Subpass = 0;
		init_info.MinImageCount = 2;
		init_info.ImageCount = 2;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		ImGui_ImplVulkan_Init(&init_info);
	}
	void VulkanImguiIntegration::cleanup(VkDevice logicalDevice)
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		vkDestroyDescriptorPool(logicalDevice, imguiPool, nullptr);
	}
	void VulkanImguiIntegration::updatePlatform()
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
	void VulkanImguiIntegration::renderData(VkCommandBuffer commandBuffer)
	{
		auto drawData = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
	}
}
