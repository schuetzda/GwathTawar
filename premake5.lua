workspace "GwathTawar"
	architecture "x86_64"
	toolset "v143"
	startproject "SimpleProject"
	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}
	
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	
	VULKAN_SDK = os.getenv("VULKAN_SDK")

	IncludeDir = {}
	IncludeDir["stb_image"] = "%{wks.location}/GwaCore/externals/stb_image"
	IncludeDir["GLFW"] = "%{wks.location}/GwaCore/externals/GLFW/include"
	IncludeDir["glm"] = "%{wks.location}/GwaCore/externals/glm"
    	IncludeDir["cgltf"] = "%{wks.location}/GwaCore/externals/cgltf"
	IncludeDir["ImGui"] = "%{wks.location}/GwaCore/externals/imgui"
	IncludeDir["SPIRV_Cross"] = "%{wks.location}/GwaCore/externals/SPIRV-Cross"
	IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

	LibraryDir = {}

	LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

	Library = {}

	Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
	Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

	Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
	Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
	
	group "Dependencies"
		include "GwaCore/externals/GLFW"
		include "GwaCore/externals/imgui"
	group "Core"
		include "GwaCore"
	group "Application"
		include "SimpleProject"
