 project "GwaCore"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"src/**.h",
		"src/**.cpp",
		"externals/stb_image/**.h"
	}
	
	includedirs
	{
            "src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.cgltf}",
		"%{IncludeDir.VulkanSDK}"
	}
	
	links
	{
		"GLFW",
		"%{Library.Vulkan}",
		"ImGui"
	}
	
	filter "system:windows"
		systemversion "latest"
		defines 
		{
			"GWA_PLATFORM_WINDOWS",
			"GWA_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "GWA_DEBUG"
		runtime "Debug"
		symbols "on"



	filter "configurations:Release"
		defines "GWA_RELEASE"
		runtime "Release"
		optimize "on"

