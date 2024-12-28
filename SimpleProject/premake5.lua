project "SimpleProject"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}
	
	includedirs
	{
		"%{wks.location}/GwaCore/src",
		"%{IncludeDir.glm}"

	}

	links 
	{
		"GwaCore"
	}
		
	filter "system:windows"
		systemversion "latest"
		

	filter "configurations:Debug"
		defines "GWA_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Dev"
		defines "GWA_DEV"
		runtime "Release"
		optimize "on"

