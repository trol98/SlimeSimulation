workspace "SlimeSimulation"
	architecture "x86"
	startproject "SlimeSimulation"

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

--Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Dependencies/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Dependencies/Glad/include"


project "SlimeSimulation"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")


	files
	{	
		"%{wks.location}/SlimeSimulation/res/**.**",
		"%{wks.location}/SlimeSimulation/src/*.h",
		"%{wks.location}/SlimeSimulation/src/*.cpp",
		"%{wks.location}/SlimeSimulation/src/vendor/glm/glm/**.hpp",
		"%{wks.location}/SlimeSimulation/src/vendor/glm/glm/**.inl",
		"%{wks.location}/SlimeSimulation/src/vendor/stb_image/*.h",
		"%{wks.location}/SlimeSimulation/src/vendor/stb_image/*.cpp"
	}

	defines
	{
		"GLEW_STATIC"
	}

	includedirs
	{
		"vendor",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}"
	}

	links
	{
		"GLFW",
		"Glad",
		"OpenGL32.lib",
		"User32.lib",
		"gdi32.lib",
		"Shell32.lib"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

group "Dependencies"
	include "SlimeSimulation/src/vendor/premake"
	include "Dependencies/GLFW"
	include "Dependencies/Glad"
	include "SlimeSimulation/src/vendor/imgui"
group ""




