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
	IncludeDir["SPIRV_Cross"] = "%{wks.location}/GwaCore/externals/SPIRV-Cross"
	IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

	LibraryDir = {}

	LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

	Library = {}

	Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
	Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

	Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
	Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
	Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
	Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

	Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
	Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
	Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
	

	
	group "Dependencies"
		include "GwaCore/externals/GLFW"
	group "Core"
		include "GwaCore"
	group "Application"
		include "SimpleProject"
